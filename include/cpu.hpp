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

constexpr int SIZE_X = 64;
constexpr int SIZE_Y = 32;

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
		uint8_t gfx[SIZE_X * SIZE_Y]; // (64x32 pixels)
		uint8_t delay_timer;
		uint8_t sound_timer;
		uint16_t stack[16];
		uint16_t sp;
		uint8_t key[16];

		// std::map<uint16_t, callback> opcodes;
		callback opcodes[16][16] = {
			{&cpu::op_00E0,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x0...
			{&cpu::op_1nnn,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x1...
			{&cpu::op_2nnn,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x2...
			{&cpu::op_3xkk,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x3...
			{&cpu::op_4xkk,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x4...
			{&cpu::op_5xy0,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x5...
			{&cpu::op_6xkk,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x6...
			{&cpu::op_7xkk,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x7...
			{&cpu::op_8xy0, &cpu::op_8xy1, &cpu::op_8xy2, &cpu::op_8xy3, &cpu::op_8xy4, &cpu::op_8xy5, &cpu::op_8xy6, &cpu::op_8xy7, &cpu::op_8xyE,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x8...
			{&cpu::op_9xy0,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0x9...
			{&cpu::op_Annn,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0xA...
			{&cpu::op_Bnnn,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0xB...
			{&cpu::op_Cxkk,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0xC...
			{&cpu::op_Dxyn,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0xD...
			{&cpu::op_Ex9E,		 nullptr, &cpu::op_ExA1,		 nullptr,		  nullptr,	   nullptr,		nullptr,		 nullptr,		  nullptr,	   nullptr,		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}, // Opcode 0xE...
			{&cpu::op_Fx07,		 nullptr, &cpu::op_Fx0A,		 nullptr, &cpu::op_Fx15, &cpu::op_Fx18, &cpu::op_Fx1E, &cpu::op_Fx29, &cpu::op_Fx33, &cpu::op_Fx55, &cpu::op_Fx65, nullptr, nullptr, nullptr, nullptr, nullptr}	 // Opcode 0xF...
		};

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
		}

		void execute_opcode()
		{

			// Get the MSB and LSB of the opcode
			uint8_t msb = (opcode >> 12) & 0xF;
			uint8_t lsb = (opcode >> 8) & 0xF;

			// Print the values of msb and lsb
			if (msb != 0 || lsb != 0)
			{
#if DEBUG
				std::cout << "opcode: " << std::hex << opcode
						  << " msb: " << static_cast<int>(msb)
						  << " lsb: " << static_cast<int>(lsb)
						  << std::endl;
#endif
			}

			auto exec = opcodes[msb][lsb];

			// Execute the opcode
			if (exec != nullptr)
			{
				(this->*exec)();
			}

			pc += 2;
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

		uint8_t get_pixel_at(int x, int y)
		{
			return gfx[y * SIZE_X + x];
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
