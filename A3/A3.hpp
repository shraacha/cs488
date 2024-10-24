// Termm-Fall 2024

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "Scene.hpp"
#include "NodeID.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <set>

// helper structs and stuff

enum InteractionMode {
	PositionAndOrientation,
	Joints
};

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;
	void reset();

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderScene(Scene &scene);
	void renderArcCircle();

	// other functions
	void drawScene();
	void drawCircle();
	void drawPickingScene();

	void renderPickingScene(Scene &scene);

	void performTrackballRotation(float x1, float y1, float x2, float y2);
	void performXYTranslation(float x1, float y1, float x2, float y2);
	void performZTranslation(float x1, float y1, float x2, float y2);

	void applyJointRotationXAxis(SceneCommandList & commandList, double degrees);
    void applyJointRotationYAxis(SceneCommandList & commandList, double degrees);

	inline void setOpenGlClearToDefault();

	inline void pickObject(double xpos, double ypos);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	//-- GL resources for picking:
	// reuses mesh geometry info
	GLuint m_vao_picking;
	GLint m_picking_positionAttribLocation;
	ShaderProgram m_shader_picking;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	//-- Scene
	std::string m_luaSceneFile;

    // holds the scene graph
    Scene m_scene;

    IdCollection idSelections;

	void performTempJointRotationXAxis(double degrees);
	void performTempJointRotationYAxis(double degrees);

    SceneCommandList m_sceneCommands;
    SceneCommandList m_tempSceneCommands;

    //-- device info
    double deviceWidth;
    double deviceHeight;

    //-- interaction state info
    InteractionMode m_interactionMode;
    bool m_startMouseInput;
	double m_mouseInputStartingXPos;
	double m_mouseInputStartingYPos;
    bool m_LMB;
    bool m_MMB;
    bool m_RMB;

    struct uiData {
		// application
        bool resetPosition = false;
        bool resetOrientation = false;
        bool resetJoints = false;
        bool resetAll = false;
        bool quit = false;
		// edit
        bool undo = false;
        bool redo = false;
		// options
        bool circle = false;
        bool zBuffer = false;
        bool backface = false;
        bool frontface = false;
        bool pickingView = false;
    } m_uiData;
};
