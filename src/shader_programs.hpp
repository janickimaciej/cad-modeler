#pragma once

#include "shader_program.hpp"

struct ShaderPrograms
{
	std::string shadersPath = "src/shaders/";
	ShaderProgram wireframe{shadersPath + "wireframe_vertex_shader.glsl",
		shadersPath + "wireframe_fragment_shader.glsl"};
	ShaderProgram solid{shadersPath + "solid_vertex_shader.glsl",
		shadersPath + "solid_fragment_shader.glsl"};
	ShaderProgram wireframePoint{shadersPath + "wireframe_point_vertex_shader.glsl",
		shadersPath + "wireframe_point_geometry_shader.glsl",
		shadersPath + "wireframe_point_fragment_shader.glsl"};
	ShaderProgram solidPoint{shadersPath + "solid_point_vertex_shader.glsl",
		shadersPath + "solid_point_geometry_shader.glsl",
		shadersPath + "solid_point_fragment_shader.glsl"};
	ShaderProgram cursor{shadersPath + "cursor_vertex_shader.glsl",
		shadersPath + "cursor_geometry_shader.glsl", shadersPath + "cursor_fragment_shader.glsl"};
	ShaderProgram grid{shadersPath + "grid_vertex_shader.glsl",
		shadersPath + "grid_fragment_shader.glsl"};
};
