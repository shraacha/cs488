// Termm--Fall 2024

#include "A2.hpp"
#include "constants.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//--------- helpers ----------
static inline line4 transformLine(const glm::mat4 & transformation, const line4 & line) {
    return {transformation * line.first, transformation * line.second};
}

static inline std::vector<line4>
transformLines(const glm::mat4 &transformation,
               const std::vector<line4> &lines) {
    std::vector<line4> newLines;
    for(auto it = lines.begin(); it < lines.end(); it++) {
        // TODO use cpp17 for emplace back
        newLines.push_back(transformLine(transformation, *it));
    }
    return newLines;
}

// Params:
//  - translatioVector[0] - translation in the x dir, relative to to current frame
//  - translatioVector[1] - ... y dir ...
//  - translatioVector[2] - ... z dir ...
static inline glm::mat4 getTranslationMatrix(const glm::vec3 & inputVec) {
    glm::mat4 translationMatrix = glm::mat4();
    for(int i = 0; i < 3; i ++) {
        // glm::mat4 is column major - i.e. columns are indexed first
        translationMatrix[3][i] = inputVec[i];
    }

    return translationMatrix;
}

// Description:
//  - creates a rotation matrix relative to the current frame. When right
//  multiplied by a matrix, the z-axis rotation is applied first, then y, then x.
// Params:
//  - inputVec[0] - radians to rotate counterclockwise around the x-axis, relative to to
//  current frame
//  - inputVec[1] - ... y-axis ...
//  - inputVec[2] - ... z-axis ...
static inline glm::mat4 getRotationMatrix(const glm::vec3 &inputVec) {
    // glm::mat4 is column major - i.e. columns are indexed first
    glm::mat4 xAxisRotation = glm::mat4();
    glm::mat4 yAxisRotation = glm::mat4();
    glm::mat4 zAxisRotation = glm::mat4();

    // x
    /*
     *     1      0       0      0
     *     0    cos(t) -sin(t)   0
     *     0    sin(t)  cos(t)   0
     *     0      0       0      1
     */
    xAxisRotation[1][1] = cosf(inputVec[0]);
    xAxisRotation[1][2] = sinf(inputVec[0]);
    xAxisRotation[2][1] = -sinf(inputVec[0]);
    xAxisRotation[2][2] = cosf(inputVec[0]);

    // y
    /*
     * with z up, x to the right, y faces forward, not towards us.
     * This means that we need to flip t when compared to a rotation wrt x & y.
     * sin is an odd f'n, cos is an even f'n
     *
     *   cos(t)   0   sin(t)  0
     *     0      1     0     0
     *  -sin(t)   0   cos(t)  0
     *     0      0     0     1
     */
    yAxisRotation[0][0] = cosf(inputVec[1]);
    yAxisRotation[0][2] = -sinf(inputVec[1]);
    yAxisRotation[2][0] = sinf(inputVec[1]);
    yAxisRotation[2][2] = cosf(inputVec[1]);

    // z
    /*
     * cos(t)   -sin(t)   0   0
     * sin(t)   cos(t)    0   0
     *   0         0      1   0
     *   0         0      0   1
     */
    zAxisRotation[0][0] = cosf(inputVec[2]);
    zAxisRotation[0][1] = sinf(inputVec[2]);
    zAxisRotation[1][0] = -sinf(inputVec[2]);
    zAxisRotation[1][1] = cosf(inputVec[2]);

    return xAxisRotation * yAxisRotation * zAxisRotation;
}

//  Description:
//  - creates a scaling matrix relative to the current frame.
// Params:
//  - inputVec[0] - factor to scale in the x-axis
//  - inputVec[1] - ... y-axis ...
//  - inputVec[2] - ... z-axis ...
static inline glm::mat4 getScaleMatrix(const glm::vec3 &inputVec) {
    glm::mat4 scaleMatrix = glm::mat4();

    // glm::mat4 is column major - i.e. columns are indexed first
    scaleMatrix[0][0] = inputVec[0];
    scaleMatrix[1][1] = inputVec[1];
    scaleMatrix[2][2] = inputVec[2];

    return scaleMatrix;
}

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
    : numVertices(0),
      index(0)
{
    positions.resize(kMaxVertices);
    colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
    : m_currentLineColour(vec3(0.0f)),
      m_modelCubeLines{c_cubeLines},
      m_modelGnomonLines{c_unitGnomonLines},
      m_worldGnomonLines{c_unitGnomonLines}
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
    // Set the background colour.
    glClearColor(0.2, 0.5, 0.3, 1.0);

    createShaderProgram();

    glGenVertexArrays(1, &m_vao);

    enableVertexAttribIndices();

    generateVertexBuffers();

    mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
    m_shader.generateProgramObject();
    m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
    m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
    m_shader.link();
}

//---------------------------------------------------------------------------------------- Fall 2022
void A2::enableVertexAttribIndices()
{
    glBindVertexArray(m_vao);

    // Enable the attribute index location for "position" when rendering.
    GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray(positionAttribLocation);

    // Enable the attribute index location for "colour" when rendering.
    GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
    glEnableVertexAttribArray(colourAttribLocation);

    // Restore defaults
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
    // Generate a vertex buffer to store line vertex positions
    {
        glGenBuffers(1, &m_vbo_positions);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

        // Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
                GL_DYNAMIC_DRAW);


        // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }

    // Generate a vertex buffer to store line colors
    {
        glGenBuffers(1, &m_vbo_colours);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

        // Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
                GL_DYNAMIC_DRAW);


        // Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
    // Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao);

    // Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
    // "position" vertex attribute index for any bound shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
    GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
    glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
    // "colour" vertex attribute index for any bound shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
    GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
    glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
    m_vertexData.numVertices = 0;
    m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
        const glm::vec3 & colour
) {
    m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
        const glm::vec2 & V0,   // Line Start (NDC coordinate)
        const glm::vec2 & V1    // Line End (NDC coordinate)
) {

    m_vertexData.positions[m_vertexData.index] = V0;
    m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
    ++m_vertexData.index;
    m_vertexData.positions[m_vertexData.index] = V1;
    m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
    ++m_vertexData.index;

    m_vertexData.numVertices += 2;
}

//---------------------------------------------------------------------------------------
void A2::drawLines(const std::vector<line4> &lineList) {
    for (auto it = lineList.begin(); it != lineList.end(); it++) {
        drawLine(glm::vec2(it->first), glm::vec2(it->second));
    }
}

//---------------------------------------------------------------------------------------
void A2::drawGnomon(const std::vector<line4> &lineList, const std::vector<colour> & colours) {
    auto itColour = colours.begin();
    for(auto it = lineList.begin(); it != lineList.end(); it++, itColour++) {
        setLineColour(*itColour);
        drawLine(glm::vec2(it->first), glm::vec2(it->second));
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
    // Place per frame, application logic here ...

    // Call at the beginning of frame, before drawing lines:
    initLineData();

    // transform lines
    std::vector<line4> transformedModelCubeLines =
        transformLines(m_perspective * m_viewRotAndTrn * m_modelScl * m_modelRotAndTrn, m_modelCubeLines);
    std::vector<line4> transformedModelGnomonLines =
        transformLines(m_perspective * m_viewRotAndTrn * m_modelRotAndTrn, m_modelGnomonLines);
    std::vector<line4> transformedWorldGnomonLines =
        transformLines(m_perspective * m_viewRotAndTrn * m_modelRotAndTrn, m_modelGnomonLines);

    // clip to near and far planes
    // (-1.0, 1.0, if mapping z)

    // homogenize

    // clip to window

    // viewport transformation

    // draw lines
    setLineColour(c_white);
    drawLines(transformedModelCubeLines);
    drawGnomon(transformedModelGnomonLines, {c_red, c_green, c_blue});
    //drawGnomon(m_worldGnomonLines, {c_cyan, c_magenta, c_yellow});
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
    static bool firstRun(true);
    if (firstRun) {
        ImGui::SetNextWindowPos(ImVec2(50, 50));
        firstRun = false;
    }

    static bool showDebugWindow(true);
    ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
    float opacity(0.5f);

    ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
            windowFlags);


        // Add more gui elements here here ...


        // Create Button, and check if it was clicked:
        if( ImGui::Button( "Quit Application" ) ) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }

        ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

    //-- Copy vertex position data into VBO, m_vbo_positions:
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
                m_vertexData.positions.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }

    //-- Copy vertex colour data into VBO, m_vbo_colours:
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
                m_vertexData.colours.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        CHECK_GL_ERRORS;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
    uploadVertexDataToVbos();

    glBindVertexArray(m_vao);

    m_shader.enable();
        glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
    m_shader.disable();

    // Restore defaults
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
        int entered
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
        double xPos,
        double yPos
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
        int button,
        int actions,
        int mods
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
        double xOffSet,
        double yOffSet
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
        int width,
        int height
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
        int key,
        int action,
        int mods
) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}
