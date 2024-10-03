// Termm--Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <functional>
#include <glm/glm.hpp>

#include <vector>
#include <optional>

#include "constants.hpp"
#include "customTypes.hpp"
#include "viewport.hpp"

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
    VertexData();

    std::vector<glm::vec2> positions;
    std::vector<glm::vec3> colours;
    GLuint index;
    GLsizei numVertices;
};


class A2 : public CS488Window {
public:
    A2();
    virtual ~A2();

protected:
    virtual void init() override;
    virtual void appLogic() override;
    virtual void guiLogic() override;
    virtual void draw() override;
    virtual void cleanup() override;

    virtual bool cursorEnterWindowEvent(int entered) override;
    virtual bool mouseMoveEvent(double xPos, double yPos) override;
    virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
    virtual bool windowResizeEvent(int width, int height) override;
    virtual bool keyInputEvent(int key, int action, int mods) override;

    void createShaderProgram();
    void enableVertexAttribIndices();
    void generateVertexBuffers();
    void mapVboDataToVertexAttributeLocation();
    void uploadVertexDataToVbos();

    void initLineData();

    void setLineColour(const glm::vec3 & colour);

    void drawLine (
            const glm::vec2 & v0,
            const glm::vec2 & v1
    );

    ShaderProgram m_shader;

    GLuint m_vao;            // Vertex Array Object
    GLuint m_vbo_positions;  // Vertex Buffer Object
    GLuint m_vbo_colours;    // Vertex Buffer Object

    VertexData m_vertexData;

    glm::vec3 m_currentLineColour;

    // additions
    void reset();

    void resetViewportData();
    void updateViewportData();

    inline void resetMouseButtons();

    void drawLines(const std::vector<line4> &lineList);
    void drawLines(const std::vector<std::optional<line4>> &lineList,
                   const colour &colour);
    void drawLines(const std::vector<std::optional<line4>> &lineList,
                   const std::vector<colour> &colours);

    static glm::vec4 zClipPlaneDistToPoint(const float & dist);
    static glm::vec4 getNearPlaneNormal();
    static glm::vec4 getFarPlaneNormal();

    void testClip();

    // model lines
    std::vector<std::optional<line4>> m_modelCubeLines;
    std::vector<std::optional<line4>> m_modelGnomonLines;
    // world
    std::vector<std::optional<line4>> m_worldGnomonLines;

    // transformations
    glm::vec3 m_modelScale;
    glm::mat4 m_modelScaleMatrix; // not updated directly
    glm::mat4 m_modelToWorld;

    glm::mat4 m_worldToView;

    float m_fov;
    glm::mat4 m_perspective;

    glm::mat4 m_deviceToNDC;
    glm::mat4 m_NDCToNDCSub;

    // clipping
    float m_nearDist;
    float m_farDist;

    // viewports
    float m_deviceWidth;
    float m_deviceHeight;

    const Viewport m_NDCViewport;
    Viewport m_NDCSubViewport;
    line4 m_deviceViewportCorners;

    std::vector<std::optional<line4>> m_NDCSubViewportLines;
    std::vector<pointAndNormal> m_NDCSubViewportWalls;

    // mouse things
    bool m_startInput;
    InteractionMode m_interactionMode;
    bool m_LMB;
    bool m_RMB;
    bool m_MMB;
};
