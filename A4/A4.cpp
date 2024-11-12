// Termm--Fall 2024

#include <cmath>
#include <limits>
#include <optional>

#include <glm/ext.hpp>

#include "ProgressBar.hpp"
#include "cs488-framework/MathUtils.hpp"

#include "A4.hpp"
#include "PhongMaterial.hpp"
#include "Primitive.hpp"
#include "RayIntersect.hpp"
#include "SceneManager.hpp"
#include "ImageHelpers.hpp"
#include "debug.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

// ------------------- constants ----------------------
const glm::dvec3 c_botScreenColour = {0.89411764705, 0.58823529411, 0.80392156862};
const glm::dvec3 c_topScreenColour = {0.013725490196, 0.003529411764, 0.001176470588};


// ------------------- helpers ----------------------
// paramaeters:
// - fovy
//   fov in DEGREES
static inline double getScreenDepth(const double & height, const double & fovy)
{
  return (height / 2) / std::tan(degreesToRadians(fovy) / 2);
}

// paramaeters:
// - flip
//   whether to flip the position, this is needed when supplying an device coordinate index
static inline double getScreenPosition(const double &sideLength,
                                       const uint &index, const bool &flip = false)
{

    if (flip)
    {
        return - (double)index + sideLength / 2 - 0.5;
    } else {
        return (double)index - sideLength / 2 + 0.5;
    }
}

static inline std::optional<Intersection>
intersect(Primitive *primitive, const Ray & ray)
{
    std::optional<Intersection> result{std::nullopt};

    // casting to derived primitive for further intersection calculation
    NonhierSphere * nhSphere = dynamic_cast<NonhierSphere *>(primitive);
    Sphere * sphere = dynamic_cast<Sphere *>(primitive);
    NonhierBox * nhBox = dynamic_cast<NonhierBox *>(primitive);
    Cube * cube = dynamic_cast<Cube *>(primitive);
    Mesh * mesh = dynamic_cast<Mesh *>(primitive);

    if (nhSphere) {
        result = findRaySphereIntersection(
            ray, nhSphere->getPosAsDvec4(), nhSphere->getRadius());
    }

    if (sphere) {
        result = findRaySphereIntersection(ray);
    }

    if (nhBox) {
        result = findRayBoxIntersection(ray, nhBox->getPosAsDvec4(), nhBox->getSize());
    }

    if (cube) {
        result = findRayBoxIntersection(ray);
    }

    if (mesh) {
        result = findRayMeshIntersection(ray, *mesh);
    }

    if (result.has_value())
    {
        result->setPosition(evaluate(ray, result->getT()));
    }

    return result;
}

static inline glm::dvec3 calculatePhongLighting(const Ray &ray,
                                          const Intersection &intersect,
                                          const PhongMaterial &material,
                                          const glm::vec3 &ambient,
                                          const std::list<Light *> &lights) {
    glm::dvec3 lightOut = glm::dvec3(ambient) * material.getKD();

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());

    for (const Light *light : lights) {
        glm::dvec3 lightVector = glm::normalize(glm::dvec3(light->position) - intersectionPoint);
        double lightDotNormal = glm::dot(lightVector, intersect.getNormalizedNormal());
        glm::dvec3 reflectedVector =
            -lightVector + 2 * lightDotNormal * intersect.getNormalizedNormal();

        lightOut += lightDotNormal * material.getKD() * glm::dvec3(light->colour);
        lightOut +=
            pow(glm::dot(reflectedVector, glm::normalize(glm::dvec3(ray.getEyePoint()) -
                                                         intersectionPoint)),
                material.getShininess()) *
            material.getKS() * glm::dvec3(light->colour);
    }

    return lightOut;
}

static inline glm::dvec3 calculateColour(const Ray &ray, const Intersection &intersect,
                                   Material *material, const glm::vec3 &ambient,
                                   const std::list<Light *> &lights) {
    glm::dvec3 colour{1.0, 1.0, 1.0};

    // casting to derived primitive
    PhongMaterial *phongMaterial = dynamic_cast<PhongMaterial *>(material);

    if (phongMaterial) {
        colour = calculatePhongLighting(ray, intersect, *phongMaterial, ambient, lights);
    }

    return colour;
}

/**
 * Generates a gradient based on the normalized y-value of the ray direction.
 * We also scale by FOV so that we see the supplied colours at the top & bottom of the view frame.
 */
static inline glm::dvec3 getBackgroundColour(glm::dvec3 rayDirection,
                                             glm::vec3 topColour = glm::vec3(1.0, 1.0, 1.0),
                                             glm::vec3 botColour = glm::vec3(0.0, 0.0, 0.0),
                                             const double & fovy = 180) {
    glm::dvec3 normalizedDirection = glm::normalize(rayDirection);
    // normalized y is in [-1, 1], we need to bring it to [0, 1] otherwise we may get negative colour vals
    double t = 0.5 * (normalizedDirection.y / sin(degreesToRadians(fovy / 2)) + 1.0);
    return  t * topColour + (1.0 - t) * botColour;
}

static inline void printSceneInfo(const uint &imgWidth, const uint &imgHeight,
                                  const SceneNode &root, const glm::vec3 &eye,
                                  const glm::vec3 &view, const glm::vec3 &up,
                                  const double &fovy, const glm::vec3 &ambient,
                                  const std::list<Light *> &lights)
{
    std::cout << "F24: Calling A4_Render(\n"
              << "\t" << root << "\t"
              << "Image(width:" << imgWidth << ", height:" << imgHeight
              << ")\n"
                 "\t"
              << "eye:  " << glm::to_string(eye) << std::endl
              << "\t"
              << "view: " << glm::to_string(view) << std::endl
              << "\t"
              << "up:   " << glm::to_string(up) << std::endl
              << "\t"
              << "fovy: " << fovy << std::endl
              << "\t"
              << "ambient: " << glm::to_string(ambient) << std::endl
              << "\t"
              << "lights{" << std::endl;

    for (const Light *light : lights) {
        std::cout << "\t\t" << *light << std::endl;
    }
    std::cout << "\t}" << std::endl;
    std::cout << ")" << std::endl;
}

// ------------------- main ----------------------
void A4_Render(
    // What to render
    SceneNode * root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & eye,
    const glm::vec3 & view,
    const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient,
    const std::list<Light *> & lights
) {
    printSceneInfo(image.width(), image.height(), *root, eye, view, up, fovy,
                   ambient, lights);

    SceneManager sceneManager;
    sceneManager.importSceneGraph(root);

    size_t h = image.height();
    size_t w = image.width();

    double zval = getScreenDepth(h, fovy);
    glm::dvec4 basePixel {0.0, 0.0, -zval, 1.0};
    glm::dvec4 baseEye {0.0, 0.0, 0.0, 1.0};

    glm::mat4 viewMatrix = glm::lookAt(eye, eye + view, up);

    ProgressBar progressBar(h * w);
    std::cout << progressBar;

    for (uint y = 0; y < h; ++y) {
        basePixel.y = getScreenPosition(h, y, true);

        for (uint x = 0; x < w; ++x) {
            basePixel.x = getScreenPosition(w, x);

            Intersection intersectionWorldCoordinates{std::numeric_limits<double>::max(), glm::dvec4()};

            Material *material = nullptr;

            for (SceneManager::PreOrderTraversalIterator nodeIt = sceneManager.begin();
                 nodeIt != sceneManager.end(); ++nodeIt) {

                if (nodeIt->m_nodeType != NodeType::GeometryNode)
                    continue;

                const GeometryNode *geometryNode = nodeIt.asGeometryNode();

                glm::mat4 transformationStack = viewMatrix *
                                                nodeIt.getInheritedTransformation() *
                                                geometryNode->get_transform();

                glm::dvec4 transformedPixel = glm::inverse(transformationStack) * basePixel;
                glm::dvec4 transformedEye = glm::inverse(transformationStack) * baseEye;

                auto result = intersect(geometryNode->getPrimitive(),
                                        Ray(transformedEye, transformedPixel));

                if (result && result->getT() < intersectionWorldCoordinates.getT() && result->getT() >= 0.0) {
                    intersectionWorldCoordinates = Intersection(
                        result->getT(),
                        transformationStack * result->getPosition(),
                        result->getNormal() * glm::inverse(transformationStack));
                    material = geometryNode->getMaterial();
                }
            }


            if (intersectionWorldCoordinates.getT() != std::numeric_limits<double>::max()) {
                setPixelColour(image, x, y, calculateColour({baseEye, basePixel}, intersectionWorldCoordinates, material, ambient, lights));
            } else {
                setPixelColour(image, x, y,
                               getBackgroundColour(glm::dvec3(basePixel),
                                                   c_topScreenColour, c_botScreenColour,
                                                   fovy));
            }
            ++progressBar;
            progressBar.conditionalOut(std::cout);
        }
    }
    std::cout << progressBar;
}
