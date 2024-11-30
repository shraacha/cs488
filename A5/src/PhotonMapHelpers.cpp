#include "PhotonMapHelpers.hpp"
#include "CookTorranceMaterial.hpp"
#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Primitive.hpp"

SceneNode * createPhotonScene(const KDTree<Photon, double> & kdTree)
{
    SceneNode * root = new SceneNode("photon");

    for (const Photon & photon : kdTree.getKeys())
    {
        GeometryNode *sphere = new GeometryNode(
            "wow", new NonhierSphere(glm::vec3(photon.getPosition()), 0.3));
        // TODO fix memory leak
        sphere->setMaterial(new CookTorranceMaterial(
            glm::vec3(photon.getPower()), glm::vec3(1.0), 0.9));

        root->add_child(sphere);
    }

    return root;
}
