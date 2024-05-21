#include "Chip8.h"

Chip8::Chip8(const Chip8Config& config)
	: config(config) {
	Reset();
}

void Chip8::Reset() {
	PC = 0x200;
	SP = 0;
	I = 0;
	rDelay = 0;
	rSound = 0;
	running = true;
	waitForInput = false;
	for (int32_t i = 0; i < regs.size(); i++) {
		regs[i] = 0;
	}
	for (int32_t i = 0; i < rom.size(); i++) {
		rom[i] = 0;
	}
	for (int32_t i = 0; i < stack.size(); i++) {
		stack[i] = 0;
	}
	for (int32_t i = 0; i < digitsPattern.size(); i++) {
		rom[i] = digitsPattern[i];
	}
	for (int32_t i = 0; i < romFile.size(); i++) {
		rom[0x200 + i] = romFile[i];
	}
	screenTexture.Reset();
}

bool Chip8::LoadROM(uint8_t* data, int32_t size) {
	if (size > romFile.size()) {
		std::cout << "Rom file is too big\n";
		return false;
	}
	for (int32_t i = 0; i < size; i++) {
		romFile[i] = data[i];
	}
	for (int32_t i = size; i < romFile.size(); i++) {
		romFile[i] = 0;
	}
	romLoaded = true;
	Reset();
	return true;
}

bool Chip8::IsReady() {
	return true;
}

void Chip8::Run(uint32_t instructions) {
	for (uint32_t i = 0; i < instructions; i++) {
		Step();
	}
}

void Chip8::Step() {
	if (!running || waitForInput) {
		return;
	}
	opcode = Read16(PC);
	PC += 2;
	switch (opcode >> 12) {
	case 0x0:
		if (opcode == 0x00e0) {
			CLS();
		}
		else if (opcode == 0x00ee) {
			RET();
		}
		else {
			JMP_M();
		}
		break;
	case 0x1:
		JMP_NNN();
		break;
	case 0x2:
		CALL_NNN();
		break;
	case 0x3:
		SE_VX_NN();
		break;
	case 0x4:
		SNE_VX_NN();
		break;
	case 0x5:
		if ((opcode & 0xf) == 0) {
			SE_VX_VY();
		}
		else {
			Unhandled();
		}
		break;
	case 0x6:
		LD_VX_NN();
		break;
	case 0x7:
		ADD_VX_NN();
		break;
	case 0x8:
		switch (opcode & 0xf) {
		case 0x0:
			LD_VX_VY();
			break;
		case 0x1:
			OR_VX_VY();
			break;
		case 0x2:
			AND_VX_VY();
			break;
		case 0x3:
			XOR_VX_VY();
			break;
		case 0x4:
			ADD_VX_VY();
			break;
		case 0x5:
			SUB_VX_VY();
			break;
		case 0x6:
			SHR_VX();
			break;
		case 0x7:
			SUBN_VX_VY();
			break;
		case 0xe:
			SHL_VX();
			break;
		default:
			Unhandled();
			break;
		}
		break;
	case 0x9:
		if ((opcode & 0xf) == 0) {
			SNE_VX_VY();
		}
		else {
			Unhandled();
		}
		break;
	case 0xa:
		LD_I_NNN();
		break;
	case 0xb:
		JMP_V0_NNN();
		break;
	case 0xc:
		RND_VX_NN();
		break;
	case 0xd:
		DRW_VX_VY_N();
		break;
	case 0xe:
		switch (opcode & 0xff) {
		case 0x9e:
			SKP_VX();
			break;
		case 0xa1:
			SKNP_VX();
			break;
		default:
			Unhandled();
			break;
		}
	case 0xf:
		switch (opcode & 0xff) {
		case 0x07:
			LD_VX_DT();
			break;
		case 0x0a:
			LD_VX_K();
			break;
		case 0x15:
			LD_DT_VX();
			break;
		case 0x18:
			LD_ST_VX();
			break;
		case 0x1e:
			ADD_I_VX();
			break;
		case 0x29:
			LD_F_VX();
			break;
		case 0x33:
			LD_B_VX();
			break;
		case 0x55:
			LD_I_VX();
			break;
		case 0x65:
			LD_VX_I();
			break;
		default:
			Unhandled();
			break;
		}
		break;
	default:
		Unhandled();
		break;
	}

	clock += 10;
	if ((clock % 100) == 0) {
		if (rDelay > 0) {
			rDelay--;
		}
		if (rSound > 0) {
			rSound--;
		}
	}
}

bool Chip8::GetKey(uint8_t index) {
	return keys[index & 0xf];
}

void Chip8::SetAwaitedKey(uint8_t index) {
	regs[(opcode >> 8) & 0xf] = index & 0xf;
	waitForInput = false;
}

void Chip8::UpdatePixel(uint8_t x, uint8_t y, uint8_t b) {
	if (!b) return;
	if (screenTexture.GetPixel(x, y) == 0x00) {
		screenTexture.SetPixel(x, y, 0xff);
	}
	else {
		screenTexture.SetPixel(x, y, 0x00);
		regs[0xf] = 1;
	}
}

void Chip8::Write8(uint16_t address, uint8_t value) {
	rom[address & 0xfff] = value;
}

uint8_t Chip8::Read8(uint16_t address) {
	return rom[address & 0xfff];
}

uint16_t Chip8::Read16(uint16_t address) {
	return (uint16_t)(rom[(address + 1) & 0xfff]) | (uint16_t)(rom[(address) & 0xfff] << 8);
}

uint16_t Chip8::Pop() {
	SP -= 2;
	uint16_t value = (uint16_t)stack[SP + 1] | ((uint16_t)stack[SP] << 8);
	return value;
}

void Chip8::Push(uint16_t value) {
	stack[SP] = value >> 8;
	stack[(SP + 1) % 256] = value & 0xff;
	SP += 2;
}

void Chip8::Unhandled() {}

void Chip8::JMP_M() {
	if (config.enableCallMachineRoutine) {
		PC = opcode & 0xffe;
	}
	else {
		Unhandled();
	}
}

void Chip8::CLS() {
	screenTexture.Reset();
}

void Chip8::RET() {
	PC = Pop();
}

void Chip8::JMP_NNN() {
	PC = opcode & 0xfff;
}

void Chip8::CALL_NNN() {
	Push(PC);
	PC = opcode & 0xfff;
}

void Chip8::SE_VX_NN() {
	if (regs[(opcode >> 8) & 0xf] == (opcode & 0xff)) {
		PC += 2;
	}
}

void Chip8::SNE_VX_NN() {
	if (regs[(opcode >> 8) & 0xf] != (opcode & 0xff)) {
		PC += 2;
	}
}

void Chip8::SE_VX_VY() {
	if (regs[(opcode >> 8) & 0xf] == regs[(opcode >> 4) & 0xf]) {
		PC += 2;
	}
}

void Chip8::LD_VX_NN() {
	regs[(opcode >> 8) & 0xf] = opcode & 0xff;
}

void Chip8::ADD_VX_NN() {
	regs[(opcode >> 8) & 0xf] += opcode & 0xff;
}

void Chip8::LD_VX_VY() {
	regs[(opcode >> 8) & 0xf] = regs[(opcode >> 4) & 0xf];
}

void Chip8::OR_VX_VY() {
	regs[(opcode >> 8) & 0xf] |= regs[(opcode >> 4) & 0xf];
}

void Chip8::AND_VX_VY() {
	regs[(opcode >> 8) & 0xf] &= regs[(opcode >> 4) & 0xf];
}

void Chip8::XOR_VX_VY() {
	regs[(opcode >> 8) & 0xf] ^= regs[(opcode >> 4) & 0xf];
}

void Chip8::ADD_VX_VY() {
	uint16_t temp = (uint16_t)regs[(opcode >> 8) & 0xf] + (uint16_t)regs[(opcode >> 4) & 0xf];
	regs[(opcode >> 8) & 0xf] = temp & 0xff;
	regs[0xf] = temp > 0xff;
}

void Chip8::SUB_VX_VY() {
	uint8_t temp = regs[(opcode >> 8) & 0xf] >= regs[(opcode >> 4) & 0xf];
	regs[(opcode >> 8) & 0xf] -= regs[(opcode >> 4) & 0xf];
	regs[0xf] = temp;
}

void Chip8::SHR_VX() {
	uint8_t temp = regs[(opcode >> 8) & 0xf] & 0b1;
	regs[(opcode >> 8) & 0xf] = regs[(opcode >> 8) & 0xf] >> 1;
	regs[0xf] = temp;
}

void Chip8::SUBN_VX_VY() {
	uint8_t temp = regs[(opcode >> 8) & 0xf] <= regs[(opcode >> 4) & 0xf];
	regs[(opcode >> 8) & 0xf] = regs[(opcode >> 4) & 0xf] - regs[(opcode >> 8) & 0xf];
	regs[0xf] = temp;
}

void Chip8::SHL_VX() {
	uint8_t temp = regs[(opcode >> 8) & 0xf] >> 7;
	regs[(opcode >> 8) & 0xf] = regs[(opcode >> 8) & 0xf] << 1;
	regs[0xf] = temp;
}

void Chip8::SNE_VX_VY() {
	if (regs[(opcode >> 8) & 0xf] != regs[(opcode >> 4) & 0xf]) {
		PC += 2;
	}
}

void Chip8::LD_I_NNN() {
	I = opcode & 0xfff;
}

void Chip8::JMP_V0_NNN() {
	PC = (opcode + regs[0]) & 0xfff;
}

void Chip8::RND_VX_NN() {
	regs[(opcode >> 8) & 0xf] = rand() & (opcode & 0xff);
}

void Chip8::DRW_VX_VY_N() {
	uint8_t x = regs[(opcode >> 8) & 0xf];
	uint8_t y = regs[(opcode >> 4) & 0xf];
	uint8_t n = opcode & 0xf;
	regs[0xf] = 0;
	for (int32_t i = 0; i < n; i++) {
		uint8_t row = Read8(I + i);
		for (int32_t j = 0; j < 8; j++) {
			UpdatePixel((x + j) % screenWidth, (y + i) % screenHeight, (row >> (7 - j)) & 0b1);
		}
	}
}

void Chip8::SKP_VX() {
	if (GetKey(regs[(opcode >> 8) & 0xf])) {
		PC += 2;
	}
}

void Chip8::SKNP_VX() {
	if (!GetKey(regs[(opcode >> 8) & 0xf])) {
		PC += 2;
	}
}

void Chip8::LD_VX_DT() {
	regs[(opcode >> 8) & 0xf] = rDelay;
}

void Chip8::LD_VX_K() {
	waitForInput = true;
}

void Chip8::LD_DT_VX() {
	rDelay = regs[(opcode >> 8) & 0xf];
}

void Chip8::LD_ST_VX() {
	rSound = regs[(opcode >> 8) & 0xf];
}

void Chip8::ADD_I_VX() {
	I += regs[(opcode >> 8) & 0xf];
}

void Chip8::LD_F_VX() {
	I = ((opcode >> 8) & 0xf) * 5;
}

void Chip8::LD_B_VX() {
	uint8_t temp = regs[(opcode >> 8) & 0xf];
	Write8(I + 2, temp % 10);
	temp /= 10;
	Write8(I + 1, temp % 10);
	temp /= 10;
	Write8(I, temp % 10);
}

void Chip8::LD_I_VX() {
	for (int32_t i = 0; i <= ((opcode >> 8) & 0xf); i++) {
		Write8(I + i, regs[i]);
	}
}

void Chip8::LD_VX_I() {
	for (int32_t i = 0; i <= ((opcode >> 8) & 0xf); i++) {
		regs[i] = Read8(I + i);
	}
}