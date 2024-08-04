#pragma once

#include "cameras/orthographicCamera.hpp"
#include "cameras/perspectiveCamera.hpp"
#include "cameraType.hpp"
#include "centerPoint.hpp"
#include "cursor.hpp"
#include "grid/grid.hpp"
#include "models/bezierCurveC0.hpp"
#include "models/bezierCurveC2.hpp"
#include "models/bezierCurveInter.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "models/torus.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

class Window;

class Scene
{
public:
	Scene(const glm::ivec2& windowSize);
	void render();
	void updateWindowSize();

	const Camera& getActiveCamera() const;
	Camera& getActiveCamera();

	int getModelCount() const;
	bool isModelVirtual(int i) const;
	bool isModelActive(int i) const;
	std::string getModelName(int i) const;
	void updateModelGUI(int i);
	void setModelIsActive(int i, bool isActive);

	Cursor& getCursor();
	CenterPoint& getActiveModelsCenter();

	void addPitchCamera(float pitchRad);
	void addYawCamera(float yawRad);
	void moveXCamera(float x);
	void moveYCamera(float y);
	void zoomCamera(float zoom);

	CameraType getCameraType() const;
	void setCameraType(CameraType cameraType);

	void addPoint();
	void addTorus();
	void addBezierCurveC0();
	void addBezierCurveC2();
	void addBezierCurveInter();
	void addActivePointsToCurve();

	void clearActiveModels();
	void deleteActiveModels();
	void deleteEmptyBezierCurvesC0();
	void deleteEmptyBezierCurvesC2();
	void deleteEmptyBezierCurvesInter();
	void deleteUnreferencedVirtualPoints();
	bool select(const glm::vec2& screenPos, bool toggle);
	void moveUniqueActiveModel(const glm::vec2& screenPos) const;

	Model* getUniqueActiveModel() const;

private:
	ShaderPrograms m_shaderPrograms{};

	const glm::ivec2& m_windowSize{};

	std::vector<Model*> m_models{};
	std::vector<std::unique_ptr<Point>> m_points{};
	std::vector<std::unique_ptr<Torus>> m_toruses{};
	std::vector<std::unique_ptr<BezierCurveC0>> m_bezierCurvesC0{};
	std::vector<std::unique_ptr<BezierCurveC2>> m_bezierCurvesC2{};
	std::vector<std::unique_ptr<BezierCurveInter>> m_bezierCurvesInter{};

	Cursor m_cursor;
	CenterPoint m_activeModelsCenter{};

	static constexpr float gridScale = 10.0f;
	Grid m_grid{gridScale};
	
	PerspectiveCamera m_perspectiveCamera;
	OrthographicCamera m_orthographicCamera;
	Camera* m_activeCamera{};

	CameraType m_cameraType = CameraType::perspective;
	
	void setAspectRatio(float aspectRatio);
	void renderModels() const;
	void renderCursor() const;
	void renderActiveModelsCenter();
	void renderGrid() const;
	std::optional<int> getClosestModel(const glm::vec2& screenPos) const;
	std::vector<Point*> getNonVirtualActivePoints() const;
	void addVirtualPoints(std::vector<std::unique_ptr<Point>> points);
};
