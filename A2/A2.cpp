// Termm--Fall 2024

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "customTypes.hpp"
#include "viewport.hpp"

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
#include <glm/ext.hpp>
using namespace glm;

#include "constants.hpp"
#include "matrixHelpers.hpp"
#include "viewport.hpp"


//--------- helpers ----------

void printOptionalLine(const std::optional<line4> & line)
{
    if (line.has_value())
    {
        std::cout << line->first << " " << line->second << std::endl;
    } else {

        std::cout << "nullopt" << std::endl;
    }
}

static inline float clampValue(const float & value, const float & upper, const float & lower)
{
    return (value <= upper) ? ((value >= lower) ? value : lower) : upper;
}

static inline glm::vec3 clampValue(const glm::vec3 &value, const float &upper,
                                   const float &lower) {
    return {clampValue(value[0], upper, lower),
            clampValue(value[1], upper, lower),
            clampValue(value[2], upper, lower)};
}

static inline glm::vec4 deviceCoordToPoint(const float &x, const float &y,
                                           const float &clampX,
                                           const float &clampY) {
    glm::vec4 point = {0.0f, 0.0f, 0.0f, 1.0f};

    point[0] = (x <= clampX) ? ((x > 0.0f) ? x : 0.0f) : clampX;
    point[1] = (y <= clampY) ? ((y > 0.0f) ? y : 0.0f) : clampY;

    return point;
}

static inline line4 transformLine(const line4 &line,
                                  const glm::mat4 &transformation)
{
    return {transformation * line.first, transformation * line.second};
}

static std::vector<std::optional<line4>>
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

static std::vector<std::optional<line4>>
transformLines(std::vector<std::optional<line4>> &&lines,
               const glm::mat4 &transformation) {
    std::for_each(lines.begin(), lines.end(),
                  [&](std::optional<line4> &line) -> void {
                      if (line.has_value()) {
                          line = transformLine(*line, transformation);
                      }
                  });

    return lines;
}

//  Description
//  - returns (Q - P) * n, where P is a point on the line,
//  n is normal to the line, and Q is the point being tested.
static inline float implicitLineEquation(const glm::vec4 & Q, const glm::vec4 & P, const glm::vec4 & n) {
    return glm::dot(Q - P, n);
}

static inline glm::vec4 parametricLineEquation(const glm::vec4 & A, const glm::vec4 & B, const float & t) {
    return (1.0f - t) * A + t * B;
}

// the line must not be a point
// TODO clean up this spaghetti code
static std::optional<line4> clip(const line4 & line, const glm::vec4 & P, const glm::vec4 & n) {
    float testA = implicitLineEquation(line.first, P, n);
    float testB = implicitLineEquation(line.second, P, n);

    if (testA <= 0.0f && testB <= 0.0f) {
        return std::nullopt;
    } else if (testA > 0.0f && testB > 0.0f) {
        return line;
    } else if (testA - testB == 0.0f) {
        return std::nullopt; // to prevent a divide by 0 in the next step
    } else {
        glm::vec4 intersectionPoint = parametricLineEquation(
            line.first, line.second, testA / (testA - testB));

        if (testA >= 0) {
            return std::make_pair(line.first, intersectionPoint);
        } else {
            return std::make_pair(intersectionPoint, line.second);
        }
    }
}

static std::optional<line4>
clip(std::optional<line4> && line,
         const std::vector<pointAndNormal> &walls)
{
    for (const auto &wall : walls) {
        if(!line.has_value()) return line;
        line = clip(*line, wall.first, wall.second);
    }
    return line;
}

static std::vector<std::optional<line4>>
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

static std::vector<std::optional<line4>>
convertToOptionalLines(const std::vector<line4> &inputLines) {
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

static std::vector<std::optional<line4>>
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
      m_NDCViewport{getNDCViewport()}
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

    // A2 impl things

    // initializing data
    reset();
}

void A2::reset()
{
    m_modelCubeLines = convertToOptionalLines(c_cubeLines);
    m_modelGnomonLines = {c_unitLineX, c_unitLineY, c_unitLineZ};
    m_worldGnomonLines = {c_unitLineX, c_unitLineY, c_unitLineZ};

    m_modelScale = c_defaultModelScale;
    m_modelToWorld = c_defaultModelToWorldMatrix;

    m_worldToView = glm::inverse(glm::make_mat4(c_simpleCameraToWorldMatrix));

    m_fov = c_defaultFOV;

    m_nearDist = c_defaultNearDistance;
    m_farDist = c_defaultFarDistance;

    resetViewportData();

    m_interactionMode = c_defaultInteractionMode;
    resetMouseButtons();
}

void A2::resetViewportData()
{
    m_deviceViewportCorners = makeDeviceViewportCornersFromPortion(
        m_deviceWidth, m_deviceHeight, c_defaultViewPortion);
    updateViewportData();
}

void A2::updateViewportData() {
    m_NDCSubViewport = makeNDCViewportFromCorners(
        transformLine(m_deviceViewportCorners, m_deviceToNDC));
    m_NDCSubViewportLines =
        convertToOptionalLines(getViewportExplicitEdges(m_NDCSubViewport));
    m_NDCSubViewportWalls = getViewportImplicitEdges(m_NDCSubViewport);
    m_NDCToNDCSub = makeViewportTransformationMatrix(m_NDCViewport, m_NDCSubViewport);
}

inline void A2::resetMouseButtons()
{
    m_LMB = m_MMB = m_RMB = false;
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
    // convention is that -z is in front
    return {0.0f, 0.0f, -dist, 1.0f};
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
    m_perspective = makePerspectiveMatrix(m_fov);
    m_modelScaleMatrix = makeScaleMatrix(m_modelScale);

    std::vector<std::optional<line4>> transformedModelCubeLines;
    std::vector<std::optional<line4>> transformedModelGnomonLines;
    std::vector<std::optional<line4>> transformedWorldGnomonLines;

    std::vector<pointAndNormal> nearAndFarPlane = {
        {zClipPlaneDistToPoint(m_nearDist), getNearPlaneNormal()},
        {zClipPlaneDistToPoint(m_farDist), getFarPlaneNormal()},
    };

    // Call at the beginning of frame, before drawing lines:
    initLineData();

    // transform lines by V * M
    transformedModelCubeLines = transformLines(m_modelCubeLines, m_worldToView * m_modelToWorld * m_modelScaleMatrix);
    transformedModelGnomonLines = transformLines(m_modelGnomonLines, m_worldToView * m_modelToWorld);
    transformedWorldGnomonLines = transformLines(m_worldGnomonLines, m_worldToView);

    // clip to near and far planes
    transformedModelCubeLines = clip(std::move(transformedModelCubeLines), nearAndFarPlane);
    transformedModelGnomonLines = clip(std::move(transformedModelGnomonLines), nearAndFarPlane);
    transformedWorldGnomonLines = clip(std::move(transformedWorldGnomonLines), nearAndFarPlane);

    // perspective correction
    transformedModelCubeLines = transformLines(std::move(transformedModelCubeLines), m_perspective);
    transformedModelGnomonLines = transformLines(std::move(transformedModelGnomonLines), m_perspective);
    transformedWorldGnomonLines = transformLines(std::move(transformedWorldGnomonLines), m_perspective);

    // homogenize
    transformedModelCubeLines = homogenize(std::move(transformedModelCubeLines));
    transformedModelGnomonLines = homogenize(std::move(transformedModelGnomonLines));
    transformedWorldGnomonLines = homogenize(std::move(transformedWorldGnomonLines));

    // viewport transformation
    transformedModelCubeLines = transformLines(std::move(transformedModelCubeLines), m_NDCToNDCSub);
    transformedModelGnomonLines = transformLines(std::move(transformedModelGnomonLines), m_NDCToNDCSub);
    transformedWorldGnomonLines = transformLines(std::move(transformedWorldGnomonLines), m_NDCToNDCSub);

    // clip to window
    transformedModelCubeLines = clip(std::move(transformedModelCubeLines), m_NDCSubViewportWalls);
    transformedModelGnomonLines = clip(std::move(transformedModelGnomonLines), m_NDCSubViewportWalls);
    transformedWorldGnomonLines = clip(std::move(transformedWorldGnomonLines), m_NDCSubViewportWalls);

    // draw lines
    drawLines(transformedModelCubeLines, c_white);
    drawLines(transformedModelGnomonLines, std::vector<colour>{c_red, c_green, c_blue});
    drawLines(transformedWorldGnomonLines, std::vector<colour>{c_cyan, c_magenta, c_yellow});
    drawLines(m_NDCSubViewportLines, c_black);
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

        // the important buttons

        if( ImGui::Button( "Quit Application (Q)" ) ) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }
        ImGui::SameLine();
        if( ImGui::Button( "Reset Application (A)" ) ) {
            reset();
        }

        ImGui::Separator();
        // ~~~~~~~~~~~~~~~~~~~

        ImGui::PushID( 0 );
        ImGui::RadioButton( "Rotate View (O)", (int *)&m_interactionMode, 0 );
        ImGui::RadioButton( "Translate View (E)", (int *)&m_interactionMode, 1 );
        ImGui::RadioButton( "Perspective (P)", (int *)&m_interactionMode, 2 );
        ImGui::RadioButton( "Rotate Model (R)", (int *)&m_interactionMode, 3 );
        ImGui::RadioButton( "Translate Model (T)", (int *)&m_interactionMode, 4 );
        ImGui::RadioButton( "Scale Model (S)", (int *)&m_interactionMode, 5 );
        ImGui::RadioButton( "Viewport (V)", (int *)&m_interactionMode, 6 );
        ImGui::PopID();

        ImGui::Separator();
        // ~~~~~~~~~~~~~~~~~~~

        ImGui::Text( "FOV (rad): %.1f", m_fov );
        ImGui::Text( "Near distance: %.1f, Far distance: %.1f", m_nearDist, m_farDist );
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

    static double prevXPos;
    static double prevYPos;

    if(m_startInput) {
        prevXPos = xPos;
        prevYPos = yPos;
        m_startInput = false;
    }

    double dX = xPos - prevXPos;
    float theta = (dX * c_fullWindowRotation) / m_deviceWidth;
    float delta = (dX * m_NDCViewport.L) / m_deviceWidth;
    float deltaScaled = (dX / m_deviceWidth) * 0.5;
    float fovDelta = ((dX * c_maxFOV) / m_deviceWidth);
    glm::vec3 inputVec;

    // Fill in with event handling code...

    switch (m_interactionMode) {
    case InteractionMode::RotateView:
        // -theta to inverse the movement
        if (m_LMB) {
            inputVec[0] = -theta;
        }
        if (m_MMB) {
            inputVec[1] = -theta;
        }
        if (m_RMB) {
            inputVec[2] = -theta;
        }

        // must re-order the matrices since
        // (X Y Z) ^-1 = (Z^-1 Y^-1 X^-1)
        m_worldToView = makeRotationMatrixZYX(inputVec) * m_worldToView;
        break;
    case InteractionMode::TranslateView:
        // -theta to inverse the movement
        if (m_LMB) {
            inputVec[0] = -delta;
        }
        if (m_MMB) {
            inputVec[1] = -delta;
        }
        if (m_RMB) {
            inputVec[2] = -delta;
        }

        m_worldToView = makeTranslationMatrix(inputVec) * m_worldToView;

        break;
    case InteractionMode::Perspective:
        // -theta to inverse the movement
        if (m_LMB) {
            m_fov = clampValue(m_fov + fovDelta, c_maxFOV, c_minFOV);
        }
        if (m_MMB) {
            // TODO should I have min max for each
            m_nearDist = clampValue(m_nearDist + delta, c_maxNearFarDistance, c_minNearFarDistance);
        }
        if (m_RMB) {
            m_farDist = clampValue(m_farDist + delta, c_maxNearFarDistance, c_minNearFarDistance);
        }
        break;
    case InteractionMode::RotateModel:
        if (m_LMB) {
            inputVec[0] = theta;
        }
        if (m_MMB) {
            inputVec[1] = theta;
        }
        if (m_RMB) {
            inputVec[2] = theta;
        }

        m_modelToWorld = m_modelToWorld * makeRotationMatrixXYZ(inputVec);
        break;
    case InteractionMode::TranslateModel:
        if (m_LMB) {
            inputVec[0] = delta;
        }
        if (m_MMB) {
            inputVec[1] = delta;
        }
        if (m_RMB) {
            inputVec[2] = delta;
        }

        m_modelToWorld = m_modelToWorld * makeTranslationMatrix(inputVec);
        break;
    case InteractionMode::ScaleModel:
        if (m_LMB) {
            inputVec[0] = delta;
        }
        if (m_MMB) {
            inputVec[1] = delta;
        }
        if (m_RMB) {
            inputVec[2] = delta;
        }

        m_modelScale =
            clampValue(m_modelScale + inputVec, c_maxScale, c_minScale);
        break;
    case InteractionMode::ViewportMode:
        if (m_LMB) {
            m_deviceViewportCorners.second = deviceCoordToPoint(xPos, yPos, m_deviceWidth, m_deviceHeight);
        }
        updateViewportData();
        break;
    }

    prevXPos = xPos;
    prevYPos = yPos;
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

    double xPos, yPos;
    glfwGetCursorPos( m_window, &xPos, &yPos );

    // Fill in with event handling code...
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // The user clicked in the window.  If it's the left
        // mouse button, initiate a rotation.

        if (actions == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                m_LMB = true;

                if (m_interactionMode == InteractionMode::ViewportMode) {
                    m_deviceViewportCorners.first = deviceCoordToPoint(xPos, yPos, m_deviceWidth, m_deviceHeight);
                }
            }

            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                m_MMB = true;
            }

            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                m_RMB = true;
            }

            m_startInput = true;
        } else {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                m_LMB = false;
            }

            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                m_MMB = false;
            }

            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                m_RMB = false;
            }
        }
    }

    eventHandled = true;
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

    // scale the old viewport corners with the glfw window
    m_deviceViewportCorners =
        transformLine(m_deviceViewportCorners,
                      makeViewportTransformationMatrix(
                          makeDeviceViewport(m_deviceWidth, m_deviceHeight),
                          makeDeviceViewport((float)width, (float)height)));

    m_deviceWidth = width;
    m_deviceHeight = height;

    m_deviceToNDC = makeViewportTransformationMatrix(
        makeDeviceViewport((float)width, (float)height), m_NDCViewport);

    updateViewportData();

    eventHandled = true;
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
    if (action == GLFW_PRESS) {
        // Respond to some key events.

        // quit
        if (key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        }

        // reset
        if (key == GLFW_KEY_A) {
            reset();
        }

        // interaction modes
        if (key == GLFW_KEY_O) {
            m_interactionMode = InteractionMode::RotateView;
        }
        if (key == GLFW_KEY_E) {
            m_interactionMode = InteractionMode::TranslateView;
        }
        if (key == GLFW_KEY_P) {
            m_interactionMode = InteractionMode::Perspective;
        }
        if (key == GLFW_KEY_R) {
            m_interactionMode = InteractionMode::RotateModel;
        }
        if (key == GLFW_KEY_T) {
            m_interactionMode = InteractionMode::TranslateModel;
        }
        if (key == GLFW_KEY_S) {
            m_interactionMode = InteractionMode::ScaleModel;
        }
        if (key == GLFW_KEY_V) {
            m_interactionMode = InteractionMode::ViewportMode;
        }
    }

    resetMouseButtons();

    eventHandled = true;
    return eventHandled;
}
