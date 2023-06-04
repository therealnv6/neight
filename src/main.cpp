#include <cpu.hpp>

int main()
{
	low::cpu cpu;
	cpu.load_rom("roms/pong.rom");

	// todo: add renderer, probably use opengl+glfw so we can also use imgui for debugging data.
	while (true)
	{
		cpu.fetch_opcode();
		cpu.execute_opcode();

		// update timers
		cpu.tick_timer();

		usleep(10000);
	}
}
