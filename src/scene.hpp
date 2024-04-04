#pragma once

#include "cameras/orthographic_camera.hpp"
#include "cameras/perspective_camera.hpp"
#include "camera_type.hpp"
#include "center_point.hpp"
#include "cursor.hpp"
#include "grid/grid.hpp"
#include "models/bezier_curve.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "models/torus.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"
#include "shader_programs.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <vector>

class Window;

class Scene
{
public:
	Scene(int windowWidth, int windowHeight);
	void update();
	void render();
	glm::ivec2 getWindowSize() const;
	void setWindowSize(int width, int height);

	const Camera& getActiveCamera() const;
	Camera& getActiveCamera();
	std::vector<Model*> getModels();
	void updateModelGUI(int i);
	void setModelIsActive(int i, bool isActive);
	Cursor& getCursor();
	CenterPoint& getActiveModelsCenter();

	void addPitchCamera(float pitchRad);
	void addYawCamera(float yawRad);
	void moveXCamera(float x);
	void moveYCamera(float y);
	void zoomCamera(float zoom);

	RenderMode getRenderMode() const;
	CameraType getCameraType() const;
	void setRenderMode(RenderMode renderMode);
	void setCameraType(CameraType cameraType);

	void addPoint();
	void addTorus();
	void addBezierCurve();
	void addActivePointsToCurve();

	void clearActiveModels();
	void deleteActiveModels();
	void activate(float xPos, float yPos, bool toggle);
	void release();
	void moveActiveModel(float xPos, float yPos) const;

	Model* getUniqueActiveModel() const;

private:
	ShaderPrograms m_shaderPrograms{};

	int m_windowWidth{};
	int m_windowHeight{};

	std::vector<Model*> m_models{};
	std::vector<std::unique_ptr<Point>> m_points{};
	std::vector<std::unique_ptr<Torus>> m_toruses{};
	std::vector<std::unique_ptr<BezierCurve>> m_bezierCurves{};

	Cursor m_cursor;
	CenterPoint m_activeModelsCenter{};
	bool m_dragging = false;

	static constexpr float gridScale = 10.0f;
	Grid m_grid{gridScale};
	
	PerspectiveCamera m_perspectiveCamera;
	OrthographicCamera m_orthographicCamera;
	Camera* m_activeCamera{};

	RenderMode m_renderMode = RenderMode::wireframe;
	CameraType m_cameraType = CameraType::perspective;

	float m_ambient = 0.1f;
	float m_diffuse = 0.5f;
	float m_specular = 0.5f;
	float m_shininess = 20.0f;
	
	void setAspectRatio(float aspectRatio);
	void renderModels() const;
	void renderCursor() const;
	void renderActiveModelsCenter();
	void renderGrid() const;
	void updateShaders() const;
	std::optional<int> getClosestModel(float xPos, float yPos) const;
	std::vector<Point*> getActivePoints() const;
};
