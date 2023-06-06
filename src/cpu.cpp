#include <cpu.hpp>

namespace low
{
	void cpu::op_00E0()
	{
		std::fill_n(gfx, sizeof(gfx), 0);
	}

	void cpu::op_00EE()
	{
		--sp;
		pc = stack[sp];
	}

	void cpu::op_1nnn()
	{
		uint16_t address = opcode & 0x0FFF;
		pc = address;
	}

	void cpu::op_2nnn()
	{
		uint16_t address = opcode & 0x0FFF;

		stack[sp] = pc;
		++sp;
		pc = address;
	}

	void cpu::op_3xkk()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t kk = opcode & 0x00FF;
		if (V[x] == kk)
			pc += 2;
	}

	void cpu::op_4xkk()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t kk = opcode & 0x00FF;
		if (V[x] != kk)
			pc += 2;
	}

	void cpu::op_5xy0()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		if (V[x] == V[y])
			pc += 2;
	}

	void cpu::op_6xkk()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t kk = opcode & 0x00FF;
		V[x] = kk;
	}

	void cpu::op_7xkk()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t kk = opcode & 0x00FF;
		V[x] += kk;
	}

	void cpu::op_8xy0()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		V[x] = V[y];
	}

	void cpu::op_8xy1()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		V[x] |= V[y];
	}

	void cpu::op_8xy2()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		V[x] &= V[y];
	}

	void cpu::op_8xy3()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		V[x] ^= V[y];
	}

	void cpu::op_8xy4()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;

		if (V[y] > (0xFF - V[x]))
			V[0xF] = 1;
		else
			V[0xF] = 0;

		V[x] += V[y];
	}

	void cpu::op_8xy5()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;

		if (V[y] > V[x])
			V[0xF] = 0;
		else
			V[0xF] = 1;

		V[x] -= V[y];
	}

	void cpu::op_8xy6()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		V[0xF] = V[x] & 0x1;
		V[x] >>= 1;
	}

	void cpu::op_8xy7()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;

		if (V[x] > V[y])
			V[0xF] = 0;
		else
			V[0xF] = 1;

		V[x] = V[y] - V[x];
	}

	void cpu::op_8xyE()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		V[0xF] = V[x] >> 7;
		V[x] <<= 1;
	}

	void cpu::op_9xy0()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		if (V[x] != V[y])
			pc += 2;
	}

	void cpu::op_Annn()
	{
		uint16_t address = opcode & 0x0FFF;
		index = address;
	}

	void cpu::op_Bnnn()
	{
		uint16_t address = opcode & 0x0FFF;
		pc = address + V[0];
	}

	void cpu::op_Cxkk()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t kk = opcode & 0x00FF;
		V[x] = std::rand() & kk;
	}

	void cpu::op_Dxyn()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t y = (opcode & 0x00F0) >> 4;
		uint8_t height = opcode & 0x000F;

		uint8_t xPos = V[x] % 64;
		uint8_t yPos = V[y] % 32;

		V[0xF] = 0;

		for (unsigned int row = 0; row < height; row++)
		{
			uint8_t spriteByte = memory[index + row];
			for (unsigned int col = 0; col < 8; col++)
			{
				if ((spriteByte & (0x80 >> col)) != 0)
				{
					unsigned int pixelIndex = xPos + col + ((yPos + row) * 64);

					if (gfx[pixelIndex] == 1)
					{
						V[0xF] = 1;
					}

					gfx[pixelIndex] ^= 1;
				}
			}
		}
	}

	void cpu::op_Ex9E()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		if (key[V[x]] != 0)
			pc += 2;
	}

	void cpu::op_ExA1()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		if (key[V[x]] == 0)
			pc += 2;
	}

	void cpu::op_Fx07()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		V[x] = delay_timer;
	}

	void cpu::op_Fx0A()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		bool keyPressed = false;

		for (uint8_t i = 0; i < 16; i++)
		{
			if (key[i] != 0)
			{
				V[x] = i;
				keyPressed = true;
			}
		}

		if (!keyPressed)
		{
			// todo: uncomment this, we don't have input yet
			// pc -= 2;
		}
	}

	void cpu::op_Fx15()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		delay_timer = V[x];
	}

	void cpu::op_Fx18()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		sound_timer = V[x];
	}

	void cpu::op_Fx1E()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		index += V[x];
	}

	void cpu::op_Fx29()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t digit = V[x];
		index = digit * 5;
	}

	void cpu::op_Fx33()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		uint8_t value = V[x];

		memory[index] = value / 100;
		memory[index + 1] = (value / 10) % 10;
		memory[index + 2] = value % 10;
	}

	void cpu::op_Fx55()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		for (uint8_t i = 0; i <= x; i++)
		{
			memory[index + i] = V[i];
		}
	}

	void cpu::op_Fx65()
	{
		uint8_t x = (opcode & 0x0F00) >> 8;
		for (uint8_t i = 0; i <= x; i++)
		{
			V[i] = memory[index + i];
		}
	}
}
