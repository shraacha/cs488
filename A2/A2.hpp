// Termm--Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

#include "constants.hpp"

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

    void drawLines (const std::vector<line4> & lineList);
    void drawGnomon(const std::vector<line4> &lineList, const std::vector<colour> & colours);

    ShaderProgram m_shader;

    GLuint m_vao;            // Vertex Array Object
    GLuint m_vbo_positions;  // Vertex Buffer Object
    GLuint m_vbo_colours;    // Vertex Buffer Object

    VertexData m_vertexData;

    glm::vec3 m_currentLineColour;

    // model lines
    std::vector<line4> m_modelCubeLines;
    std::vector<line4> m_modelGnomonLines;
    // world
    std::vector<line4> m_worldGnomonLines;
    // viewport
    std::vector<line4> m_viewportLines;

    // transformations
    glm::mat4 m_modelRotAndTsl;
    glm::mat4 m_modelScl;

    glm::mat4 m_viewRotAndTsl;

    glm::mat4 m_perspective;
};
