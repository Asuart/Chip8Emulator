#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Chip8.h"
#include "ScreenPlane.h"
#include "TextureUploader.h"

class EmulatorWindow {
public:
	GLFWwindow* mainWindow = nullptr;

	EmulatorWindow(uint32_t width, uint32_t height);
	~EmulatorWindow();

	bool LoadROM(const std::string& romPath);
	void Start();
	void HandleResolutionChange(uint32_t width, uint32_t height);

private:
	ScreenPlane* screenPlane = nullptr;
	TextureUploader<uint8_t>* screenUploader = nullptr;
	uint32_t width;
	uint32_t height;
	Chip8 emulator;

	void UpdateKeyStates();
};

