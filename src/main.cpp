#include "guis/gui.hpp"
#include "scene.hpp"
#include "shader_program.hpp"
#include "window.hpp"

#include <iostream>
#include <string>

int main()
{
	constexpr int initialWindowWidth = 1200;
	constexpr int initialWindowHeight = 700;

	Window window{initialWindowWidth, initialWindowHeight};
	Scene scene{initialWindowWidth, initialWindowHeight};
	GUI gui{window.getPtr(), scene, initialWindowWidth, initialWindowHeight};
	window.setUserData(scene, gui);

	while (!window.shouldClose())
	{
		gui.update();
		scene.update();
		window.clear();
		scene.render();
		gui.render();
		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}
