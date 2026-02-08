#pragma once

#include <string>

class Model;

class ModelGUI
{
public:
	ModelGUI(const Model& model);
	virtual ~ModelGUI() = default;

	virtual void update() = 0;

protected:
	std::string suffix() const;

private:
	const std::string m_suffix{};
};
