// Termm--Fall 2024

#include "A2.hpp"
#include "constants.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <algorithm>
#include <iterator>
#include <optional>
#include <tuple>
#include <functional>

using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;


//--------- helpers ----------
static inline line4 transformLine(const line4 &line,
                                  const glm::mat4 &transformation) {
    return {transformation * line.first, transformation * line.second};
}

static inline std::vector<std::optional<line4>>
transformLines(const std::vector<std::optional<line4>> &lines,
               const glm::mat4 &transformation) {
    std::vector<std::optional<line4>> transformedLines;

    std::transform(lines.begin(), lines.end(),
                   std::back_inserter(transformedLines),
                   [&](const std::optional<line4> &line) {
                       return transformLine(*line, transformation);
                   });

    return transformedLines;
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

static inline glm::mat4 getPerspectiveMatrix(const float & fov) {
    glm::mat4 perspectiveMatrix = glm::mat4();

    // scale x and y based on the FOV. This controls what gets mapped to 1.
    perspectiveMatrix[0][0] = cosf(fov / 2);
    perspectiveMatrix[1][1] = cosf(fov / 2);

    // currently not mapping z
    // TODO add z mapping

    // replace w with z
    perspectiveMatrix[2][3] = -1; // camera is looking down -z, so we flip it

    return perspectiveMatrix;
}

//  Description
//  - returns (Q - P) * n, where P is a point on the line,
//  n is normal to the line, and Q is the point being tested.
static inline float implicitLineEquation(const glm::vec4 & Q, const glm::vec4 & P, const glm::vec4 & n) {
    return glm::dot((Q - P), n);
}

static inline glm::vec4 parametricLineEquation(const glm::vec4 & A, const glm::vec4 & B, const float & t) {
    return (1.0f - t) * A + t * B;
}

// the line must not be a point
// TODO clean up this spagetti code
static inline std::optional<line4> clip(const line4 & line, const glm::vec4 & P, const glm::vec4 & n) {
    float testA = implicitLineEquation(line.first, P, n);
    float testB = implicitLineEquation(line.second, P, n);

    if (testA < 0 && testB < 0) {
        return std::nullopt;
    } else if (testA >= 0 && testB >= 0) {
        return line;
    } else if (testA - testB == 0) {
        return std::nullopt; // to prevent a divide by 0 in the next step
    } else {
        glm::vec4 intersectionPoint = parametricLineEquation(
            line.first, line.second, testA / (testA - testB));

        if (testA >= 0) {
            return std::make_pair(line.first, intersectionPoint);
        } else {
            return std::make_pair(line.second, intersectionPoint);
        }
    }
}

static inline std::optional<line4>
clip(std::optional<line4> && line,
         const std::vector<pointAndNormal> &walls)
{
    for (const auto &wall : walls) {
        line = clip(*line, wall.first, wall.second);
        if(!line.has_value()) return line;
    }
    return line;
}

static inline std::vector<std::optional<line4>>
clip(std::vector<std::optional<line4>> &&lines,
          const std::vector<pointAndNormal> &walls)
{
    std::for_each(lines.begin(),
                  lines.end(),
                  [&](std::optional<line4> & line) -> void
                  {
                      line = clip(std::move(line), walls);
                  });

    return lines;
}

static inline std::vector<std::optional<line4>>
convertLinesToOptionalLines(const std::vector<line4> &inputLines) {
    std::vector<std::optional<line4>> optionalLines;

    std::transform(inputLines.begin(), inputLines.end(),
                  std::back_inserter(optionalLines),
                  [](const line4 & line){return line;});

    return optionalLines;
}


// We assume that we /can/ homogenize, i.e. no points with w = 0
static inline glm::vec4
homogenize(const glm::vec4 & point) {
    return point * 1.0f/point[3];
}

static inline std::vector<std::optional<line4>>
homogenize(std::vector<std::optional<line4>> &&lines)
{
    std::for_each(lines.begin(), lines.end(),
                  [](std::optional<line4> &line) -> void {
                      // We assume that we can homogenize, no points with w = 0
                      if (line.has_value()) {
                          line->first = homogenize(line->first);
                          line->second = homogenize(line->second);
                      }
                  });
    return lines;
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
      m_modelCubeLines{},
      m_modelGnomonLines{{c_unitLineX, c_unitLineY, c_unitLineZ}},
      m_worldGnomonLines{{c_unitLineX, c_unitLineY, c_unitLineZ}},
      m_viewRotAndTsl{glm::inverse(glm::make_mat4(c_defaultCameraToWorldMatrix))},
      m_perspective{getPerspectiveMatrix(c_defaultFOV)},
      m_nearDist{c_defaultNearDistance},
      m_farDist{c_defaultFarDistance}
{
    m_modelCubeLines = convertLinesToOptionalLines(c_cubeLines);
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
    for (const auto & line : lineList) {
        drawLine(glm::vec2(line.first), glm::vec2(line.second));
    }
}

void A2::drawLines(const std::vector<std::optional<line4>> &lineList, const colour & colour) {
    setLineColour(colour);
    for (const auto & line : lineList) {
        if (line.has_value()) {
            drawLine(glm::vec2(line->first), glm::vec2(line->second));
        }
    }
}

void A2::drawLines(const std::vector<std::optional<line4>> &lineList,
                   const std::vector<colour> &colours) {
    auto itColour = colours.begin();
    for (auto it = lineList.begin();
         it != lineList.end() && itColour != colours.end(); it++, itColour++) {
        if (it->has_value()) {
            setLineColour(*itColour);
            drawLine(glm::vec2((*it)->first), glm::vec2((*it)->second));
        }
    }
}


//----------------------------------------------------------------------------------------
glm::vec4 A2::zClipPlaneDistToPoint(const float & dist) {
    return -dist * c_unitZ;
}

glm::vec4 A2::getNearPlaneNormal() {
    return -c_standardBasisZ;
}

glm::vec4 A2::getFarPlaneNormal() {
    return c_standardBasisZ;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
    // data
    std::vector<std::optional<line4>> transformedModelCubeLines;
    std::vector<std::optional<line4>> transformedModelGnomonLines;
    std::vector<std::optional<line4>> transformedWorldGnomonLines;

    // Call at the beginning of frame, before drawing lines:
    initLineData();

    // transform lines by V * M
    transformedModelCubeLines = transformLines(m_modelCubeLines, m_perspective * m_viewRotAndTsl * m_modelScl * m_modelRotAndTsl);
    transformedModelGnomonLines = transformLines(m_modelGnomonLines, m_perspective * m_viewRotAndTsl * m_modelRotAndTsl);
    transformedWorldGnomonLines = transformLines(m_worldGnomonLines, m_perspective * m_viewRotAndTsl);

    // clip to near and far planes
    std::vector<pointAndNormal> nearAndFarPlane = {
        {zClipPlaneDistToPoint(m_nearDist), getNearPlaneNormal()},
        {zClipPlaneDistToPoint(m_farDist), getFarPlaneNormal()},
    };

    transformedModelCubeLines = clip(std::move(transformedModelCubeLines), nearAndFarPlane);
    transformedModelGnomonLines = clip(std::move(transformedModelGnomonLines), nearAndFarPlane);
    transformedWorldGnomonLines = clip(std::move(transformedWorldGnomonLines), nearAndFarPlane);

    // homogenize
    transformedModelCubeLines = homogenize(std::move(transformedModelCubeLines));
    transformedModelGnomonLines = homogenize(std::move(transformedModelGnomonLines));
    transformedWorldGnomonLines = homogenize(std::move(transformedWorldGnomonLines));

    // clip to window

    // auto clipNDC = [&](std::optional<line4> &line) {
    //     line = clipLine(*line, {1.0f, 0.0f, 0.0f, 0.0f}, -c_standardBasisX);

    //     if (line.has_value()) {
    //         line = clipLine(*line, {-1.0f, 0.0f, 0.0f, 0.0f}, c_standardBasisX);
    //         if (line.has_value()) {
    //             line = clipLine(*line, {0.0f, 1.0f, 0.0f, 0.0f},
    //                             -c_standardBasisY);
    //             if (line.has_value()) {
    //                 line = clipLine(*line, {0.0f, -1.0f, 0.0f, 0.0f},
    //                                 c_standardBasisY);
    //             }
    //         }
    //     }
    // };

    // viewport transformation

    // draw lines
    drawLines(transformedModelCubeLines, c_white);
    drawLines(transformedModelGnomonLines, std::vector<colour>{c_red, c_green, c_blue});
    drawLines(transformedWorldGnomonLines, std::vector<colour>{c_cyan, c_magenta, c_yellow});
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
