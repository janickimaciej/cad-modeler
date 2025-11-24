#include "guis/leftPanel.hpp"

#include "cameras/cameraType.hpp"

#include <imgui/imgui.h>

#include <optional>
#include <string>

LeftPanel::LeftPanel(Scene& scene, const glm::ivec2& windowSize) :
	m_scene{scene},
	m_windowSize{windowSize},
	m_addC0BezierSurfacePanel
	{
		[this] (int patchesU, int patchesV, float sizeU, float sizeV,
			BezierSurfaceWrapping wrapping)
		{
			m_scene.addC0BezierSurface(patchesU, patchesV, sizeU, sizeV, wrapping);
			m_addC0BezierSurfacePanel.reset();
			m_mode = Mode::none;
		}
	},
	m_addC2BezierSurfacePanel
	{
		[this] (int patchesU, int patchesV, float sizeU, float sizeV,
			BezierSurfaceWrapping wrapping)
		{
			m_scene.addC2BezierSurface(patchesU, patchesV, sizeU, sizeV, wrapping);
			m_addC2BezierSurfacePanel.reset();
			m_mode = Mode::none;
		}
	},
	m_addGregorySurfacePanel
	{
		scene,
		[this] ()
		{
			m_addGregorySurfacePanel.reset();
			m_mode = Mode::none;
		}
	},
	m_addIntersectionPanel
	{
		scene,
		[this] ()
		{
			m_addIntersectionPanel.reset();
			m_mode = Mode::none;
		}
	},
	m_convertIntersectionToInterpolatingCurvePanel
	{
		scene,
		[this] ()
		{
			m_convertIntersectionToInterpolatingCurvePanel.reset();
			m_mode = Mode::none;
		}
	}
{ }

void LeftPanel::update()
{
	ImGui::SetNextWindowPos({0, 0}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({width, static_cast<float>(m_windowSize.y)}, ImGuiCond_Always);
	ImGui::Begin("leftPanel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushItemWidth(100);

	updateCamera();
	if (m_scene.getCameraType() == CameraType::perspective)
	{
		separator();
		updateAnaglyph();
	}
	separator();
	updateCursor();
	separator();
	updateSelectedModelsCenter();
	separator();
	updateButtons();

	ImGui::PopItemWidth();
	ImGui::End();
}

void LeftPanel::updateCamera()
{
	ImGui::Text("Camera");
	ImGui::Spacing();

	ImGui::PushItemWidth(120);

	CameraType cameraType = m_scene.getCameraType();
	CameraType prevCameraType = cameraType;
	if (ImGui::BeginCombo("##cameraType", cameraTypeLabels[static_cast<int>(cameraType)].c_str()))
	{
		for (int cameraTypeIndex = 0; cameraTypeIndex < cameraTypeCount; ++cameraTypeIndex)
		{
			bool isSelected = cameraTypeIndex == static_cast<int>(cameraType);
			if (ImGui::Selectable(cameraTypeLabels[cameraTypeIndex].c_str(), isSelected))
			{
				cameraType = static_cast<CameraType>(cameraTypeIndex);
			}
		}
		ImGui::EndCombo();
	}
	if (cameraType != prevCameraType)
	{
		m_scene.setCameraType(static_cast<CameraType>(cameraType));
	}

	ImGui::PopItemWidth();

	m_scene.updateActiveCameraGUI();
}

void LeftPanel::updateAnaglyph()
{
	bool anaglyphOn = m_scene.getAnaglyphOn();
	bool prevAnaglyphOn = anaglyphOn;
	ImGui::Checkbox("anaglyph", &anaglyphOn);
	if (anaglyphOn != prevAnaglyphOn)
	{
		m_scene.setAnaglyphOn(anaglyphOn);
	}

	if (anaglyphOn)
	{
		{
			static constexpr float stepPrecision = 1;
			static const std::string format = "%.1f";

			float eyesDistance = m_scene.getEyesDistance();
			float prevEyesDistance = eyesDistance;
			ImGui::InputFloat("eyes distance", &eyesDistance, stepPrecision, stepPrecision,
				format.c_str());
			if (eyesDistance != prevEyesDistance)
			{
				m_scene.setEyesDistance(eyesDistance);
			}
		}

		{
			static constexpr float stepPrecision = 10;
			static const std::string format = "%.0f";

			float screenDistance = m_scene.getScreenDistance();
			float prevScreenDistance = screenDistance;
			ImGui::InputFloat("screen distance", &screenDistance, stepPrecision, stepPrecision,
				format.c_str());
			if (screenDistance != prevScreenDistance)
			{
				m_scene.setScreenDistance(screenDistance);
			}
		}

		{
			static constexpr float stepPrecision = 0.1f;
			static const std::string format = "%.2f";

			float projectionPlane = m_scene.getProjectionPlane();
			float prevProjectionPlane = projectionPlane;
			ImGui::InputFloat("projection plane", &projectionPlane, stepPrecision, stepPrecision,
				format.c_str());
			if (projectionPlane != prevProjectionPlane)
			{
				m_scene.setProjectionPlane(projectionPlane);
			}
		}
	}
}

void LeftPanel::updateCursor()
{
	ImGui::Text("Cursor");
	ImGui::Spacing();
	m_scene.updateCursorGUI();
}

void LeftPanel::updateSelectedModelsCenter()
{
	ImGui::Text("Selected models");
	ImGui::Spacing();
	m_scene.updateSelectedModelsCenterGUI();
}

void LeftPanel::updateButtons()
{
	if (ImGui::Button("Add point"))
	{
		m_scene.addPoint();
	}

	separator();

	if (ImGui::Button("Add torus"))
	{
		m_scene.addTorus();
	}

	separator();

	if (ImGui::Button("Add C0 Bezier curve"))
	{
		m_scene.addC0BezierCurve();
	}

	separator();

	if (ImGui::Button("Add C2 Bezier curve"))
	{
		m_scene.addC2BezierCurve();
	}

	separator();

	if (ImGui::Button("Add interpolating curve"))
	{
		m_scene.addInterpolatingBezierCurve();
	}

	separator();

	if (ImGui::Button("Add points to curve"))
	{
		m_scene.addSelectedPointsToCurve();
	}

	separator();

	if (ImGui::Button("Add C0 Bezier surface"))
	{
		resetPanels();
		if (m_mode == Mode::c0BezierSurface)
		{
			m_mode = Mode::none;
		}
		else
		{
			m_mode = Mode::c0BezierSurface;
		}
	}

	if (m_mode == Mode::c0BezierSurface)
	{
		m_addC0BezierSurfacePanel.update();
	}

	separator();

	if (ImGui::Button("Add C2 Bezier surface"))
	{
		resetPanels();
		if (m_mode == Mode::c2BezierSurface)
		{
			m_mode = Mode::none;
		}
		else
		{
			m_mode = Mode::c2BezierSurface;
		}
	}

	if (m_mode == Mode::c2BezierSurface)
	{
		m_addC2BezierSurfacePanel.update();
	}

	separator();

	if (ImGui::Button("Add Gregory surface") && (m_scene.getModelCount(ModelType::c0BezierSurface) +
		m_scene.getModelCount(ModelType::c2BezierSurface) > 0))
	{
		resetPanels();
		if (m_mode == Mode::gregorySurface)
		{
			m_mode = Mode::none;
		}
		else
		{
			m_mode = Mode::gregorySurface;
		}
	}

	if (m_mode == Mode::gregorySurface)
	{
		m_addGregorySurfacePanel.update();
	}

	separator();

	if (ImGui::Button("Add intersection") && (m_scene.getModelCount(ModelType::torus) +
		m_scene.getModelCount(ModelType::bezierPatch) +
		m_scene.getModelCount(ModelType::c0BezierSurface) +
		m_scene.getModelCount(ModelType::c2BezierSurface) > 0))
	{
		resetPanels();
		if (m_mode == Mode::intersection)
		{
			m_mode = Mode::none;
		}
		else
		{
			m_mode = Mode::intersection;
		}
	}

	if (m_mode == Mode::intersection)
	{
		m_addIntersectionPanel.update();
	}

	separator();

	if (ImGui::Button("Convert inters. to interp. curve") &&
		m_scene.getModelCount(ModelType::intersectionCurve) > 0)
	{
		resetPanels();
		if (m_mode == Mode::convert)
		{
			m_mode = Mode::none;
		}
		else
		{
			m_mode = Mode::convert;
		}
	}

	if (m_mode == Mode::convert)
	{
		m_convertIntersectionToInterpolatingCurvePanel.update();
	}

	separator();

	if (ImGui::Button("Collapse 2 points"))
	{
		m_scene.collapse2Points();
	}

	separator();

	if (ImGui::Button("Magic"))
	{
		m_scene.magic();
	}

	separator();

	if (ImGui::Button("Generate heightmap"))
	{
		m_scene.generateHeightmap();
	}

	separator();

	if (ImGui::Button("Generate offset heightmaps"))
	{
		m_scene.generateOffsetHeightmaps();
	}

	separator();

	if (ImGui::Button("Generate path 1"))
	{
		m_scene.generatePath1();
	}
}

void LeftPanel::resetPanels()
{
	m_addC0BezierSurfacePanel.reset();
	m_addC2BezierSurfacePanel.reset();
	m_addGregorySurfacePanel.reset();
	m_addIntersectionPanel.reset();
	m_convertIntersectionToInterpolatingCurvePanel.reset();
}

void LeftPanel::separator()
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
}
