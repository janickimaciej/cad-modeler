#include "guis/valueWindows/serializingWindow.hpp"

#include "serializer/sceneSerializer.hpp"

#include <imgui/imgui.h>

SerializingWindow::SerializingWindow(Scene& scene, const glm::ivec2& viewportSize) :
	ValueWindow{viewportSize},
	m_scene{scene}
{ }

void SerializingWindow::startSerializing()
{
	m_path[0] = '\0';
}

void SerializingWindow::apply()
{
	SceneSerializer serializer;
	serializer.serialize(m_scene, {m_path.data()});
}

std::string SerializingWindow::name()
{
	return "serializing";
}

int SerializingWindow::width()
{
	return 250;
}

void SerializingWindow::input()
{
	ImGui::InputText("##serializing", m_path.data(), m_path.size());
}
