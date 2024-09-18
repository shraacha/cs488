// Termm--Fall 2024

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <cmath>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.hpp"

using namespace glm;
using namespace std;

static const size_t DIM = 16;

// Static helpers

static inline glm::mat4 rotateMatrixYByTheta(const glm::mat4 & W, const float & theta) {
	return glm::rotate(W, theta, vec3(0.0f, 1.0f, 0.0f));
}

static inline float calculateRotationAngleFromDistance(const double & distance) {
	return (float)(distance * c_distanceToAngleScale);
}

static inline glm::vec3 getDefaultCameraPos() {
	return glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 );
}

static inline glm::vec3 getDefaultCameraTarget() {
	return glm::vec3( 0.0f, 0.0f, 0.0f );
}

static inline glm::mat4 getViewMatrix(const glm::vec3 & camPosition, const glm::vec3 & camTarget) {
	return glm::lookAt(camPosition, camTarget, glm::vec3( 0.0f, 1.0f, 0.0f ));
}

// moves pos distance * units towards target, returns new pos
static inline glm::vec3 moveTowardsTarget(const glm::vec3 & position, const glm::vec3 & target, const float & distance) {
	return position - glm::normalize(position - target) * distance;
}

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: m_currentColEntity( 0 ),
	  m_maze(DIM),
	  colourFromGUI(c_defaultFloorColour),
	  m_floorColour(c_defaultFloorColour),
	  m_wallColour(c_defaultWallColour),
	  m_avatarColour(c_defaultAvatarColour),
	  m_wallHeight(c_defaultWallHeight),
	  m_worldTranslation(translateMatrixToModelCenter(glm::mat4())),
	  m_worldRotation(glm::mat4()),
	  m_cameraPos(getDefaultCameraPos()),
	  m_cameraTarget(getDefaultCameraTarget())
{
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
	initAvatar();
	initWalls();
	initFloor();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = getViewMatrix(m_cameraPos, m_cameraTarget);

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::initAvatar()
{
	// TODO for now, avatar is a cube. Set as sphere later.
	// Record buffer assignments in the vertex array
	glGenVertexArrays(1, &m_avatar_vao);
	glBindVertexArray(m_avatar_vao);

	// creating the vertex buffer
	glGenBuffers(1, &m_avatar_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_avatar_vbo);
	glBufferData(GL_ARRAY_BUFFER, c_unitCubeVertices.size() * sizeof(float), c_unitCubeVertices.data(), GL_DYNAMIC_DRAW);

	// creating the element buffer
	glGenBuffers(1, &m_avatar_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_avatar_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, c_unitCubeIndices.size() * sizeof(unsigned int), c_unitCubeIndices.data(), GL_DYNAMIC_DRAW);
	m_avatarCount =  c_unitCubeIndices.size();

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	CHECK_GL_ERRORS;
}

void A1::initWalls() {
	// the walls use a single cube defined at (0, 0, 0) and copy over the cube when drawing

	// Record buffer assignments in the vertex array
	glGenVertexArrays(1, &m_wall_vao);
	glBindVertexArray(m_wall_vao);

	// creating the vertex buffer
	glGenBuffers(1, &m_wall_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_wall_vbo);
	glBufferData(GL_ARRAY_BUFFER, c_unitCubeVertices.size() * sizeof(float), c_unitCubeVertices.data(), GL_STATIC_DRAW);

	// creating the element buffer
	glGenBuffers(1, &m_wall_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wall_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, c_unitCubeIndices.size() * sizeof(unsigned int), c_unitCubeIndices.data(), GL_STATIC_DRAW);
	m_wallBlockCount =  c_unitCubeIndices.size();

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	CHECK_GL_ERRORS;
}

void A1::initFloor() {
	glGenVertexArrays(1, &m_floor_vao);
	glBindVertexArray(m_floor_vao);

	// creating the vertex buffer
	glGenBuffers(1, &m_floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, c_unitSquareVertices.size() * sizeof(float), c_unitSquareVertices.data(), GL_STATIC_DRAW);
	m_floorCount = c_unitSquareVertices.size();

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// helpers

void A1::reset() {
	m_currentColEntity =  0;
	colourFromGUI = c_defaultFloorColour;
	m_floorColour = c_defaultFloorColour;
	m_wallColour = c_defaultWallColour;
	m_avatarColour = c_defaultAvatarColour;
	m_wallHeight = c_defaultWallHeight;
	m_worldTranslation = translateMatrixToModelCenter(glm::mat4());
	m_worldRotation = glm::mat4();
	m_cameraPos = getDefaultCameraPos();
	m_cameraTarget = getDefaultCameraTarget();
	m_maze.reset();
	m_mouseXDiffWhenPressed = 0;
}

void A1::updateMouseData() {
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		double xPos, yPos;
		glfwGetCursorPos( m_window, &xPos, &yPos );
		if(m_mouseLeftPressed) {
			m_mouseXDiffWhenPressed = xPos - m_mouseXPrevPos;
		}

		m_mouseXPrevPos = xPos;
	}
}

void A1::setEntityColour(const int &entity, const std::array<float, 3> &colour) {
	switch (entity) {
		case 0:
			m_floorColour = colour;
			break;
		case 1:
			m_wallColour = colour;
			break;
		case 2:
			m_avatarColour = colour;
			break;
		default:
			break;
	}
}

std::array<float, 3> A1::getEntityColour(const int &entity) const {
	switch (entity) {
		case 0:
			return m_floorColour;
		case 1:
			return m_wallColour;
		case 2:
			return m_avatarColour;
		default:
			return c_blackColour;
	}
}

bool A1::getWallState(const int & x, const int & z) const {
	return (bool)m_maze.getValue(x, z);
}

glm::vec3 A1::getWallScaleVec() const {
	return glm::vec3(1.0f, float(m_wallHeight), 1.0f);
}

glm::vec3 A1::getFloorScaleVec() const {
	return glm::vec3((float)DIM, 1.0f, (float)DIM);
}

// Position Relative To 0,0,0
glm::vec3 A1::getAvatarPosition() const {
	std::pair<int, int> playerPos = m_maze.getPlayerPos();
	return glm::vec3((float)playerPos.first, 0.0f, (float)playerPos.second);
}

glm::mat4 A1::translateMatrixToModelCenter(const glm::mat4 & W) const {
	return glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
}

void A1::downsizeWalls() {
	m_wallHeight = (m_wallHeight - c_wallHeightStepSize) <= c_minWallHeight ? c_minWallHeight : (m_wallHeight - c_wallHeightStepSize);
}

void A1::upsizeWalls() {
	m_wallHeight = (m_wallHeight + c_wallHeightStepSize) >= c_maxWallHeight ? c_maxWallHeight : (m_wallHeight + c_wallHeightStepSize);
}

void A1::digMaze() {
	m_maze.digMaze();
	m_maze.movePlayerToStart();
	// std::cout << "The current maze: " << std::endl;
	// m_maze.printMaze();
}

void A1::moveAvatarRight() {
	m_maze.movePlayerRight();
}
void A1::moveAvatarLeft() {
	m_maze.movePlayerLeft();
}
void A1::moveAvatarDown() {
	m_maze.movePlayerDown();
}
void A1::moveAvatarUp() {
	m_maze.movePlayerUp();
}

void A1::digAndMoveAvatarRight() {
	m_maze.digAndMovePlayerRight();
}
void A1::digAndMoveAvatarLeft() {
	m_maze.digAndMovePlayerLeft();
}
void A1::digAndMoveAvatarDown() {
	m_maze.digAndMovePlayerDown();
}
void A1::digAndMoveAvatarUp() {
	m_maze.digAndMovePlayerUp();
}

void A1::moveCameraIn() {
	if (glm::length(m_cameraPos - m_cameraTarget - c_worldScaleStep) >= c_camMinDist) {
		m_cameraPos = moveTowardsTarget(m_cameraPos, m_cameraTarget, c_worldScaleStep);
	}
}

void A1::moveCameraOut() {
	if (glm::length(m_cameraPos - m_cameraTarget + c_worldScaleStep) <= c_camMaxDist) {
		m_cameraPos = moveTowardsTarget(m_cameraPos, m_cameraTarget, -c_worldScaleStep);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...

	// updating mouse data
	updateMouseData();

	// update the world rotation based on mouse data
	m_worldRotation = rotateMatrixYByTheta(m_worldRotation, calculateRotationAngleFromDistance(m_mouseXDiffWhenPressed));
	view = getViewMatrix(m_cameraPos, m_cameraTarget);
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);


	    // ~~~~~~~~~~~~~~~~~~~
		ImGui::Separator();
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		ImGui::SameLine();

		if( ImGui::Button( "Reset Application (R)" ) ) {
			reset();
		}
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2 / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2 / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2 / 7.0f, 0.8f, 0.8f));
		if( ImGui::Button( "Dig Maze (D)" ) ) {
			digMaze();
		}
		ImGui::PopStyleColor(3);

	    // ~~~~~~~~~~~~~~~~~~~
		ImGui::Separator();
		ImGui::Text( "Settings:");
		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		// Colour picker
		ImGui::PushID( 0 );
		ImGui::Text("Set colour for:"); ImGui::SameLine();
		ImGui::RadioButton( "Floor", &m_currentColEntity, 0 ); ImGui::SameLine();
		ImGui::RadioButton( "Wall", &m_currentColEntity, 1 ); ImGui::SameLine();
		ImGui::RadioButton( "Avatar", &m_currentColEntity, 2 ); ImGui::SameLine();
		colourFromGUI = getEntityColour(m_currentColEntity);
		ImGui::ColorEdit3( "##color", colourFromGUI.data() );
		setEntityColour(m_currentColEntity, colourFromGUI);
		ImGui::PopID();

		// Growing Bars (space/backspace)
		ImGui::Text("Wall height: (BACKSPACE)"); ImGui::SameLine();
		ImGui::SliderInt("##Wall Height", &m_wallHeight, c_minWallHeight, c_maxWallHeight); ImGui::SameLine();
		ImGui::Text("(SPACE)");

	    // ~~~~~~~~~~~~~~~~~~~
		ImGui::Separator();
	    // List other controls:
		ImGui::Text( "Controls:");
		ImGui::Text( "- Use the arrow keys to move the avatar.");
		ImGui::Text( "- Click with LMB and drag left/right to rotate the maze.");
		ImGui::Text( "- Release LMB while dragging to spin freely.");
		ImGui::Text( "- Scroll up/down to scale the model up/down.");
		ImGui::Text( "- Press left/right shift while walking into a wall to destroy and move into it.");

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/
	    // ~~~~~~~~~~~~~~~~~~~
		ImGui::Separator();
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		// ImGui::Text( "Camera distance from target: %f units", glm::length(m_cameraPos - m_cameraTarget));

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	m_shader.enable();
		glEnable( GL_DEPTH_TEST );
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);


		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );

		// Draw the grid
		glBindVertexArray( m_grid_vao );
	    mat4 gridTransform = m_worldRotation * m_worldTranslation;
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( gridTransform ) );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

		// Draw the floor
	    // Floor Transform matrix
	    mat4 floorTransform = glm::scale(m_worldTranslation, getFloorScaleVec());
		glUniform3f( col_uni, m_floorColour[0], m_floorColour[1], m_floorColour[2] );
		floorTransform = m_worldRotation * floorTransform;
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( floorTransform ) );
		glBindVertexArray(m_floor_vao);
		glDrawArrays(GL_TRIANGLES, 0, m_floorCount);

		// Draw the walls
	    // Wall Transform matrix
	    mat4 wallTransform = glm::scale(glm::mat4(), getWallScaleVec());
	    mat4 individualWallTransform;
		glUniform3f( col_uni, m_wallColour[0], m_wallColour[1], m_wallColour[2] );

		// draw the individual wall blocks
		for(int i = 0; i < DIM; i++) {
			for(int j = 0; j < DIM; j++) {
				if (getWallState(j, i) == 1) {
					mat4 individualWallTranslate = glm::translate(glm::mat4(), vec3(float(j), 0, float(i))); // translate the wall to the right position
					individualWallTransform = m_worldRotation * individualWallTranslate * m_worldTranslation * wallTransform;
					glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( individualWallTransform ) );
					glBindVertexArray(m_wall_vao);
					glDrawElements(GL_TRIANGLES, m_wallBlockCount, GL_UNSIGNED_INT, 0);
				}
			}
		}

		// Draw the avatar
		mat4 avatarTranslate = glm::translate(glm::mat4(), getAvatarPosition()); // TODO update on avatar position change for efficiency
		mat4 avatarTransform = m_worldRotation * avatarTranslate * m_worldTranslation;
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( avatarTransform ) );
		glUniform3f( col_uni, m_avatarColour[0], m_avatarColour[1], m_avatarColour[2] );

		glBindVertexArray(m_avatar_vao);
		glDrawElements(GL_TRIANGLES, m_avatarCount, GL_UNSIGNED_INT, 0);


		// Highlight the active square. TODO

	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.

		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
			m_mouseLeftPressed = true;
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
			m_mouseLeftPressed = false;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	// Zoom in or out.
	if (yOffSet < 0) {
		moveCameraOut();
	} else if (yOffSet > 0) {
		moveCameraIn();
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	// re-calculate the proj matrix due to window resize
	proj = glm::perspective(
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
		// quit
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		// reset
		if (key == GLFW_KEY_R) {
			reset();
		}

		// dig maze
		if (key == GLFW_KEY_D) {
			digMaze();
		}

		// scaling walls
		if (key == GLFW_KEY_SPACE) {
			upsizeWalls();
		}
		if (key == GLFW_KEY_BACKSPACE) {
			downsizeWalls();
		}

		// movement
		if (key == GLFW_KEY_RIGHT) {
			m_isShiftPressed ? digAndMoveAvatarRight() : moveAvatarRight();
		}
		if (key == GLFW_KEY_LEFT) {
			m_isShiftPressed ? digAndMoveAvatarLeft() : moveAvatarLeft();;
		}
		if (key == GLFW_KEY_DOWN) {
			m_isShiftPressed ? digAndMoveAvatarDown() : moveAvatarDown();;
		}
		if (key == GLFW_KEY_UP) {
			m_isShiftPressed ? digAndMoveAvatarUp() : moveAvatarUp();;
		}

		// directional digging
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			m_isShiftPressed = true;
		}
	} else if (action == GLFW_RELEASE) {
		// directional digging
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			m_isShiftPressed = false;
		}
	}
	return eventHandled;
}
