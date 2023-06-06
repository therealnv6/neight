#include <chrono>
#include <cpu.hpp>

int main()
{
	low::cpu cpu;
	cpu.load_rom("roms/particles.rom");

	// todo: add renderer, probably use opengl+glfw so we can also use imgui for debugging data.
	while (true)
	{
		cpu.fetch_opcode();
		cpu.execute_opcode();

		// update timers
		cpu.tick_timer();

		std::cout << "\033[2J\033[1;1H";

		for (int y = 0; y < 32; y++)
		{
			for (int x = 0; x < 64; x++)
			{
				if (cpu.get_pixel_at(x, y))
				{
					std::cout << "*";
				}
				else
				{
					std::cout << " ";
				}
			}
			std::cout << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3));
	}
}
