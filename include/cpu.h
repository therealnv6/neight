#pragma once
#include "memory.h"
#include <spdlog/spdlog.h>

namespace low
{
    class cpu
    {
        cpu()
        {
            for (int i = 0; i < 0x10; i++)
            {
                for (int j = 0; j < 0x10; j++)
                {
                    opcode_table[i][j] = &cpu::unknown_opcode;
                }
            }

            // map known opcodes to their corresponding handlers
            opcode_table[0x0][0x0] = &cpu::opcode_0x00e0;
            opcode_table[0x0][0xE] = &cpu::opcode_0x00ee;
            opcode_table[0x1][0x1] = &cpu::opcode_0x1nnn;
            opcode_table[0x2][0x2] = &cpu::opcode_0x2nnn;
        }

        void handle_opcode(unsigned short opcode)
        {
            uint8_t high_byte = (opcode >> 8) & 0xFF;
            uint8_t low_byte = opcode & 0xFF;

            auto opcode_handler = opcode_table[high_byte >> 4][low_byte >> 4];
            (this->*opcode_handler)(opcode);
        }

    private:
        using opcode_handler_t = void (cpu::*)(unsigned short);

        std::array<std::array<opcode_handler_t, 0x10>, 0x10> opcode_table;
        low::memory memory;

        void unknown_opcode(uint16_t opcode)
        {
            spdlog::warn("unknown opcode! {}", opcode);
        }

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

        // Clear the display.
        void opcode_0x00e0(uint16_t);

        // Return from a subroutine.
        void opcode_0x00ee(uint16_t);

        // Jump to a specific address.
        void opcode_0x1nnn(uint16_t);

        // Call a subroutine at a specific address.
        void opcode_0x2nnn(uint16_t);

        // Skip the next instruction if a register equals a specific value.
        void opcode_0x3xkk(uint16_t);

        /*
         * The following functions are helper functions that are used by the opcode handlers.
         */

        // Add a value to a register and set the carry flag if necessary.
        void add_with_carry(uint8_t &reg, uint8_t value);

        // Subtract a value from a register and set the borrow flag if necessary.
        void subtract_with_borrow(uint8_t &reg, uint8_t value);

        // XOR a value with a register.
        void bitwise_xor(uint8_t &reg, uint8_t value);

        // Set a register to a specific value.
        void set_register(uint8_t &reg, uint8_t value);

        // Shift a register left by one bit and set the carry flag to the value of the most significant bit.
        void shift_left(uint8_t &reg);

        // Shift a register right by one bit and set the carry flag to the value of the least significant bit.
        void shift_right(uint8_t &reg);

        // Wait for a key to be pressed and store its value in a register.
        void wait_for_keypress(uint8_t &reg);

        // Skip the next instruction if a key is pressed.
        void skip_if_key_pressed(uint8_t reg);

        // Skip the next instruction if a key is not pressed.
        void skip_if_key_not_pressed(uint8_t reg);

        // Draw a sprite at a specific location on the screen.
        void draw_sprite(uint8_t x, uint8_t y, uint8_t height);

        // Update the timer counters.
        void update_timers();
    };
}