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
        }

        void handle_opcode(unsigned short opcode)
        {
            unsigned char high_byte = (opcode >> 8) & 0xFF;
            unsigned char low_byte = opcode & 0xFF;

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

        void opcode_0x00e0(uint16_t);
        void opcode_0x00ee(uint16_t);
    };
}