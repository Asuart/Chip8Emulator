#include "EmulatorWindow.h"

EmulatorWindow emulatorWindow(1280, 640);

void drop_callback(GLFWwindow* window, int count, const char** paths) {
	emulatorWindow.LoadROM(paths[0]);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	emulatorWindow.HandleResolutionChange(width, height);
	glViewport(0, 0, width, height);
}

int main(int argc, char** argv) {
	glfwSetWindowSizeCallback(emulatorWindow.mainWindow, window_size_callback);
	glfwSetDropCallback(emulatorWindow.mainWindow, drop_callback);

	if (argc > 2) {
		std::cout << "Programm uses first argument as a path to a ROM file. All other are ignored.\n";
	}
	std::cout << "Emulator can run some ROMs.\nYou can drag and drop ROM-files on window\n";
	std::cout << "Controls:\n1 2 3 4\nQ W E R\nA S D F\nZ X C V\n";
	std::cout << "\n";

	if (argc > 1) {
		emulatorWindow.LoadROM(argv[1]);
	}

	emulatorWindow.Start();

	return 0;
}