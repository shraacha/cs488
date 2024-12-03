#include "PhotonMapHelpers.hpp"
#include "CookTorranceMaterial.hpp"
#include "PhongMaterial.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Primitive.hpp"

SceneNode * createPhotonScene(const KDTree<Photon, double> & kdTree, double radius)
{
    SceneNode * root = new SceneNode("photon");

    for (const Photon & photon : kdTree.getKeys())
    {
        GeometryNode *sphere = new GeometryNode(
            "wow", new NonhierSphere(glm::vec3(photon.getPosition()), radius));
        // TODO fix memory leak
        sphere->setMaterial(new PhongMaterial(
            glm::vec3(photon.getPower()), glm::vec3(0.5), 0.3));

        root->add_child(sphere);
    }

    return root;
}
