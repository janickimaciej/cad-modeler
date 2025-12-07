#pragma once

#include "anaglyphMode.hpp"
#include "cameras/camera.hpp"
#include "cameras/cameraType.hpp"
#include "cameras/orthographicCamera.hpp"
#include "cameras/perspectiveCamera.hpp"
#include "centerPoint.hpp"
#include "cursor.hpp"
#include "framebuffer.hpp"
#include "heightmap.hpp"
#include "models/bezierCurves/c0BezierCurve.hpp"
#include "models/bezierCurves/c2BezierCurve.hpp"
#include "models/bezierCurves/interpolatingBezierCurve.hpp"
#include "models/bezierSurfaces/bezierPatch.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/bezierSurfaces/c0BezierSurface.hpp"
#include "models/bezierSurfaces/c2BezierSurface.hpp"
#include "models/gregorySurface.hpp"
#include "models/intersectionCurve.hpp"
#include "models/model.hpp"
#include "models/modelType.hpp"
#include "models/point.hpp"
#include "models/torus.hpp"
#include "plane/plane.hpp"
#include "quad.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>

static inline constexpr glm::ivec2 heightmapSize{3000, 3000};

class Scene
{
	friend class C0BezierCurveSerializer;
	friend class C2BezierCurveSerializer;
	friend class InterpolatingBezierCurveSerializer;
	friend class C0BezierSurfaceSerializer;
	friend class C2BezierSurfaceSerializer;
	friend class PointSerializer;
	friend class SceneSerializer;
	friend class TorusSerializer;

public:
	using TextureData = std::array<std::array<std::array<float, 3>, heightmapSize.x>,
		heightmapSize.y>;
	using HeightmapData = std::array<std::array<float, heightmapSize.x>, heightmapSize.y>;

	Scene(const glm::ivec2& windowSize);
	void update();
	void render();
	void updateWindowSize(const glm::ivec2& windowSize);

	CameraType getCameraType() const;
	void setCameraType(CameraType cameraType);

	void addPitchCamera(float pitchRad);
	void addYawCamera(float yawRad);
	void moveXCamera(float x);
	void moveYCamera(float y);
	void zoomCamera(float zoom);
	void moveCameraToSelectedModels();

	int getModelCount(ModelType type = ModelType::all) const;
	bool isAnyModelSelected() const;
	bool isOneModelSelected() const;
	bool isModelVirtual(int i, ModelType type = ModelType::all) const;
	bool isModelSelected(int i, ModelType type = ModelType::all) const;
	void selectModel(int i, ModelType type = ModelType::all);
	void deselectModel(int i, ModelType type = ModelType::all);
	void toggleModel(int i, ModelType type = ModelType::all);
	void deselectAllModels();
	void deleteSelectedModels();
	bool selectUniqueModel(const glm::vec2& screenPos);
	void toggleModel(const glm::vec2& screenPos);
	void moveUniqueSelectedModel(const glm::vec2& offset) const;
	void collapse2Points();
	BezierPatch* getUniqueSelectedBezierPatch() const;
	Intersectable* getUniqueSelectedIntersectable() const;
	bool isCursorAtPos(const glm::vec2& screenPos) const;
	void moveCursor(const glm::vec2& offset);
	void moveCursorToSelectedModels();

	void rotateXSelectedModels(float angleRad);
	void rotateYSelectedModels(float angleRad);
	void rotateZSelectedModels(float angleRad);
	void scaleXSelectedModels(float scale);
	void scaleYSelectedModels(float scale);
	void scaleZSelectedModels(float scale);

	std::string getUniqueSelectedModelName() const;
	void setUniqueSelectedModelName(const std::string& name) const;
	std::string getModelOriginalName(int i, ModelType type = ModelType::all) const;
	std::string getModelName(int i, ModelType type = ModelType::all) const;

	void addPoint();
	void addTorus();
	void addC0BezierCurve();
	void addC2BezierCurve();
	void addInterpolatingBezierCurve();
	void addSelectedPointsToCurve();
	void addC0BezierSurface(int patchesU, int patchesV, float sizeU, float sizeV,
		BezierSurfaceWrapping wrapping);
	void addC2BezierSurface(int patchesU, int patchesV, float sizeU, float sizeV,
		BezierSurfaceWrapping wrapping);
	void addGregorySurface(const std::array<BezierPatch*, 3>& patches);
	void addIntersectionCurve(const std::array<Intersectable*, 2>& surfaces, float step,
		bool useCursor);
	void convertIntersectionToInterpolatingCurve(int numberOfPoints);

	void updateActiveCameraGUI();
	void updateCursorGUI();
	void updateSelectedModelsCenterGUI();
	void updateModelGUI(int i, ModelType type = ModelType::all);

	bool getAnaglyphOn() const;
	void setAnaglyphOn(bool anaglyphOn);
	float getEyesDistance() const;
	void setEyesDistance(float eyesDistance);
	float getScreenDistance() const;
	void setScreenDistance(float screenDistance);
	float getProjectionPlane() const;
	void setProjectionPlane(float projectionPlane);

	void magic();
	void generateHeightmap();
	void generateOffsetHeightmap(float radius, bool flatCutter, float pathLevel = 0);
	std::unique_ptr<HeightmapData> getHeightmapData(Heightmap& heightmap);
	void generateEdge();
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		int xIndex, float z);
	static float getHeightmapHeight(float defaultHeight, const HeightmapData& heightmapData,
		float x, float z);
	static float getCurvatureRadius(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	void generatePath1();
	void generatePath2();
	void generatePath3();
	void generatePath4();

	Heightmap m_heightmap{heightmapSize};
	Heightmap m_offsetHeightmap{heightmapSize};
	Heightmap m_edge{heightmapSize};
	OrthographicCamera m_heightmapCamera;
	bool m_renderHeightmap = false;
	bool m_renderOffsetHeightmap = false;
	bool m_renderEdge = false;

private:
	std::vector<Model*> m_models{};
	std::vector<Model*> m_selectedModels{};
	std::vector<std::unique_ptr<Point>> m_points{};
	std::vector<std::unique_ptr<Torus>> m_toruses{};
	std::vector<std::unique_ptr<C0BezierCurve>> m_c0BezierCurves{};
	std::vector<std::unique_ptr<C2BezierCurve>> m_c2BezierCurves{};
	std::vector<std::unique_ptr<InterpolatingBezierCurve>> m_interpolatingBezierCurves{};
	std::vector<std::unique_ptr<BezierPatch>> m_bezierPatches{};
	std::vector<std::unique_ptr<C0BezierSurface>> m_c0BezierSurfaces{};
	std::vector<std::unique_ptr<C2BezierSurface>> m_c2BezierSurfaces{};
	std::vector<std::unique_ptr<GregorySurface>> m_gregorySurfaces{};
	std::vector<std::unique_ptr<IntersectionCurve>> m_intersectionCurves{};

	Cursor m_cursor{};
	CenterPoint m_selectedModelsCenter{m_selectedModels};

	static constexpr float gridScale = 7.5f;
	Plane m_plane{gridScale};

	PerspectiveCamera m_perspectiveCamera;
	OrthographicCamera m_orthographicCamera;
	Camera* m_activeCamera{};

	CameraType m_cameraType = CameraType::perspective;

	std::vector<const BezierCurve*> m_bezierCurvesToBeDeleted{};
	const BezierCurve::SelfDestructCallback m_bezierCurveSelfDestructCallback =
		[this] (const BezierCurve* curve)
		{
			addBezierCurveForDeletion(curve);
		};

	std::vector<const GregorySurface*> m_gregorySurfacesToBeDeleted{};
	const GregorySurface::SelfDestructCallback m_gregorySurfaceSelfDestructCallback =
		[this] (const GregorySurface* surface)
		{
			addGregorySurfaceForDeletion(surface);
		};

	Framebuffer m_leftEyeFramebuffer;
	Quad m_quad{};
	bool m_anaglyphOn = false;

	void setUpFramebuffer() const;
	void clearFramebuffer(AnaglyphMode anaglyphMode) const;

	void renderModels() const;
	void renderCursor() const;
	void renderSelectedModelsCenter() const;
	void renderGrid() const;

	Model* getUniqueSelectedModel() const;
	std::optional<int> getClosestModel(const glm::vec2& screenPos) const;
	std::vector<Point*> getNonVirtualSelectedPoints() const;
	void addPoints(std::vector<std::unique_ptr<Point>> points);
	void addBezierPatches(std::vector<std::unique_ptr<BezierPatch>> patches);

	void addBezierCurveForDeletion(const BezierCurve* curve);
	void addGregorySurfaceForDeletion(const GregorySurface* surface);
	void deleteEmptyBezierCurves();
	void deleteInvalidBezierPatches();
	void deleteInvalidGregorySurfaces();
	void deleteUnreferencedNonDeletablePoints();
	void deleteIntersectionCurves(const std::vector<IntersectionCurve*>& intersectionCurves);

	template <typename Type>
	void deleteSelectedModels(std::vector<std::unique_ptr<Type>>& models);
};

template <typename Type>
void Scene::deleteSelectedModels(std::vector<std::unique_ptr<Type>>& models)
{
	std::erase_if
	(
		models,
		[] (const std::unique_ptr<Type>& model)
		{
			return model->isSelected() && model->isDeletable();
		}
	);
}
