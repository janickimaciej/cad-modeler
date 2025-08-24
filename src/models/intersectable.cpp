#include "models/intersectable.hpp"

#include "framebuffer.hpp"

#include <cstddef>
#include <stack>

Intersectable::Intersectable(const glm::vec3& pos, const std::string& name,
	const ChangeCallback& changeCallback, const ShaderProgram& flatShaderProgram) :
	Model{pos, name},
	m_flatShaderProgram{flatShaderProgram},
	m_changeCallback{changeCallback}
{ }

glm::vec3 Intersectable::surface(const glm::vec2& pos) const
{
	return surface(pos.x, pos.y);
}

glm::vec3 Intersectable::surfaceDU(const glm::vec2& pos) const
{
	return surfaceDU(pos.x, pos.y);
}

glm::vec3 Intersectable::surfaceDV(const glm::vec2& pos) const
{
	return surfaceDV(pos.x, pos.y);
}

void Intersectable::addIntersectionCurve(IntersectionCurve* curve, int surfaceIndex)
{
	m_intersectionCurves.push_back(curve);
	registerForNotification(curve);
	m_intersectionCurveTrims.push_back(Trim::none);
	m_intersectionCurveTextures.push_back(createIntersectionCurveTexture(curve, surfaceIndex));
}

int Intersectable::intersectionCurveCount() const
{
	return static_cast<int>(m_intersectionCurves.size());
}

std::string Intersectable::intersectionCurveName(int index) const
{
	return m_intersectionCurves[index]->getName();
}

Intersectable::Trim Intersectable::getIntersectionCurveTrim(int index) const
{
	return m_intersectionCurveTrims[index];
}

void Intersectable::setIntersectionCurveTrim(int index, Trim trim)
{
	for (Trim& trim : m_intersectionCurveTrims)
	{
		trim = Trim::none;
	}
	m_trimmingCurve = std::nullopt;

	if (trim != Trim::none)
	{
		m_intersectionCurveTrims[index] = trim;
		m_trimmingCurve = index;
	}
}

unsigned int Intersectable::getIntersectionCurveTextureId(int index) const
{
	return m_intersectionCurveTextures[index].getId();
}

void Intersectable::notifyChange()
{
	m_changeCallback(m_intersectionCurves);
}

void Intersectable::registerForNotification(IntersectionCurve* curve)
{
	m_intersectionCurveDestroyNotifications.push_back(curve->registerForDestroyNotification
		(
			[this] (IntersectionCurve* curve)
			{
				intersectionCurveDestroyNotification(curve);
			}
		));
}

Texture Intersectable::createIntersectionCurveTexture(const IntersectionCurve* curve,
	int surfaceIndex)
{
	std::unique_ptr<TextureData> textureData = std::make_unique<TextureData>();

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

	floodfill(*textureData);

	Texture texture{{textureSize, textureSize}, (*textureData)[0][0].data()};
	return texture;
}

void Intersectable::intersectionCurveDestroyNotification(const IntersectionCurve* curve)
{
	int curveIndex = getCurveIndex(curve);
	m_intersectionCurves.erase(m_intersectionCurves.begin() + curveIndex);
	m_intersectionCurveDestroyNotifications.erase(
		m_intersectionCurveDestroyNotifications.begin() + curveIndex);
	m_intersectionCurveTrims.erase(m_intersectionCurveTrims.begin() + curveIndex);
	m_intersectionCurveTextures.erase(m_intersectionCurveTextures.begin() + curveIndex);
	if (m_trimmingCurve.has_value() && *m_trimmingCurve == curveIndex)
	{
		m_trimmingCurve = std::nullopt;
	}
}

std::unique_ptr<FlatMesh> Intersectable::createIntersectionMesh(
	const std::vector<glm::vec2>& intersectionPoints) const
{
	std::vector<glm::vec2> vertices{};
	std::vector<unsigned int> indices{};

	int index = 0;
	glm::vec2 segmentStart{};
	glm::vec2 segmentEnd = intersectionPoints[0];
	vertices.push_back(params2Tex(segmentEnd));
	for (std::size_t segment = 0; segment < intersectionPoints.size() - 1; ++segment)
	{
		segmentStart = segmentEnd;
		segmentEnd = intersectionPoints[segment + 1];

		bool cross = false;
		glm::vec2 crossDirection{0, 0};
		float xDiff = segmentEnd.x - segmentStart.x;
		float yDiff = segmentEnd.y - segmentStart.y;
		if (xDiff > 0.5f)
		{
			crossDirection += glm::vec2{-1, 0};
			cross = true;
		}
		if (xDiff < -0.5f)
		{
			crossDirection += glm::vec2{1, 0};
			cross = true;
		}
		if (yDiff > 0.5f)
		{
			crossDirection += glm::vec2{0, -1};
			cross = true;
		}
		if (yDiff < -0.5f)
		{
			crossDirection += glm::vec2{0, 1};
			cross = true;
		}

		if (cross)
		{
			vertices.push_back(params2Tex(segmentEnd + crossDirection));
			indices.push_back(index);
			indices.push_back(++index);

			vertices.push_back(params2Tex(segmentStart - crossDirection));
			vertices.push_back(params2Tex(segmentEnd));
			indices.push_back(++index);
			indices.push_back(++index);
		}
		else
		{
			vertices.push_back(params2Tex(segmentEnd));
			indices.push_back(index);
			indices.push_back(++index);
		}
	}

	return std::make_unique<FlatMesh>(vertices, indices);
}

void Intersectable::floodfill(TextureData& data) const
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

	std::stack<glm::vec2> stack{};
	fill(data[startY][startX]);
	stack.push({startX, startY});
	while (!stack.empty())
	{
		glm::vec2 pixel = stack.top();
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

int Intersectable::getCurveIndex(const IntersectionCurve* curve) const
{
	auto iterator = std::find(m_intersectionCurves.begin(), m_intersectionCurves.end(), curve);
	return static_cast<int>(iterator - m_intersectionCurves.begin());
}

glm::vec2 Intersectable::params2Tex(const glm::vec2& parameters)
{
	static constexpr float visualAdjustment = 0.9999f;
	return
	{
		visualAdjustment * (2 * parameters.x - 1),
		visualAdjustment * (-2 * parameters.y + 1)
	};
}
