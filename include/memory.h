#pragma once

#include <array>

namespace emu
{
    typedef std::array<unsigned char, 4096> memory_alloc;

    /*
     * Memory Map:
     * +---------------+= 0xFFF (4095) End of Chip-8 RAM
     * |               |
     * |               |
     * |               |
     * |               |
     * |               |
     * | 0x200 to 0xFFF|
     * |     Chip-8    |
     * | Program / Data|
     * |     Space     |
     * |               |
     * |               |
     * |               |
     * +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
     * |               |
     * |               |
     * |               |
     * +---------------+= 0x200 (512) Start of most Chip-8 programs
     * | 0x000 to 0x1FF|
     * | Reserved for  |
     * |  interpreter  |
     * +---------------+= 0x000 (0) Start of Chip-8 RAM
     *
     */
    class memory
    {
    public:
        memory()
            : mem {}
        {
        }

        memory_alloc &get_memory() { return mem; }

    private:
        memory_alloc mem;
    };
}
