#include "guis/valueWindows/deserializingWindow.hpp"

#include "serializer/serializer.hpp"

#include <imgui/imgui.h>

DeserializingWindow::DeserializingWindow(Scene& scene, const glm::ivec2& windowSize) :
	ValueWindow{windowSize},
	m_scene{scene}
{ }

void DeserializingWindow::startDeserializing()
{
	m_path[0] = '\0';
}

void DeserializingWindow::apply()
{
	Serializer serializer;
	serializer.deserialize(m_scene, {m_path.data()});
}

std::string DeserializingWindow::name()
{
	return "deserializing";
}

int DeserializingWindow::width()
{
	return 250;
}

void DeserializingWindow::input()
{
	ImGui::InputText("##deserializing", m_path.data(), m_path.size());
}
