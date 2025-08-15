#pragma once

#include "intersectable.hpp"
#include "models/bezierSurfaces/bezierSurface.hpp"
#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"
#include "models/bezierSurfaces/bezierPatch.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <memory>
#include <vector>

class C0BezierSurface : public BezierSurface
{
	friend class C0BezierSurfaceSerializer;

public:
	C0BezierSurface(const Intersectable::ChangeCallback& changeCallback,
		const ShaderProgram& bezierSurfaceShaderProgram,
		const ShaderProgram& bezierSurfaceGridShaderProgram,
		const ShaderProgram& pointShaderProgram, int patchesU, int patchesV, const glm::vec3& pos,
		float sizeU, float sizeV, BezierSurfaceWrapping wrapping,
		std::vector<std::unique_ptr<Point>>& points,
		std::vector<std::unique_ptr<BezierPatch>>& patches);
	virtual ~C0BezierSurface() = default;

private:
	static int m_count;

	virtual std::vector<std::unique_ptr<Point>> createPoints(
		const ShaderProgram& pointShaderProgram, const glm::vec3& pos, float sizeU,
		float sizeV) override;
	virtual void createGridMesh() override;
	virtual void updateGridMesh() override;
	virtual std::array<std::array<Point*, 4>, 4> getBezierPoints(std::size_t patchU,
		std::size_t patchV) const override;
};
