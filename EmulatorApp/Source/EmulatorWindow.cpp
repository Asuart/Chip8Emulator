#include "EmulatorWindow.h"

EmulatorWindow::EmulatorWindow(uint32_t _width, uint32_t _height)
	: width(_width), height(_height) {
	if (glfwInit() != GL_TRUE) {
		std::cout << "GLFW failed to initialize\n";
		exit(1);
	}

	mainWindow = glfwCreateWindow(width, height, "NES Emulator", NULL, NULL);
	glfwMakeContextCurrent(mainWindow);
	glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW failed to initialize\n";
		exit(2);
	}

	screenPlane = new ScreenPlane();
	screenUploader = new TextureUploader(emulator.screenTexture);
}

EmulatorWindow::~EmulatorWindow() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
	delete screenPlane;
	delete screenUploader;
}

void EmulatorWindow::HandleResolutionChange(uint32_t _width, uint32_t _height) {
	width = _width;
	height = _height;
}

bool EmulatorWindow::LoadROM(const std::string& romPath) {
	std::cout << "Loading rom: \"" << romPath << "\"\n";
	std::ifstream reader(romPath, std::ifstream::binary | std::fstream::ate);
	if (!reader) {
		std::cout << "Could not open ROM file: \"" << romPath << "\"\n";
		return false;
	}

	uint32_t romSize = (uint32_t)reader.tellg();
	uint8_t* ROMdata = new uint8_t[romSize];
	reader.seekg(0, reader.beg);
	reader.read((char*)ROMdata, romSize);
	reader.close();

	bool loaded = emulator.LoadROM(ROMdata, romSize);

	delete[] ROMdata;

	return loaded;
}

void EmulatorWindow::Start() {
	const double targetFPS = 60;
	const double frameTime = 1.0 / targetFPS;
	const uint32_t instructionsPerInputPull = 50;
	const uint32_t instructionsPerSecond = 1000;
	double timeAccumulator = 0;
	double lastTime = glfwGetTime();
	uint32_t instructionCounter = 0;
	while (!glfwWindowShouldClose(mainWindow)) {
		glfwPollEvents();
		if ((instructionCounter % instructionsPerInputPull) == 0) {
			UpdateKeyStates();
		}
		
		emulator.Run(1);

		instructionCounter++;
		if ((instructionCounter % instructionsPerSecond) == 0) {
			glClear(GL_COLOR_BUFFER_BIT);

			glUseProgram(screenPlane->shader);

			GLint aspectLoc = glGetUniformLocation(screenPlane->shader, "aspect");
			screenUploader->Upload();
			screenUploader->Bind(GL_TEXTURE0);
			float aspect = ((float)emulator.screenTexture.width / emulator.screenTexture.height) / ((float)width / height);
			glUniform1f(aspectLoc, aspect);
			glBindVertexArray(screenPlane->vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			glfwSwapBuffers(mainWindow);

			if (timeAccumulator < frameTime) {
				glfwWaitEventsTimeout(frameTime - timeAccumulator);
			}
			timeAccumulator = 0;
		}

		double newTime = glfwGetTime();
		timeAccumulator += newTime - lastTime;
		lastTime = newTime;
	}
}

void EmulatorWindow::UpdateKeyStates() {
	emulator.keys[0x1] = glfwGetKey(mainWindow, GLFW_KEY_1) > 0 ? 1 : 0;
	emulator.keys[0x2] = glfwGetKey(mainWindow, GLFW_KEY_2) > 0 ? 1 : 0;
	emulator.keys[0x3] = glfwGetKey(mainWindow, GLFW_KEY_3) > 0 ? 1 : 0;
	emulator.keys[0xc] = glfwGetKey(mainWindow, GLFW_KEY_4) > 0 ? 1 : 0;

	emulator.keys[0x4] = glfwGetKey(mainWindow, GLFW_KEY_Q) > 0 ? 1 : 0;
	emulator.keys[0x5] = glfwGetKey(mainWindow, GLFW_KEY_W) > 0 ? 1 : 0;
	emulator.keys[0x6] = glfwGetKey(mainWindow, GLFW_KEY_E) > 0 ? 1 : 0;
	emulator.keys[0xd] = glfwGetKey(mainWindow, GLFW_KEY_R) > 0 ? 1 : 0;

	emulator.keys[0x7] = glfwGetKey(mainWindow, GLFW_KEY_A) > 0 ? 1 : 0;
	emulator.keys[0x8] = glfwGetKey(mainWindow, GLFW_KEY_S) > 0 ? 1 : 0;
	emulator.keys[0x9] = glfwGetKey(mainWindow, GLFW_KEY_D) > 0 ? 1 : 0;
	emulator.keys[0xe] = glfwGetKey(mainWindow, GLFW_KEY_F) > 0 ? 1 : 0;

	emulator.keys[0xa] = glfwGetKey(mainWindow, GLFW_KEY_Z) > 0 ? 1 : 0;
	emulator.keys[0x0] = glfwGetKey(mainWindow, GLFW_KEY_X) > 0 ? 1 : 0;
	emulator.keys[0xb] = glfwGetKey(mainWindow, GLFW_KEY_C) > 0 ? 1 : 0;
	emulator.keys[0xf] = glfwGetKey(mainWindow, GLFW_KEY_V) > 0 ? 1 : 0;

	if (emulator.waitForInput) {
		for (int32_t i = 0; i < emulator.keys.size(); i++) {
			if (emulator.keys[i]) {
				emulator.SetAwaitedKey(i);
				return;
			}
		}
	}
}