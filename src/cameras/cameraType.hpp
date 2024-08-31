#pragma once

#include <array>
#include <string>

inline constexpr int cameraTypeCount = 2;

enum class CameraType
{
	orthographic,
	perspective
};

inline const std::array<const std::string, cameraTypeCount> cameraTypeLabels
{
	"Orthographic",
	"Perspective"
};
