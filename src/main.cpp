#include "gui.hpp"
#include "scene.hpp"
#include "shader_program.hpp"
#include "window.hpp"

#include <string>

int main()
{
	constexpr int initialWindowWidth = 1200;
	constexpr int initialWindowHeight = 700;
	constexpr float initialAspectRatio =
		static_cast<float>(initialWindowWidth) / initialWindowHeight;

	Window window{initialWindowWidth, initialWindowHeight};
	Scene scene{initialAspectRatio, window};
	GUI gui{window.getPtr(), scene};

	while (!window.shouldClose())
	{
		gui.update();
		window.clear();
		scene.render();
		gui.render();
		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}
