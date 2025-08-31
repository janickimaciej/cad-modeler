#pragma once

#include "framebuffer.hpp"
#include "meshes/flatMesh.hpp"
#include "models/intersectionCurve.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"
#include "shaderPrograms.hpp"
#include "texture.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <vector>

class Intersectable : public Model
{
	static constexpr int smallTextureSize = 256;
	static constexpr int bigTextureSize = 1024;

	template <int size>
	using TextureData = std::array<std::array<std::array<unsigned char, 3>, size>, size>;

public:
	enum class Trim
	{
		none,
		red,
		green
	};

	using ChangeCallback = std::function<void(const std::vector<IntersectionCurve*>&)>;

	Intersectable(const glm::vec3& pos, const std::string& name,
		const ChangeCallback& changeCallback);

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
	std::vector<Texture> m_intersectionCurveSmallTextures{};
	std::vector<Texture> m_intersectionCurveBigTextures{};
	std::optional<int> m_trimmingCurve = std::nullopt;
	const ShaderProgram& m_flatShaderProgram = *ShaderPrograms::flat;

	ChangeCallback m_changeCallback;

	void registerForNotification(IntersectionCurve* curve);
	void intersectionCurveDestroyNotification(const IntersectionCurve* curve);
	std::unique_ptr<FlatMesh> createIntersectionMesh(
		const std::vector<glm::vec2>& intersectionPoints) const;
	int getCurveIndex(const IntersectionCurve* curve) const;
	static glm::vec2 params2Tex(const glm::vec2& parameters);

	template <int textureSize>
	Texture createIntersectionCurveTexture(const IntersectionCurve* curve, int surfaceIndex);

	template <int textureSize>
	void floodfill(TextureData<textureSize>& data) const;
};

template <int textureSize>
Texture Intersectable::createIntersectionCurveTexture(const IntersectionCurve* curve,
	int surfaceIndex)
{
	std::unique_ptr<TextureData<textureSize>> textureData =
		std::make_unique<TextureData<textureSize>>();

	Framebuffer framebuffer{{textureSize, textureSize}};
	std::vector<glm::vec2> intersectionPoints = curve->getIntersectionPoints(surfaceIndex);
	std::unique_ptr<FlatMesh> mesh = createIntersectionMesh(intersectionPoints);

	framebuffer.bind();

	glClearColor(0, 0.5f, 0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_flatShaderProgram.use();
	mesh->render();
	framebuffer.getTextureData((*textureData)[0][0].data());

	framebuffer.unbind();

	floodfill<textureSize>(*textureData);

	GLenum uWrap = uWrapped() ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	GLenum vWrap = vWrapped() ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	Texture texture{{textureSize, textureSize}, (*textureData)[0][0].data(), uWrap, vWrap};
	return texture;
}

template <int textureSize>
void Intersectable::floodfill(TextureData<textureSize>& data) const
{
	auto isEmpty =
		[] (const std::array<unsigned char, 3>& pixel)
		{
			return pixel[0] < 128;
		};
	auto fill =
		[] (std::array<unsigned char, 3>& pixel)
		{
			pixel[0] = 128;
			pixel[1] = 0;
		};

	int startX = 0;
	int startY = 0;
	while (!isEmpty(data[startY][startX]))
	{
		if (startX == 255)
		{
			if (startY == 255)
			{
				return;
			}
			++startY;
			startX = 0;
		}
		else
		{
			++startX;
		}
	}

	std::stack<glm::ivec2> stack{};
	fill(data[startY][startX]);
	stack.push({startX, startY});
	while (!stack.empty())
	{
		glm::ivec2 pixel = stack.top();
		stack.pop();

		if (pixel.x == 0 && uWrapped() && isEmpty(data[pixel.y][textureSize - 1]))
		{
			fill(data[pixel.y][textureSize - 1]);
			stack.push({textureSize - 1, pixel.y});
		}
		if (pixel.x != 0 && isEmpty(data[pixel.y][pixel.x - 1]))
		{
			fill(data[pixel.y][pixel.x - 1]);
			stack.push({pixel.x - 1, pixel.y});
		}

		if (pixel.x == textureSize - 1 && uWrapped() && isEmpty(data[pixel.y][0]))
		{
			fill(data[pixel.y][0]);
			stack.push({0, pixel.y});
		}
		if (pixel.x != textureSize - 1 && isEmpty(data[pixel.y][pixel.x + 1]))
		{
			fill(data[pixel.y][pixel.x + 1]);
			stack.push({pixel.x + 1, pixel.y});
		}

		if (pixel.y == 0 && vWrapped() && isEmpty(data[textureSize - 1][pixel.x]))
		{
			fill(data[textureSize - 1][pixel.x]);
			stack.push({pixel.x, textureSize - 1});
		}
		if (pixel.y != 0 && isEmpty(data[pixel.y - 1][pixel.x]))
		{
			fill(data[pixel.y - 1][pixel.x]);
			stack.push({pixel.x, pixel.y - 1});
		}

		if (pixel.y == textureSize - 1 && vWrapped() && isEmpty(data[0][pixel.x]))
		{
			fill(data[0][pixel.x]);
			stack.push({pixel.x, 0});
		}
		if (pixel.y != textureSize - 1 && isEmpty(data[pixel.y + 1][pixel.x]))
		{
			fill(data[pixel.y + 1][pixel.x]);
			stack.push({pixel.x, pixel.y + 1});
		}
	}
}
