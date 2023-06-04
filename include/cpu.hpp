#pragma once
#include "memory.h"
#include <spdlog/spdlog.h>

#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>

static const uint8_t FONT_SET[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};

namespace low
{

	class cpu
	{
		typedef void (cpu::*callback)();

	private:
		uint16_t opcode;
		uint8_t memory[4096];
		uint8_t V[16];
		uint16_t index;
		uint16_t pc;
		uint8_t gfx[64 * 32]; // (64x32 pixels)
		uint8_t delay_timer;
		uint8_t sound_timer;
		uint16_t stack[16];
		uint16_t sp;
		uint8_t key[16];

		std::map<uint16_t, callback> opcodes;

	public:
		cpu()
		{
			pc = 0x200;
			opcode = 0;
			index = 0;
			sp = 0;

			std::fill_n(key, sizeof(key), 0);
			std::fill_n(gfx, sizeof(gfx), 0);
			std::fill_n(stack, sizeof(stack), 0);
			std::fill_n(V, sizeof(V), 0);

			for (int i = 0; i < 80; i++)
			{
				memory[i] = FONT_SET[i];
			}

			opcodes[0x0000] = &low::cpu::op_00E0;
			opcodes[0x000E] = &low::cpu::op_00EE;
			opcodes[0x1000] = &low::cpu::op_1nnn;
			opcodes[0x2000] = &low::cpu::op_2nnn;
			opcodes[0x3000] = &low::cpu::op_3xkk;
			opcodes[0x4000] = &low::cpu::op_4xkk;
			opcodes[0x5000] = &low::cpu::op_5xy0;
			opcodes[0x6000] = &low::cpu::op_6xkk;
			opcodes[0x7000] = &low::cpu::op_7xkk;
			opcodes[0x8000] = &low::cpu::op_8xy0;
			opcodes[0x8001] = &low::cpu::op_8xy1;
			opcodes[0x8002] = &low::cpu::op_8xy2;
			opcodes[0x8003] = &low::cpu::op_8xy3;
			opcodes[0x8004] = &low::cpu::op_8xy4;
			opcodes[0x8005] = &low::cpu::op_8xy5;
			opcodes[0x8006] = &low::cpu::op_8xy6;
			opcodes[0x8007] = &low::cpu::op_8xy7;
			opcodes[0x800E] = &low::cpu::op_8xyE;
			opcodes[0x9000] = &low::cpu::op_9xy0;
			opcodes[0xA000] = &low::cpu::op_Annn;
			opcodes[0xB000] = &low::cpu::op_Bnnn;
			opcodes[0xC000] = &low::cpu::op_Cxkk;
			opcodes[0xD000] = &low::cpu::op_Dxyn;
			opcodes[0xE09E] = &low::cpu::op_Ex9E;
			opcodes[0xE0A1] = &low::cpu::op_ExA1;
			opcodes[0xF007] = &low::cpu::op_Fx07;
			opcodes[0xF00A] = &low::cpu::op_Fx0A;
			opcodes[0xF015] = &low::cpu::op_Fx15;
			opcodes[0xF018] = &low::cpu::op_Fx18;
			opcodes[0xF01E] = &low::cpu::op_Fx1E;
			opcodes[0xF029] = &low::cpu::op_Fx29;
			opcodes[0xF033] = &low::cpu::op_Fx33;
			opcodes[0xF055] = &low::cpu::op_Fx55;
			opcodes[0xF065] = &low::cpu::op_Fx65;

			// Seed the random number generator
			std::srand(static_cast<unsigned int>(std::time(nullptr)));
		}

		void load_rom(const char *romPath)
		{
			std::ifstream file(romPath, std::ios::binary | std::ios::ate);

			if (file.is_open())
			{
				std::streampos fileSize = file.tellg();
				char *buffer = new char[fileSize];

				file.seekg(0, std::ios::beg);
				file.read(buffer, fileSize);
				file.close();

				for (int i = 0; i < fileSize; i++)
				{
					memory[i + 512] = buffer[i];
				}

				delete[] buffer;
			}
		}

		void fetch_opcode()
		{
			opcode = memory[pc] << 8 | memory[pc + 1];
			pc += 2;
		}

		void execute_opcode()
		{
			for (const auto &offset : { 0xF000, 0x000F, 0x00FF })
			{
				const uint16_t prefix = opcode & offset;
				const auto found = opcodes.find(prefix);

				if (found != opcodes.end())
				{
					(this->*found->second)();
					return;
				}
			}

			spdlog::warn("unknown opcode! {}", opcode);
		}

		void tick_timer()
		{
			if (delay_timer > 0)
			{
				--delay_timer;
			}

			if (sound_timer > 0)
			{
				if (--sound_timer == 0)
				{
					std::cout << "sound thing" << std::endl;
				}
			}
		}

	private:
		/*
		 * The following functions are opcode handlers, which are called when the corresponding
		 * opcode is encountered. Each opcode handler function is responsible for executing the
		 * operation specified by the opcode.
		 *
		 * For example:
		 * 0x00e0 -> [0x0][0x0]
		 * 0x00ee -> [0x0][0xE]
		 * 0x1nnn -> [0x1][0x1]
		 * ...
		 */
		void op_00E0();
		void op_00EE();
		void op_1nnn();
		void op_2nnn();
		void op_3xkk();
		void op_4xkk();
		void op_5xy0();
		void op_6xkk();
		void op_7xkk();
		void op_8xy0();
		void op_8xy1();
		void op_8xy2();
		void op_8xy3();
		void op_8xy4();
		void op_8xy5();
		void op_8xy6();
		void op_8xy7();
		void op_8xyE();
		void op_9xy0();
		void op_Annn();
		void op_Bnnn();
		void op_Cxkk();
		void op_Dxyn();
		void op_Ex9E();
		void op_ExA1();
		void op_Fx07();
		void op_Fx0A();
		void op_Fx15();
		void op_Fx18();
		void op_Fx1E();
		void op_Fx29();
		void op_Fx33();
		void op_Fx55();
		void op_Fx65();
	};
}
