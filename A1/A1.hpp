// Termm--Fall 2024

#pragma once

#include <array>

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();
	void initWalls();
	void initAvatar();
	void initFloor();

	// helpers
	void reset();

	void updateMouseData();

	inline void setEntityColour(const int & entity, const std::array<float, 3> & colour);
	inline std::array<float, 3> getEntityColour(const int & entity) const;
	inline bool getWallState(const int & x, const int & z) const;
	inline glm::vec3 getWallScaleVec() const;
	inline glm::vec3 getFloorScaleVec() const;
	inline glm::vec3 getAvatarPosition() const;
	inline glm::mat4 translateMatrixToModelCenter(const glm::mat4 & W) const;

	void downsizeWalls();
	void upsizeWalls();

	inline void digMaze();

	inline void moveAvatarRight();
	inline void moveAvatarLeft();
	inline void moveAvatarDown();
	inline void moveAvatarUp();

	inline void digAndMoveAvatarRight();
	inline void digAndMoveAvatarLeft();
	inline void digAndMoveAvatarDown();
	inline void digAndMoveAvatarUp();

	inline void moveCameraIn();
	inline void moveCameraOut();

	// member vars
	Maze m_maze;
	int m_wallHeight;

	// colours
	std::array<float, 3> m_floorColour;
	std::array<float, 3> m_wallColour;
	std::array<float, 3> m_avatarColour;
	std::array<float, 3> colourFromGUI;
	int m_currentColEntity;

	// keyboard data
	double m_mouseXPrevPos;
	bool m_mouseLeftPressed;
	double m_mouseXDiffWhenPressed;
	bool m_isShiftPressed;

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to the wall geometry
	GLuint m_wall_vao; // Vertex Array Object
	GLuint m_wall_vbo; // Vertex Buffer Object
	GLuint m_wall_ebo; // Element Buffer Object
	size_t m_wallBlockCount; // # of indices

	// Fields related to the floor geometry
	GLuint m_floor_vao;
	GLuint m_floor_vbo;
	size_t m_floorCount;

	// Fields related to the avatar geometry
	GLuint m_avatar_vao; // Vertex Array Object
	GLuint m_avatar_vbo; // Vertex Buffer Object
	GLuint m_avatar_ebo; // Element Buffer Object
	size_t m_avatarCount; // # of indices

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;
	glm::mat4 m_worldTranslation; // matrix for world translation
	glm::mat4 m_worldRotation;

	glm::vec3 m_cameraPos;
	glm::vec3 m_cameraTarget;
};
