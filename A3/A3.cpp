// Termm-Fall 2024

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <utility>

#include "helpers.hpp"

using namespace glm;

// consts
static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

static const InteractionMode c_interactionModeDefault = InteractionMode::PositionAndOrientation;
static const bool c_cullFrontDefault = false;
static const bool c_cullBackDefault = false;
static const bool c_zBufferDefault = true;

//----------------------------------------------------------------------------------------
// helpers
static inline void conditionallyEnableCulling(bool front, bool back)
{
	if (front || back) {
		glEnable(GL_CULL_FACE);
		if (front && back) {
			glCullFace(GL_FRONT_AND_BACK);
		}
		else if (front) {
			glCullFace(GL_FRONT);
		}
		else {
			glCullFace(GL_BACK);
		}
	}
}

static inline void disableCulling() {
	glDisable(GL_CULL_FACE);
}

static inline void conditionallyEnableDepthTesting(bool testing)
{
	if (testing) {
		glEnable(GL_DEPTH_TEST);
		// not actualy sure if we need this since the enabled vals are cleared between each frame?
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
    }
}

static inline void disableDepthTesting() {
		glDisable(GL_DEPTH_TEST);
}

// converts from top left originm x right, y down to centered origin, x right, y up
static inline std::pair<float, float>
topLeftOriginToCenteredOrigin(float viewportWidth, float viewportHeight,
							  float x, float y)
{
	// essentially the origin shift of the affine C.O.B.
	// flip y
	return {x - (viewportWidth / 2), -y + (viewportHeight / 2)};
}

// ~~~ trackball stuff ~~~
// trackball radius is a portion of the smallest measure between width/height
// portionOfSmallest
static inline float getTrackballRadius(float deviceWidth, float deviceHeight,
										float portionOfSmallest = 0.5)
{
	return portionOfSmallest * (((deviceWidth < deviceHeight) ? deviceWidth : deviceHeight) / 2);
}

// centered at the origin
// if outside circle, map to closest point on the sphere at z = 0
// outputs points on a unit sphere
static inline glm::vec3 circleToSphereMappingNormalized(float r, float x, float y)
{
	glm::vec3 vertex;
	// if outside of the trackball, send vertex to the edge of the trackball
	if ((x * x + y * y) >= r * r) {
		vertex = glm::vec3(x, y, 0);
	}
	else {
		vertex = glm::vec3(x, y, sqrt(r * r - (x * x + y * y)));
	}

	return glm::normalize(vertex);
}

static inline glm::vec3 circleToSphereMappingNormalized(float r, std::pair<float, float> coord)
{
	return circleToSphereMappingNormalized(r, coord.first, coord.second);
}

static inline glm::vec3 getArcballRotationAxis (glm::vec3 v1, glm::vec3 v2)
{
	glm::vec3 result = glm::cross(v1, v2);

	if (glm::length(result) == 0)
	{
		return result;
	} else
	{
		return glm::normalize(result);
	}
}

// in radians
static inline double getArcballRotationAngle (glm::vec3 v1, glm::vec3 v2)
{
	// sus clamp...
	// we work with normalized vectors, so dot is expected to be within [-1, 1],
	// but sometimes it is out of this range, so we return here if so
	return acos(clampValue(glm::dot(v1, v2), 1.0f, -1.0f));
}
//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{
	reset();
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.2, 0.5, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("blenderCube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("cone.obj"),
			getAssetFilePath("cylinder.obj"),
			getAssetFilePath("pyramid.obj"),
			getAssetFilePath("triangularPrism.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}


//----------------------------------------------------------------------------------------
void A3::reset() {
	m_interactionMode = c_interactionModeDefault;
	m_cullFront = c_cullFrontDefault;
	m_cullBack = c_cullBackDefault;
	m_zBuffer = c_zBufferDefault;
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	bool importResult = m_scene.importSceneGraph(import_lua(filename));
	if (!importResult) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("Phong.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("Phong.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(2.0f, 30.0f, 30.0f);
	m_light.rgbIntensity = vec3(0.8f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.1f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const mat4 & nodeTransformation,
		const vec3 & diffuseInfo,
		const vec3 & specularInfo,
		const float & phongCoefficient,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * nodeTransformation;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = diffuseInfo;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;

		location = shader.getUniformLocation("material.ks");
		vec3 ks = specularInfo;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;

		location = shader.getUniformLocation("material.shininess");
		float shininess = phongCoefficient;
		glUniform1fv(location, 1, &shininess);
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	// enables
	conditionallyEnableDepthTesting(m_zBuffer);
	conditionallyEnableCulling(m_cullFront, m_cullBack);

	renderScene(m_scene);


	disableDepthTesting();
	disableCulling();

	renderArcCircle();
}

//----------------------------------------------------------------------------------------
void A3::renderScene(Scene & scene) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

		for (Scene::PreOrderTraversalIterator nodeIt = scene.begin();
			 nodeIt != scene.end(); ++nodeIt) {

            if (nodeIt->m_nodeType != NodeType::GeometryNode)
                continue;

            const GeometryNode *geometryNode =
                static_cast<const GeometryNode *>(&(*nodeIt));

            updateShaderUniforms(m_shader,
                                 nodeIt.getInheritedTransformation() *
                                     geometryNode->trans,
                                 geometryNode->material.kd, geometryNode->material.ks,
                                 geometryNode->material.shininess, m_view);

            // Get the BatchInfo corresponding to the GeometryNode's unique
            // MeshId.
            BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

            //-- Now render the mesh:
            m_shader.enable();
            glDrawArrays(GL_TRIANGLES, batchInfo.startIndex,
                         batchInfo.numIndices);
            m_shader.disable();
        }

    glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::performTrackballRotation(float x1, float y1, float x2, float y2)
{
	float trackballRadius = getTrackballRadius(
		static_cast<float>(m_windowWidth), static_cast<float>(m_windowHeight));
	glm::vec3 v1 = circleToSphereMappingNormalized(
		trackballRadius, topLeftOriginToCenteredOrigin(
							 static_cast<float>(m_windowWidth),
							 static_cast<float>(m_windowHeight), x1, y1));
	glm::vec3 v2 = circleToSphereMappingNormalized(
		trackballRadius, topLeftOriginToCenteredOrigin(
							 static_cast<float> (m_windowWidth),
							 static_cast<float> (m_windowHeight), x2, y2));

	m_scene.rotate(getArcballRotationAxis(v1, v2),
				   getArcballRotationAngle(v1, v2));
}

//----------------------------------------------------------------------------------------
// operates on device coordinates
void A3::performXYTranslation(float x1, float y1, float x2, float y2)
{
	float windowWidth = static_cast<float>(m_windowWidth);
	float windowHeight = static_cast<float>(m_windowHeight);

	float scaleFactor = 2.0f; // TODO tweak

	float scaledX = ((x2 - x1) / windowWidth) * scaleFactor;
	// note that y is flipped due to device coordinates having positive y downwards
	float scaledY = ((y1 - y2) / windowHeight) * scaleFactor;

	m_scene.translate({scaledX, scaledY, 0});
}

//----------------------------------------------------------------------------------------
// operates on device coordinates
void A3::performZTranslation(float x1, float y1, float x2, float y2)
{
	float windowHeight = static_cast<float>(m_windowHeight);

	float scaleFactor = 2.0f;

	// note that positive y is downwards in device coordinates
	float scaledY = ((y2 - y1) / windowHeight) * scaleFactor;

	m_scene.translate({0, 0, scaledY});
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	// Fill in with event handling code...
    static double prevXPos;
    static double prevYPos;

	auto updatePrevPosition = [&] () {
		prevXPos = xPos;
		prevYPos = yPos;
	};

    if(m_startMouseInput) {
		updatePrevPosition();
        m_startMouseInput = false;
    }

	switch (m_interactionMode) {
	case InteractionMode::PositionAndOrientation:
		if (m_LMB) {
			performXYTranslation(prevXPos, prevYPos, xPos, yPos);
		}
		if (m_MMB) {
			performZTranslation(prevXPos, prevYPos, xPos, yPos);
		}
		if (m_RMB) {
			performTrackballRotation(prevXPos, prevYPos, xPos, yPos);
		}
		break;
	case InteractionMode::Joints:
		break;
	}

    updatePrevPosition();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	// Fill in with event handling code...
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // The user clicked in the window.  If it's the left
        // mouse button, initiate a rotation.

        if (actions == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                m_LMB = true;
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                m_MMB = true;
            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                m_RMB = true;
            }

            m_startMouseInput = true;
            eventHandled = true;
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
            eventHandled = true;
        }
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
