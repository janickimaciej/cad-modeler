#pragma once

#include <array>
#include <string>

constexpr int cameraTypeCount = 2;

enum class CameraType
{
	orthographic,
	perspective
};

inline std::array<std::string, cameraTypeCount> cameraTypeLabels = {"orthographic", "perspective"};
