#pragma once

#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <stack>
#include <utility>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "JointNode.hpp"

const std::string c_head_joint_name = "head_joint";

// This struct stores data that should be inherited from the parent node in some form
struct InheritedNodeData {
    glm::mat4 trans;
    std::optional<NodeID> nodeId;
};

bool operator==(const InheritedNodeData & a, const InheritedNodeData & b);
bool operator!=(const InheritedNodeData & a, const InheritedNodeData & b);

InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode);

// controls how data is inherited
InheritedNodeData makeInheritableNodeData(const SceneNode & thisNode,
                                       const InheritedNodeData &inheritedData);

using NodeAndInheritedData = std::pair<SceneNode *, InheritedNodeData>;

// container for scene nodes
class Scene {
public:
    Scene();

    bool importSceneGraph(SceneNode* root);

    bool isEmpty();

    // in radians
    void rotate(const glm::vec3 & axis, const double & theta);
    void translate(const glm::vec3 & translation);

    void resetRotation();
    void resetTranslation();

    // ids
    bool isValidId(const NodeID & id);
    bool isHeadId(const NodeID & id);
    JointNode* getJoint(const NodeID & id);
    void resetAllJoints();

    // iterator stuff
    struct PreOrderTraversalIterator {
        using iterator_category = std::forward_iterator_tag;
        using pointer_t = SceneNode *;
        using const_pointer_t = const SceneNode *;
        using reference_t = SceneNode &;
        using const_reference_t = const SceneNode &;

        // ctors
        PreOrderTraversalIterator(); // for end()
        PreOrderTraversalIterator(pointer_t ptr);

        // operations
        const_reference_t operator*() const;
        const_pointer_t operator->() const;

        // prefix
        PreOrderTraversalIterator& operator++();
        // postfix
        PreOrderTraversalIterator operator++(int);

        friend bool operator==(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);
        friend bool operator!=(const PreOrderTraversalIterator& a, const PreOrderTraversalIterator& b);

        // other functions
        glm::mat4 getInheritedTransformation();
        std::optional<NodeID> getInheritedJointID();

    private:
        std::stack<NodeAndInheritedData> m_nodeStack;

        void addNodesToStack(std::list<SceneNode*> & nodes, const InheritedNodeData & inheritedData);
    };

    PreOrderTraversalIterator begin() {return PreOrderTraversalIterator(m_sceneRoot.get());};
    PreOrderTraversalIterator end() {return PreOrderTraversalIterator();};

private:
    std::unique_ptr<SceneNode> m_sceneRoot; // owns the tree
    SceneNode* m_globalTranslationNode;
    SceneNode* m_globalRotationNode;

    std::map<NodeID, SceneNode *> m_jointIDToNodeMap;
    std::pair<NodeID, SceneNode *> m_headIDtoNode;
};

class SceneCommand {
  public:
    virtual ~SceneCommand() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class MoveJointsCommand : public SceneCommand {
  public:
    MoveJointsCommand(Scene * scene, const std::vector<NodeID> & jointIds,
                      const double & degreesX, const double & degreesY);
    virtual void execute() override;
    virtual void undo() override;

  private:
    Scene * scene;
    std::vector<std::tuple<JointNode*, double, double>> jointAndRotation;
};

class RotateCommand : public SceneCommand {
  public:
    RotateCommand(Scene *scene, const glm::vec3 & axis, const double & degrees);
    virtual void execute() override;
    virtual void undo() override;

  private:
    Scene *scene;
    glm::vec3 axis;
    double degrees;
};

class TranslateCommand : public SceneCommand {
  public:
    TranslateCommand(Scene *scene, const glm::vec3 & translation);
    virtual void execute() override;
    virtual void undo() override;

  private:
    Scene *scene;
    glm::vec3 translation;
};

class SceneCommandList {
  public:
    void addCommand(std::unique_ptr<SceneCommand> command);
    void undoCommand();
    void redoCommand();
    void undoAll();
    inline void clearAll();
    void undoAndClearAll();

    size_t getLength();
    size_t getIndex();

  private:
    std::vector<std::unique_ptr<SceneCommand>> commandList;
    size_t indexPlusOne;
};
