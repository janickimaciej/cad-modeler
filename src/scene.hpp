#pragma once

#include "cameras/camera.hpp"
#include "cameras/cameraType.hpp"
#include "cameras/orthographicCamera.hpp"
#include "cameras/perspectiveCamera.hpp"
#include "centerPoint.hpp"
#include "cursor.hpp"
#include "grid/grid.hpp"
#include "models/bezierCurves/bezierCurveC0.hpp"
#include "models/bezierCurves/bezierCurveC2.hpp"
#include "models/bezierCurves/bezierCurveInter.hpp"
#include "models/bezierSurfaces/bezierSurfaceC0.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/model.hpp"
#include "models/point.hpp"
#include "models/torus.hpp"
#include "shaderPrograms.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

class Scene
{
public:
	Scene(const glm::ivec2& windowSize);
	void update();
	void render() const;
	void updateWindowSize();

	CameraType getCameraType() const;
	void setCameraType(CameraType cameraType);

	void addPitchCamera(float pitchRad);
	void addYawCamera(float yawRad);
	void moveXCamera(float x);
	void moveYCamera(float y);
	void zoomCamera(float zoom);

	int getModelCount() const;
	bool isAnyModelSelected() const;
	bool isOneModelSelected() const;
	bool isModelVirtual(int i) const;
	bool isModelSelected(int i) const;
	void selectModel(int i);
	void deselectModel(int i);
	void toggleModel(int i);
	void deselectAllModels();
	void deleteSelectedModels();
	bool selectUniqueModel(const glm::vec2& screenPos);
	void toggleModel(const glm::vec2& screenPos);
	void moveUniqueSelectedModel(const glm::vec2& screenPos) const;

	void rotateXSelectedModels(float angleRad);
	void rotateYSelectedModels(float angleRad);
	void rotateZSelectedModels(float angleRad);
	void scaleXSelectedModels(float scale);
	void scaleYSelectedModels(float scale);
	void scaleZSelectedModels(float scale);

	std::string getUniqueSelectedModelName() const;
	void setUniqueSelectedModelName(const std::string& name) const;
	std::string getModelName(int i) const;

	void addPoint();
	void addTorus();
	void addBezierCurveC0();
	void addBezierCurveC2();
	void addBezierCurveInter();
	void addSelectedPointsToCurve();
	void addBezierSurfaceC0(int patchesU, int patchesV, float sizeU, float sizeV,
		BezierSurfaceWrapping wrapping);

	void updateActiveCameraGUI();
	void updateCursorGUI();
	void updateSelectedModelsCenterGUI();
	void updateModelGUI(int i);

private:
	ShaderPrograms m_shaderPrograms{};

	const glm::ivec2& m_windowSize{};

	std::vector<Model*> m_models{};
	std::vector<Model*> m_selectedModels{};
	std::vector<std::unique_ptr<Point>> m_points{};
	std::vector<std::unique_ptr<Torus>> m_toruses{};
	std::vector<std::unique_ptr<BezierCurveC0>> m_bezierCurvesC0{};
	std::vector<std::unique_ptr<BezierCurveC2>> m_bezierCurvesC2{};
	std::vector<std::unique_ptr<BezierCurveInter>> m_bezierCurvesInter{};
	std::vector<std::unique_ptr<BezierSurfaceC0>> m_bezierSurfacesC0{};

	Cursor m_cursor{m_shaderPrograms.cursor};
	CenterPoint m_selectedModelsCenter{m_shaderPrograms.cursor, m_selectedModels};

	static constexpr float gridScale = 10.0f;
	Grid m_grid{m_shaderPrograms.grid, gridScale};
	
	PerspectiveCamera m_perspectiveCamera;
	OrthographicCamera m_orthographicCamera;
	Camera* m_activeCamera{};

	CameraType m_cameraType = CameraType::perspective;
	
	std::vector<const BezierCurve*> m_bezierCurvesToBeDeleted{};
	const BezierCurve::SelfDestructCallback m_curveSelfDestructCallback =
		[this] (const BezierCurve* curve)
		{
			addBezierCurveForDeletion(curve);
		};
	
	void setAspectRatio(float aspectRatio);
	void renderModels() const;
	void renderCursor() const;
	void renderSelectedModelsCenter() const;
	void renderGrid() const;

	Model* getUniqueSelectedModel() const;
	std::optional<int> getClosestModel(const glm::vec2& screenPos) const;
	std::vector<Point*> getNonVirtualSelectedPoints() const;
	void addVirtualPoints(std::vector<std::unique_ptr<Point>> points);

	void addBezierCurveForDeletion(const BezierCurve* curve);
	void deleteEmptyBezierCurves();
	void deleteUnreferencedVirtualPoints();

	template <typename ModelType>
	void deleteSelectedModels(std::vector<std::unique_ptr<ModelType>>& models);
};

template <typename ModelType>
void Scene::deleteSelectedModels(std::vector<std::unique_ptr<ModelType>>& models)
{
	std::erase_if
	(
		models,
		[] (const std::unique_ptr<ModelType>& model)
		{
			return model->isSelected() && !model->isVirtual();
		}
	);
}
