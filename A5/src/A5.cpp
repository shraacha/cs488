// Termm--Fall 2024

#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <cstdlib>
#include <algorithm>

#include <glm/ext.hpp>
#include "Material.hpp"
#include "glm/detail/func_common.hpp"
#include "glm/detail/func_vector_relational.hpp"
#include "glm/detail/type_vec.hpp"
#include "glm/gtx/string_cast.hpp"

#include "cs488-framework/MathUtils.hpp"

#include "A5.hpp"
#include "Camera.hpp"
#include "CookTorranceMaterial.hpp"
#include "Intersection.hpp"
#include "KDTree.hpp"
#include "NormalMaterial.hpp"
#include "PhongMaterial.hpp"
#include "Photon.hpp"
#include "PhotonMapHelpers.hpp"
#include "Primitive.hpp"
#include "ProgressBar.hpp"
#include "Ray.hpp"
#include "SceneManager.hpp"
#include "ScreenHelpers.hpp"
#include "ThreadPool.hpp"
#include "debug.hpp"

#include "ImageHelpers.hpp"
#include "ImageKernelHelpers.hpp" // can't include imagekernel and this directly since that ends up giving error
#include "LightingHelpers.hpp"

#include "IntersectionRoutines.hpp"

// ------------------- constants ----------------------
const glm::dvec3 c_botScreenColour = {0.89411764705, 0.58823529411, 0.80392156862};
const glm::dvec3 c_topScreenColour = {0.013725490196, 0.003529411764, 0.001176470588};


// ------------------- helpers ----------------------

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
            std::pair<glm::dvec3, double> reflectionDir, refractionDir;
            glm::dvec3 reflectionRadiance, refractionRadiance;

            if (material->isSpecularReflective())
            {
                reflectionDir = material
                        ->sampleReflectionDirection(
                            glm::normalize(glm::dvec3(ray.getDirection())),
                            intersect.getNormalizedNormal());

                // TODO probability should be used in the calculation somewhere
                reflectionRadiance = intersectAndGetColour(
                    sceneManager,
                    Ray(intersect.getPosition(),
                        intersect.getPosition() +
                            glm::dvec4(reflectionDir.first, 0.0)),
                    lights, ambient, ior, currDepth + 1, maxDepth) * (1 / reflectionDir.second);
            }

            if (material->isTransmissiveRefractive())
            {
                refractionDir =
                    material
                        ->sampleRefractionDirection(
                            glm::normalize(glm::dvec3(ray.getDirection())),
                            intersect.getNormalizedNormal(), ior) ;

                refractionRadiance = intersectAndGetColour(
                    sceneManager,
                    Ray(intersect.getPosition(),
                        intersect.getPosition() +
                            glm::dvec4(refractionDir.first, 0.0)),
                    lights, ambient, ior, currDepth + 1,
                    maxDepth);
            }

            fragmentColour += material->getRadiance(
                ray, intersect, ambient, contributingLights, reflectionDir.first,
                reflectionRadiance, refractionDir.first, refractionRadiance);
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

bool noTermination(const MaterialAction & action, const Material & material) {
    return false;
}

static std::optional<Photon>
castPhoton(const SceneManager & sceneManager, const Ray & ray, Photon & photon,
           const double & ior, unsigned int currDepth, unsigned int maxDepth,
           std::function<bool(const MaterialAction &, const Material &)> earlyTerminationCriteria = &noTermination)
{
    std::optional<std::pair<Intersection, Material *>> intersectionResult =
        intersect(sceneManager, ray);

    if (!intersectionResult)
    {
        return std::nullopt;
    }
    else
    {
        Material *material = intersectionResult.value().second;
        Intersection & intersection = intersectionResult.value().first;
        MaterialActionAndConstants materialAction = material->russianRouletteAction(
            ray.getNormalizedDirection(), intersection.getNormalizedNormal());
        std::pair<glm::dvec3, double> newDirection;

        // check early termination criteria
        if (earlyTerminationCriteria(materialAction.action, *material)) {
            return std::nullopt;
        }

        // check if the photon has been absorbed
        if (materialAction.action == MaterialAction::Absorb)
        {
            photon.setIncidenceDir(
                glm::normalize(glm::dvec3(-ray.getDirection())));
            photon.setPosition(glm::dvec3(intersection.getPosition()));

            return photon;
        } else if (currDepth == maxDepth) {
            return std::nullopt;
        }
        // specular/transmission cases
        else
        {
            if (materialAction.action == MaterialAction::Reflect)
            {
                // sample direction
                newDirection = material->sampleReflectionDirection(
                    ray.getNormalizedDirection(),
                    intersection.getNormalizedNormal());

                // scale photon power otherwise it may blow up the scene
                photon.piecewiseAverageScale(materialAction.kS);
            }
            else if (materialAction.action == MaterialAction::Transmit &&
                     material->isTransmissiveRefractive())
            {
                newDirection = material->sampleRefractionDirection(
                    ray.getNormalizedDirection(),
                    intersection.getNormalizedNormal(), ior);

                // scale photon power otherwise it may blow up the scene
                photon.piecewiseAverageScale(material->getAlbedo() *
                                             materialAction.kS);
            }
            else if (materialAction.action == MaterialAction::Transmit &&
                     material->isTransmissiveReflective())
            {
                // sample reflection direction
                newDirection = material->sampleReflectionDirection(
                    ray.getNormalizedDirection(),
                    intersection.getNormalizedNormal());

                // scale photon power otherwise it may blow up the scene
                photon.piecewiseAverageScale(material->getAlbedo() *
                                             materialAction.kD);
            }
            else
            {
                // sample diffusely
                newDirection = material->sampleDiffuseDirection(
                    ray.getNormalizedDirection(),
                    intersection.getNormalizedNormal());

                // scale photon power otherwise it may blow up the scene
                photon.piecewiseAverageScale(material->getAlbedo() *
                                             materialAction.kD);
            }

            if (glm::any(glm::isnan(photon.getPower())))
            {
                return std::nullopt;
            }
            else
            {
                return castPhoton(
                    sceneManager,
                    Ray(intersection.getPosition(), newDirection.first), photon,
                    ior, currDepth + 1, maxDepth);
            }
        }
    }
}

static KDTree<Photon, double>
createCausticPhotonMap(const SceneManager & sceneManager,
                    const std::vector<const Light *> & lights,
                    const glm::dvec3 & ambient, const double & ior,
                    unsigned int maxDepth, unsigned int numSamples = 100)
{
    std::vector<Photon> photons;

    for (const auto * light : lights)
    {
        for (int i = 0; i < numSamples; i++)
        {
            Photon photon (light->colour);
            // DLOG("photon power initial: %s", glm::to_string(photon.getPower()).c_str());

            std::optional<Photon> result =
                castPhoton(sceneManager, light->sampleRay().first, photon, ior,
                           1, maxDepth,
                           [](const MaterialAction & action,
                              const Material & material) -> bool
                           {
                               return !(action == MaterialAction::Reflect ||
                                        action == MaterialAction::Transmit &&
                                            material.isTransmissiveRefractive()||
                                        action == MaterialAction::Transmit &&
                                            material.isTransmissiveReflective()&&
                                            !material.isTransmissiveDiffuse());
                           });

            if (result)
            {
                // DLOG("photon colour: %s", glm::to_string(photon.getPower()).c_str());
                photons.emplace_back(result.value());
            }
            else
            {
                --i;
            }
        }
    }

    return KDTree<Photon, double>(
        photons, {PhotonCompare1(), PhotonCompare2(), PhotonCompare3()},
        {PhotonDistance1(), PhotonDistance2(), PhotonDistance3()});
}

static KDTree<Photon, double>
createGlobalPhotonMap(const SceneManager & sceneManager,
                    const std::vector<const Light *> & lights,
                    const glm::dvec3 & ambient, const double & ior,
                    unsigned int maxDepth, unsigned int numSamples = 100)
{
    std::vector<Photon> photons;

    for (const auto * light : lights)
    {
        for (int i = 0; i < numSamples; i++)
        {
            Photon photon (light->colour);

            if (auto result = castPhoton(sceneManager, light->sampleRay().first,
                                         photon, ior, 0, maxDepth))
            {
                photons.emplace_back(result.value());
            }
            else
            {
                --i;
            }
        }
    }

    DLOG("Num photons: %zd", photons.size());
    return KDTree<Photon, double>(
        photons, {PhotonCompare1(), PhotonCompare2(), PhotonCompare3()},
        {PhotonDistance1(), PhotonDistance2(), PhotonDistance3()});
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

glm::dvec3 calculatePixelValue(
    const SceneManager & sceneManager, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec4 & viewSpaceEye,
    const glm::dvec4 & viewSpacePixel, const glm::mat4 & viewMatrix,
    unsigned int numSamples, bool supersample = false, bool jitter = false)
{
    // get vector of subpixels
    std::vector<glm::dvec4> subPixels;
    if (supersample)
    {
        subPixels = generateSubScreenPositions(viewSpacePixel, 3, 3, jitter);
    }
    else
    {
        subPixels = generateSubScreenPositions(viewSpacePixel, 1, 1, jitter);
    }

    glm::dvec3 pixelColour = glm::dvec3(0.0, 0.0, 0.0);

    // iterate over subpixels, add to total
    for (auto subPixel : subPixels)
    {
        glm::dvec4 worldSpaceEye = glm::inverse(viewMatrix) * viewSpaceEye;
        glm::dvec4 worldSpacePixel = glm::inverse(viewMatrix) * subPixel;

        for (unsigned int i = 0; i < numSamples; i++)
        {
            pixelColour += intersectAndGetColour(
                sceneManager, Ray(worldSpaceEye, worldSpacePixel), lights,
                ambient, 1.0, 0, 2);
        }
    }

    // average pixel vals
    return pixelColour * 1.0 / ((double)subPixels.size() * (double)numSamples);
}

void renderDispatch(const SceneManager & sceneManager, Image & image,
                    const Camera & camera, const glm::vec3 & ambient,
                    const std::vector<const Light *> & lights,
                    const std::vector<glm::uvec2> & screenPixels,
                    ProgressBar & progressBar, unsigned int numSamples,
                    bool supersample = false, bool jitter = false)
{

    size_t h = image.height();
    size_t w = image.width();

    double zval = getScreenDepth(h, camera.m_fovy);
    glm::dvec4 viewSpacePixel{0.0, 0.0, -zval, 1.0};
    glm::dvec4 viewSpaceEye{0.0, 0.0, 0.0, 1.0};

    glm::mat4 viewMatrix =
        glm::lookAt(camera.m_eye, camera.m_view, camera.m_up);

    for (const glm::uvec2 pixel : screenPixels)
    {
        viewSpacePixel.y = getScreenPosition(h, pixel.y, true);
        viewSpacePixel.x = getScreenPosition(w, pixel.x);

        // set colour
        setPixelColour(image, pixel.x, pixel.y,
                       calculatePixelValue(sceneManager, ambient, lights,
                                           viewSpaceEye, viewSpacePixel,
                                           viewMatrix, numSamples, supersample, jitter));
        ++progressBar;
        progressBar.conditionalOut(std::cout);
    }
}

void renderDispatch(const SceneManager & sceneManager, Image & image,
                    const Camera & camera, const glm::vec3 & ambient,
                    const std::vector<const Light *> & lights,
                    unsigned int startY, unsigned int endY, unsigned int startX,
                    unsigned int endX, ProgressBar & progressBar,
                    unsigned int numSamples, bool supersample = false,
                    bool jitter = false)
{

    size_t h = image.height();
    size_t w = image.width();

    double zval = getScreenDepth(h, camera.m_fovy);
    glm::dvec4 viewSpacePixel{0.0, 0.0, -zval, 1.0};
    glm::dvec4 viewSpaceEye{0.0, 0.0, 0.0, 1.0};

    glm::mat4 viewMatrix =
        glm::lookAt(camera.m_eye, camera.m_view, camera.m_up);

    for (unsigned int y = startY; y < endY; ++y)
    {
        viewSpacePixel.y = getScreenPosition(h, y, true);

        for (unsigned int x = startX; x < endX; ++x)
        {
            viewSpacePixel.x = getScreenPosition(w, x);

            // set colour
            setPixelColour(image, x, y,
                           calculatePixelValue(sceneManager, ambient, lights,
                                               viewSpaceEye, viewSpacePixel,
                                               viewMatrix, numSamples, supersample, jitter));

            ++progressBar;
        }
    }
}

void render(const SceneManager & sceneManager, Image & image,
            const Camera & camera, const glm::vec3 & ambient,
            const std::vector<const Light *> & lights,
            std::vector<glm::uvec2> screenPixels, unsigned int numSamples,
            unsigned int numThreads, unsigned int pixelsPerChunk = 128,
            bool supersample = false, bool jitter = false)
{

    ProgressBar progressBar(image.height() * image.width());
    std::atomic<bool> b;
    numThreads =
        std::max(std::min(std::thread::hardware_concurrency(), numThreads), 1u);

    ThreadPool threadPool(numThreads);

    DLOG("num threads: %d", numThreads);

    std::cout << progressBar;

    // split up work to thread pool
    while (screenPixels.size() > 0) {
        std::vector<glm::uvec2> chunk;

        if (screenPixels.size() > pixelsPerChunk)
        {
            chunk = std::vector<glm::uvec2>(
                screenPixels.begin(), screenPixels.begin() + pixelsPerChunk);
            screenPixels.erase(screenPixels.begin(), screenPixels.begin() + pixelsPerChunk);
        }
        else
        {
            chunk = std::move(screenPixels);
            screenPixels.clear();
        }

        threadPool.queueJob(
            [&, chunk]()
            {
                renderDispatch(sceneManager, image, camera, ambient, lights,
                               chunk, progressBar, numSamples, supersample,
                               jitter);
            });
    }

    DLOG("thread start");
    threadPool.start();

    // while thread pool is busy, update the progress bar output
    while (threadPool.isBusy())
    {
        progressBar.conditionalOut(std::cout);
    }

    DLOG("thread stop");
    threadPool.stop();

    std::cout << progressBar;
}

void renderSingleThreaded(const SceneManager & sceneManager, Image & image,
            const Camera & camera, const glm::vec3 & ambient,
            const std::vector<const Light *> & lights, unsigned int numSamples,
            unsigned int numThreads)
{
        ProgressBar progressBar(image.height() * image.width());

        std::cout << progressBar;
        renderDispatch(sceneManager, image, camera, ambient, lights, 0,
                       image.height(), 0, image.height(), progressBar,
                       numSamples);
}

// ------------------- main ----------------------
void A5_Render(
    // What to render
    SceneNode *root, Image & image,
    // Viewing parameters
    const glm::vec3 & eye, const glm::vec3 & view, const glm::vec3 & up,
    double fovy,
    // Lighting parameters
    const glm::vec3 & ambient, const std::list<Light *> & lights,
    unsigned int numSamples, bool visualizePhotons,
    double photonRadiusVisualization, unsigned int numPhotons,
    unsigned int numThreads, bool adaptiveSupersampling)
{
    std::vector<const Light *> lightVector;

    for (const Light *light : lights)
    {
        lightVector.emplace_back(light);
    }

    printSceneInfo(image.width(), image.height(), *root, eye, view, up, fovy,
                   ambient, lights);

    SceneManager sceneManager;
    sceneManager.importSceneGraph(root);

    Camera camera(eye, view, up, fovy);

    auto kdTree = createCausticPhotonMap(sceneManager, lightVector, ambient, 1, 10, numPhotons);

    if (visualizePhotons)
    {
        SceneManager photonManager;
        photonManager.importSceneGraph(
            createPhotonScene(kdTree, photonRadiusVisualization));

        render(photonManager, image, camera, ambient, lightVector,
               getAllPixelCoordinates(image), numSamples, numThreads);
    }
    else
    {
        render(sceneManager, image, camera, ambient, lightVector,
               getAllPixelCoordinates(image), numSamples, numThreads);

        DLOG("rendered");

        if (adaptiveSupersampling)
        {
            Image varianceImage = apply(
                image, ImageKernel<1, glm::dvec3>(&varianceKernelFunc<1>));
            Image thresholdImage =
                apply(varianceImage,
                      ImageKernel<0, glm::dvec3>(&varianceThresholdKernelFunc));

            DLOG("supersample pass");

            std::vector<glm::uvec2> edgePixels =
                getWhitePixelCoordinates(thresholdImage);
            DLOG("num threshold pixels: %ld", edgePixels.size())
            ProgressBar progressBar(edgePixels.size());

            render(sceneManager, image, camera, ambient, lightVector,
                   edgePixels, numSamples, numThreads, 128, true, true);

            varianceImage.savePng("varianceImage.png");
            thresholdImage.savePng("thresholdImage.png");
        }
    }
}
