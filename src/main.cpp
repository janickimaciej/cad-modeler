#include "guis/gui.hpp"
#include "scene.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

int main()
{
	static constexpr glm::ivec2 initialWindowSize{1900, 1000};

	Window window{initialWindowSize};
	Scene scene{window.size()};
	GUI gui{window.getPtr(), scene, window.size()};
	window.setWindowData(scene, gui);

	while (!window.shouldClose())
	{
		gui.update();
		scene.update();
		scene.render();
		gui.render();
		window.swapBuffers();
		window.pollEvents();
	}

	return 0;
}
