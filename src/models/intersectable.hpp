#pragma once

#include "meshes/flatMesh.hpp"
#include "models/intersectionCurve.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"
#include "texture.hpp"

#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class Intersectable : public Model
{
	static constexpr int textureSize = 1024;
	using TextureData =
		std::array<std::array<std::array<unsigned char, 3>, textureSize>, textureSize>;
public:
	enum class Trim
	{
		none,
		red,
		green
	};

	using ChangeCallback = std::function<void(const std::vector<IntersectionCurve*>&)>;

	Intersectable(const glm::vec3& pos, const std::string& name,
		const ChangeCallback& changeCallback, const ShaderProgram& flatShaderProgram);

	virtual glm::vec3 surface(float u, float v) const = 0;
	virtual glm::vec3 surfaceDU(float u, float v) const = 0;
	virtual glm::vec3 surfaceDV(float u, float v) const = 0;

	glm::vec3 surface(const glm::vec2& pos) const;
	glm::vec3 surfaceDU(const glm::vec2& pos) const;
	glm::vec3 surfaceDV(const glm::vec2& pos) const;

	virtual bool uWrapped() const = 0;
	virtual bool vWrapped() const = 0;

	void addIntersectionCurve(IntersectionCurve* curve, int surfaceIndex);
	int intersectionCurveCount() const;
	std::string intersectionCurveName(int index) const;

	Trim getIntersectionCurveTrim(int index) const;
	void setIntersectionCurveTrim(int index, Trim trim);
	unsigned int getIntersectionCurveTextureId(int index) const;

protected:
	void notifyChange();
	void useTrim(const ShaderProgram& surfaceShaderProgram) const;

private:
	std::vector<IntersectionCurve*> m_intersectionCurves{};
	std::vector<std::shared_ptr<IntersectionCurve::DestroyCallback>>
		m_intersectionCurveDestroyNotifications{};
	std::vector<Trim> m_intersectionCurveTrims{};
	std::vector<Texture> m_intersectionCurveTextures{};
	std::optional<int> m_trimmingCurve = std::nullopt;
	const ShaderProgram& m_flatShaderProgram;

	ChangeCallback m_changeCallback;

	void registerForNotification(IntersectionCurve* curve);
	Texture createIntersectionCurveTexture(const IntersectionCurve* curve, int surfaceIndex);
	void intersectionCurveDestroyNotification(const IntersectionCurve* curve);
	std::unique_ptr<FlatMesh> createIntersectionMesh(
		const std::vector<glm::vec2>& intersectionPoints) const;
	void floodfill(TextureData& data) const;
	int getCurveIndex(const IntersectionCurve* curve) const;
	static glm::vec2 params2Tex(const glm::vec2& parameters);
};
