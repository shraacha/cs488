// Termm--Fall 2024

#include <cmath>
#include <limits>
#include <optional>

#include <glm/ext.hpp>

#include "cs488-framework/MathUtils.hpp"

#include "A4.hpp"
#include "PhongMaterial.hpp"
#include "Primitive.hpp"
#include "RayIntersect.hpp"
#include "SceneManager.hpp"
#include "ImageHelpers.hpp"
#include "debug.hpp"

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

static inline std::optional<std::pair<double, glm::dvec4>>
intersect(Primitive *primitive, const glm::dvec4 eye, const glm::dvec4 pixel)
{
    std::optional<std::pair<double, glm::dvec4>> result{std::nullopt};

    // casting to derived primitive for further intersection calculation
    NonhierSphere * nhSphere = dynamic_cast<NonhierSphere *>(primitive);
    Sphere * sphere = dynamic_cast<Sphere *>(primitive);
    NonhierBox * nhBox = dynamic_cast<NonhierBox *>(primitive);
    Cube * cube = dynamic_cast<Cube *>(primitive);

    if (nhSphere) {
        result = findRaySphereIntersectAndNormal(
            eye, pixel, nhSphere->getPosAsDvec4(), nhSphere->getRadius());
    }

    if (sphere) {
        result = findRaySphereIntersectAndNormal(eye, pixel);
    }

    if (nhBox) {
        result = findRayBoxIntersectAndNormal(eye, pixel, nhBox->getPosAsDvec4(), nhBox->getSize());
    }

    if (cube) {
        result = findRayBoxIntersectAndNormal(eye, pixel);
    }
    return result;
}

static inline glm::dvec3 getColour(Material *material)
{
    glm::dvec3 colour{1.0, 1.0, 1.0};

    // casting to derived primitive
    PhongMaterial *phongMaterial = dynamic_cast<PhongMaterial *>(material);

    if (phongMaterial) {
        colour = phongMaterial->getKD();
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

    for (uint y = 0; y < h; ++y) {
        basePixel.y = getScreenPosition(h, y, true);

        for (uint x = 0; x < w; ++x) {
            basePixel.x = getScreenPosition(w, x);

            double t = std::numeric_limits<double>::max();
            glm::dvec4 normal;
            Material * material = nullptr;

            for (SceneManager::PreOrderTraversalIterator nodeIt = sceneManager.begin();
                 nodeIt != sceneManager.end(); ++nodeIt) {

                if (nodeIt->m_nodeType != NodeType::GeometryNode)
                    continue;

                const GeometryNode *geometryNode = nodeIt.asGeometryNode();

                glm::dvec4 transformedPixel =
                    geometryNode->get_inverse() *
                    glm::inverse(nodeIt.getInheritedTransformation()) *
                    glm::inverse(viewMatrix) * basePixel;
                glm::dvec4 transformedEye =
                    geometryNode->get_inverse() *
                    glm::inverse(nodeIt.getInheritedTransformation()) *
                    glm::inverse(viewMatrix) * baseEye;

                auto result = intersect(geometryNode->getPrimitive(),
                                        transformedEye, transformedPixel);

                if (result && result->first < t && result->first >= 0.0) {
                    t = result->first;
                    normal = result->second;
                    material = geometryNode->getMaterial();
                }
            }

            if (t != std::numeric_limits<double>::max())
            {
                setPixelColour(image, x, y, getColour(material));
            }
            else
            {
                setPixelColour(image, x, y,
                               getBackgroundColour(glm::dvec3(basePixel),
                                                   c_topScreenColour, c_botScreenColour, fovy));
            }
        }
    }
}
