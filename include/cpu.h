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
        }

    private:
        using opcode_handler_t = void (cpu::*)(unsigned short);

        std::array<std::array<opcode_handler_t, 0x10>, 0x10> opcode_table;
        low::memory memory;

        void unknown_opcode(uint16_t opcode)
        {
            spdlog::warn("unknown opcode! {}", opcode);
        }
    };
}