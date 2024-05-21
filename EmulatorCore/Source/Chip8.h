#pragma once
#include <array>
#include <string>
#include <iostream>
#include "Texture.h"

struct Chip8Config {
	bool enableCallMachineRoutine = false;
};

class Chip8 {
public:
	static const uint32_t screenWidth = 64;
	static const uint32_t screenHeight = 32;
	Texture<uint8_t> screenTexture = Texture<uint8_t>(screenWidth, screenHeight);
	std::array<uint8_t, 0x10> keys;
	bool waitForInput = false;
	bool running = true;
	bool romLoaded = false;

	Chip8(const Chip8Config& config = Chip8Config());

	void Reset();
	bool LoadROM(uint8_t* data, int32_t size);
	bool IsReady();
	void Run(uint32_t instructions);
	void SetAwaitedKey(uint8_t index);

private:
	std::array<uint8_t, 0xc00> romFile;
	Chip8Config config;
	uint16_t PC, I;
	uint8_t SP;
	uint8_t rDelay, rSound;
	std::array<uint8_t, 0x10> regs;
	std::array<uint8_t, 0x1000> rom;
	std::array<uint8_t, 0x100> stack;

	uint64_t clock;

	uint16_t opcode;

	void Step();
	bool GetKey(uint8_t index);
	void UpdatePixel(uint8_t x, uint8_t y, uint8_t b);

	void Write8(uint16_t address, uint8_t value);
	uint8_t Read8(uint16_t address);
	uint16_t Read16(uint16_t address);
	uint16_t Pop();
	void Push(uint16_t value);

	void Unhandled();
	void JMP_M();
	void CLS();
	void RET();
	void JMP_NNN();
	void CALL_NNN();
	void SE_VX_NN();
	void SNE_VX_NN();
	void SE_VX_VY();
	void LD_VX_NN();
	void ADD_VX_NN();
	void LD_VX_VY();
	void OR_VX_VY();
	void AND_VX_VY();
	void XOR_VX_VY();
	void ADD_VX_VY();
	void SUB_VX_VY();
	void SHR_VX();
	void SUBN_VX_VY();
	void SHL_VX();
	void SNE_VX_VY();
	void LD_I_NNN();
	void JMP_V0_NNN();
	void RND_VX_NN();
	void DRW_VX_VY_N();
	void SKP_VX();
	void SKNP_VX();
	void LD_VX_DT();
	void LD_VX_K();
	void LD_DT_VX();
	void LD_ST_VX();
	void ADD_I_VX();
	void LD_F_VX();
	void LD_B_VX();
	void LD_I_VX();
	void LD_VX_I();

	const std::array<uint8_t, 16 * 5> digitsPattern = {
		0xF0, 0x90, 0x90, 0x90, 0xF0,
		0x20, 0x60, 0x20, 0x20, 0x70,
		0xF0, 0x10, 0xF0, 0x80, 0xF0,
		0xF0, 0x10, 0xF0, 0x10, 0xF0,
		0x90, 0x90, 0xF0, 0x10, 0x10,
		0xF0, 0x80, 0xF0, 0x10, 0xF0,
		0xF0, 0x80, 0xF0, 0x90, 0xF0,
		0xF0, 0x10, 0x20, 0x40, 0x40,
		0xF0, 0x90, 0xF0, 0x90, 0xF0,
		0xF0, 0x90, 0xF0, 0x10, 0xF0,
		0xF0, 0x90, 0xF0, 0x90, 0x90,
		0xE0, 0x90, 0xE0, 0x90, 0xE0,
		0xF0, 0x80, 0x80, 0x80, 0xF0,
		0xE0, 0x90, 0x90, 0x90, 0xE0,
		0xF0, 0x80, 0xF0, 0x80, 0xF0,
		0xF0, 0x80, 0xF0, 0x80, 0x80
	};
};