#pragma once

#include "cameras/orthographic_camera.hpp"
#include "cameras/perspective_camera.hpp"
#include "camera_type.hpp"
#include "models/model.hpp"
#include "render_mode.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <vector>

class Window;

class Scene
{
public:
	Scene(float aspectRatio, Window& window);
	void render();

	Camera& getActiveCamera();
	Model& getActiveModel();

	void addPitch(float pitchRad);
	void addYaw(float yawRad);
	void addRadius(float radius);
	void moveX(float x);
	void moveY(float y);
	void zoom(float zoom);

	RenderMode getRenderMode() const;
	CameraType getCameraType() const;
	void setAspectRatio(float aspectRatio);
	void setRenderMode(RenderMode renderMode);
	void setCameraType(CameraType cameraType);

private:
	ShaderProgram m_wireframeShaderProgram{"src/shaders/wireframe_vertex_shader.glsl",
		"src/shaders/wireframe_fragment_shader.glsl"};
	ShaderProgram m_solidShaderProgram{"src/shaders/solid_vertex_shader.glsl",
		"src/shaders/solid_fragment_shader.glsl"};

	std::vector<std::unique_ptr<Model>> m_models{};
	Model* m_activeModel{};

	OrthographicCamera m_orthographicCamera;
	PerspectiveCamera m_perspectiveCamera;
	Camera* m_activeCamera{};

	RenderMode m_renderMode = RenderMode::wireframe;
	CameraType m_cameraType = CameraType::orthographic;

	float m_ambient = 0.1f;
	float m_diffuse = 0.5f;
	float m_specular = 0.5f;
	float m_shininess = 20.0f;

	void updateShaders() const;
};
