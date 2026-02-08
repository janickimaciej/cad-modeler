#include "gui/modelGUIs/bezierPatchGUI.hpp"

#include "models/bezierSurfaces/bezierPatch.hpp"

BezierPatchGUI::BezierPatchGUI(BezierPatch& patch) :
	ModelGUI{patch},
	m_patch{patch}
{ }

void BezierPatchGUI::update()
{ }
