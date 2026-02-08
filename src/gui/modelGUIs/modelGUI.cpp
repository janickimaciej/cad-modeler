#include "gui/modelGUIs/modelGUI.hpp"

#include "models/model.hpp"

ModelGUI::ModelGUI(const Model& model) :
	m_suffix{"##" + model.getOriginalName()}
{ }

std::string ModelGUI::suffix() const
{
	return m_suffix;
}
