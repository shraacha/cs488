// Termm--Fall 2024

#include <cmath>
#include <limits>
#include <optional>
#include <cstdlib>

#include <glm/ext.hpp>

#include "ProgressBar.hpp"
#include "cs488-framework/MathUtils.hpp"

#include "A5.hpp"
#include "Primitive.hpp"
#include "SceneManager.hpp"
#include "debug.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
#include "PhongMaterial.hpp"
#include "CookTorranceMaterial.hpp"
#include "NormalMaterial.hpp"

#include "Helpers.hpp"
#include "ImageHelpers.hpp"
#include "LightingHelpers.hpp"
#include "glm/detail/type_vec.hpp"
#include "glm/gtx/string_cast.hpp"

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

static inline std::vector<glm::dvec4>
generateSubScreenPositions(const glm::vec4 &screenPosition, unsigned int wSubDivs = 1,
                           unsigned int hSubDivs = 1, bool jitter = true) {
    std::vector<glm::dvec4> positions;

    glm::vec4 bottomLeft(screenPosition.x - 0.5, screenPosition.y - 0.5, screenPosition.z, 1.0);

    double wStep = 1.0 / (double)(wSubDivs);
    double hStep = 1.0 / (double)(hSubDivs);

    double wHalfStep = wStep / 2.0;
    double hHalfStep = hStep / 2.0;

    glm::vec4 startLocation(bottomLeft.x + wHalfStep, bottomLeft.y + hHalfStep, bottomLeft.z, bottomLeft.w);
    double row = startLocation.y;
    double startCol = startLocation.x;

    for (unsigned int i = 0; i < hSubDivs; ++i) {
        double col = startCol;
        for (unsigned int j = 0; j < wSubDivs; ++j) {
            double wJitter = jitter ? ((static_cast<double>(std::rand()) / RAND_MAX) * wStep) - wHalfStep : 0;
            double hJitter = jitter ? ((static_cast<double>(std::rand()) / RAND_MAX) * hStep) - hHalfStep : 0;

            positions.emplace_back(col + hJitter, row + wJitter, startLocation.z,  startLocation.w);

            row += wStep;
        }
        col += hStep;
    }

    return positions;
}

static inline std::optional<std::pair<Intersection, Material *>>
intersect(const SceneManager & sceneManager, const Ray & ray) {
    std::optional<std::pair<Intersection, Material *>> intersectionAndMaterial{
        std::nullopt};

    for (SceneManager::PreOrderTraversalIterator nodeIt = sceneManager.begin();
         nodeIt != sceneManager.end(); ++nodeIt) {

        if (nodeIt->m_nodeType != NodeType::GeometryNode)
            continue;

        const GeometryNode *geometryNode = nodeIt.asGeometryNode();

        glm::mat4 transformationStack =
            nodeIt.getInheritedTransformation() * geometryNode->get_transform();

        glm::dvec4 transformedEye =
            glm::inverse(transformationStack) * ray.getEyePoint();
        glm::dvec4 transformedPixel =
            glm::inverse(transformationStack) * ray.getPixelPoint();

        auto result = geometryNode->getPrimitive()->intersect(
            Ray(transformedEye, transformedPixel));

        if (result.has_value() &&
            (!intersectionAndMaterial.has_value() ||
             (result->getT() < intersectionAndMaterial->first.getT())) &&
            result->getT() >= ray.getMinThreshold()) {
            intersectionAndMaterial = std::make_pair<Intersection, Material *>(
                Intersection(
                    result->getT(), transformationStack * result->getPosition(),
                    result->getNormal() * glm::inverse(transformationStack)),
                geometryNode->getMaterial());
        }
    }

    return intersectionAndMaterial;
}

static inline glm::dvec3 calculateColour(const Ray &ray, const Intersection &intersect,
                                   Material *material, const glm::vec3 &ambient,
                                   const std::vector<const Light *> &lights) {
    glm::dvec3 colour{1.0, 1.0, 1.0};

    if (material->isReflective() && material->isRefractive())
    {
        colour = material->getRadiance(ray, intersect, ambient, lights);
    } else if (material->isReflective()) {
        colour = material->getRadiance(ray, intersect, ambient, lights);
    } else {
        colour = material->getRadiance(ray, intersect, ambient, lights);
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

static glm::dvec3
intersectAndGetColour(const SceneManager & sceneManager, const Ray & ray,
                      const std::vector<const Light *> & lights,
                      const glm::dvec3 & ambient, const double & ior,
                      unsigned int currDepth, unsigned int maxDepth)
{
    glm::dvec3 fragmentColour = glm::dvec3(0.0, 0.0, 0.0);

    auto eyeRayIntersectionAndMaterial = intersect(sceneManager, ray);

    if (eyeRayIntersectionAndMaterial.has_value())
    {
        std::vector<const Light *> contributingLights;

        for (const Light *light : lights)
        {
            auto shadowRayIntersectionAndMaterial = intersect(
                sceneManager,
                Ray(eyeRayIntersectionAndMaterial->first.getPosition(),
                    glm::vec4(light->position, 1.0)));

            if (!shadowRayIntersectionAndMaterial.has_value())
            {
                contributingLights.emplace_back(light);
            }
            else
            {
                double lenToShadowing = glm::length(
                    glm::dvec3(
                        shadowRayIntersectionAndMaterial->first.getPosition()) -
                    glm::dvec3(
                        eyeRayIntersectionAndMaterial->first.getPosition()));
                double lenToLight = glm::length(
                    glm::dvec3(light->position) -
                    glm::dvec3(
                        eyeRayIntersectionAndMaterial->first.getPosition()));

                if (lenToLight < lenToShadowing)
                {
                    contributingLights.emplace_back(light);
                }
            }
        }

        Intersection & intersect = eyeRayIntersectionAndMaterial->first;
        Material * material = eyeRayIntersectionAndMaterial->second;

        if (currDepth >= maxDepth)
        {
            // direct lighting if max depth or if there are no
            // reflective/refractive properties
            fragmentColour += material->getRadiance(ray, intersect, ambient,
                                                    contributingLights);
        }
        else
        {
            glm::dvec3 reflectionDir, reflectionRadiance, refractionDir,
                refractionRadiance;

            if (material->isReflective())
            {
                reflectionDir =
                    material
                        ->sampleReflectionDirection(
                            glm::normalize(glm::dvec3(ray.getDirection())),
                            intersect.getNormalizedNormal())
                        .first;

                reflectionRadiance = intersectAndGetColour(
                    sceneManager,
                    Ray(intersect.getPosition(),
                        intersect.getPosition() +
                            glm::dvec4(reflectionDir, 0.0)),
                    lights, ambient, ior, currDepth + 1, maxDepth);
            }

            if (material->isRefractive())
            {
                refractionDir =
                    material
                        ->sampleRefractionDirection(
                            glm::normalize(glm::dvec3(ray.getDirection())),
                            intersect.getNormalizedNormal(), ior)
                        .first;

                refractionRadiance = intersectAndGetColour(
                    sceneManager,
                    Ray(intersect.getPosition(),
                        intersect.getPosition() +
                            glm::dvec4(refractionDir, 0.0)),
                    lights, ambient, ior, currDepth + 1,
                    maxDepth);
            }

            fragmentColour += material->getRadiance(
                ray, intersect, ambient, contributingLights, reflectionDir,
                reflectionRadiance, refractionDir, refractionRadiance);
        }
    }
    else
    {
        fragmentColour +=
            getBackgroundColour(glm::dvec3(ray.getDirection()),
                                c_topScreenColour, c_botScreenColour);
    }

    return fragmentColour;
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
    SceneNode *root,

    // Image to write to, set to a given width and height
    Image & image,

    // Viewing parameters
    const glm::vec3 & eye, const glm::vec3 & view, const glm::vec3 & up,
    double fovy,

    // Lighting parameters
    const glm::vec3 & ambient, const std::list<Light *> & lights)
{
    unsigned int numSamples = 5;

    std::vector<const Light *> lightVector;

    for (const Light *light : lights)
    {
        lightVector.emplace_back(light);
    }

    printSceneInfo(image.width(), image.height(), *root, eye, view, up, fovy,
                   ambient, lights);

    SceneManager sceneManager;
    sceneManager.importSceneGraph(root);

    size_t h = image.height();
    size_t w = image.width();

    double zval = getScreenDepth(h, fovy);
    glm::dvec4 viewSpacePixel{0.0, 0.0, -zval, 1.0};
    glm::dvec4 viewSpaceEye{0.0, 0.0, 0.0, 1.0};

    glm::mat4 viewMatrix = glm::lookAt(eye, view, up);

    ProgressBar progressBar(h * w);
    std::cout << progressBar;

    for (uint y = 0; y < h; ++y)
    {
        viewSpacePixel.y = getScreenPosition(h, y, true);

        for (uint x = 0; x < w; ++x)
        {
            viewSpacePixel.x = getScreenPosition(w, x);

            // get vector of subpixels
            std::vector<glm::dvec4> subPixels =
                generateSubScreenPositions(viewSpacePixel);

            glm::dvec3 pixelColour = glm::dvec3(0.0, 0.0, 0.0);

            // iterate over subpixels, add to total
            for (auto subPixel : subPixels)
            {
                glm::dvec4 worldSpaceEye =
                    glm::inverse(viewMatrix) * viewSpaceEye;
                glm::dvec4 worldSpacePixel =
                    glm::inverse(viewMatrix) * subPixel;

                for (unsigned int i = 0; i < numSamples; i++) {
                    pixelColour += intersectAndGetColour(
                        sceneManager, Ray(worldSpaceEye, worldSpacePixel),
                        lightVector, ambient, 1.0, 0, 2);
                }
            }

            // average pixel vals
            pixelColour *= 1.0 / ((double)subPixels.size() * (double)numSamples);

            // set colour
            setPixelColour(image, x, y, pixelColour);

            ++progressBar;
            progressBar.conditionalOut(std::cout);
        }
    }
    std::cout << progressBar;
}
