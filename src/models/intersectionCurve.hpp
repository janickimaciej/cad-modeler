#pragma once

#include "guis/modelGUIs/intersectionCurveGUI.hpp"
#include "meshes/mesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"
#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <vector>

class Intersectable;

class IntersectionCurve : public Model
{
	using PointPair = std::array<glm::vec2, 2>;

public:
	using DestroyCallback = std::function<void(IntersectionCurve*)>;

	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, float step,
		const glm::vec3& cursorPos);
	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, float step);
	virtual ~IntersectionCurve();

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3&) override;

	int pointCount() const;
	std::vector<glm::vec2> getIntersectionPoints(int surfaceIndex) const;
	std::vector<glm::vec3> getIntersectionPoints() const;
	bool isClosed() const;

	std::shared_ptr<DestroyCallback> registerForDestroyNotification(
		const DestroyCallback& callback);

private:
	static constexpr float m_startingTemperature = 0.5f;
	static constexpr int m_simulatedAnnealingIterations = static_cast<int>(1e4f);

	static int m_count;

	const ShaderProgram& m_shaderProgram;
	std::unique_ptr<Mesh> m_mesh{};
	std::array<const Intersectable*, 2> m_surfaces{};
	std::vector<PointPair> m_pointPairs{};
	bool m_isClosed{};
	IntersectionCurveGUI m_gui{*this};

	std::vector<std::weak_ptr<DestroyCallback>> m_destroyNotifications{};

	static std::unique_ptr<IntersectionCurve> create(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces, float step,
		const PointPair& startingPointPair);
	IntersectionCurve(const ShaderProgram& shaderProgram,
		const std::array<const Intersectable*, 2>& surfaces,
		const std::vector<PointPair>& pointPairs, bool isClosed);
	void createMesh();
	virtual void updateShaders() const override;
	void updatePos();

	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces,
		const glm::vec3& cursorPos);
	static PointPair findClosestSamples(const std::array<const Intersectable*, 2>& surfaces);
	static PointPair findClosestSamples(const Intersectable* surface, const glm::vec3& cursorPos);
	static PointPair findClosestSamples(const Intersectable* surface);

	static std::optional<PointPair> gradientMethod(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& startingPointPair);
	static std::vector<PointPair> findIntersectionPoints(
		const std::array<const Intersectable*, 2>& surfaces, float step,
		const PointPair& startingPointPair);
	static void normalizePoints(std::vector<PointPair>& pointPairs);
	static std::optional<PointPair> newtonMethod(
		const std::array<const Intersectable*, 2>& surfaces, float step,
		const std::optional<PointPair>& prevPointPair, const PointPair& startingPointPair,
		bool backwards = false);

	static float getDistanceSquared(const glm::vec3& pos1, const glm::vec3& pos2);
	static float getParametersDistanceSquared(const PointPair& pointPair, bool uWrapped,
		bool vWrapped);
	static bool outsideDomain(const std::array<const Intersectable*, 2>& surfaces,
		const PointPair& pointPair);
	static std::optional<glm::vec2> normalizeToDomain(const Intersectable* surface,
		const glm::vec2& point);
	static std::optional<PointPair> normalizeToDomain(
		const std::array<const Intersectable*, 2>& surfaces, const PointPair& pointPair);
	static PointPair vec4ToPointPair(const glm::vec4& vec);
	static std::optional<glm::vec4> pointPairToVec4(const std::optional<PointPair>& pointPair);

	void notifyDestroy();
	void clearExpiredNotifications();

	template <typename Vec>
	static Vec simulatedAnnealing(const std::function<float(const Vec&)>& function,
		const std::function<std::optional<Vec>(const Vec&)>& pointCheck, float startingTemperature,
		int iterations);

	template <typename Vec>
	static Vec randomVec(std::mt19937& generator);
};

template <typename Vec>
Vec IntersectionCurve::simulatedAnnealing(const std::function<float(const Vec&)>& lossFunction,
	const std::function<std::optional<Vec>(const Vec&)>& pointCheck, float startingTemperature,
	int iterations)
{
	std::mt19937 generator{0};
	std::uniform_real_distribution<float> uniformDistribution{0, 1};

	float temperature = startingTemperature;
	float dTemperature = startingTemperature / iterations;

	Vec point{0.5f};
	float value = std::numeric_limits<float>::max();

	while (temperature > 0)
	{
		std::optional<Vec> newPoint = std::nullopt;
		while (!newPoint.has_value())
		{
			newPoint = point + randomVec<Vec>(generator) * temperature;
			newPoint = pointCheck(*newPoint);
		}

		float newValue = lossFunction(*newPoint);
		if (newValue < value)
		{
			point = *newPoint;
			value = newValue;
		}
		else
		{
			float probability = std::exp((value - newValue) / temperature);
			if (probability > uniformDistribution(generator))
			{
				point = *newPoint;
				value = newValue;
			}
		}

		temperature -= dTemperature;
	}

	return point;
}

template <typename Vec>
Vec IntersectionCurve::randomVec(std::mt19937& generator)
{
	std::normal_distribution<float> normalDistribution{0, 1};

	Vec vec{};
	for (int i = 0; i < Vec::length(); ++i)
	{
		vec[i] = normalDistribution(generator);
	}
	return vec;
}
