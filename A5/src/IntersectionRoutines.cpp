#include "IntersectionRoutines.hpp"

std::optional<std::pair<Intersection, Material *>>
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
