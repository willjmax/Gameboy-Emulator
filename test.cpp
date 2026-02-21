#include <iostream>

#include "gameboy.h"

int main() {

    GameBoy gb;

    gb.loadROM("/home/will/gameboy/gb-test-roms/cpu_instrs/cpu_instrs.gb");

    Bus bus = gb.getBus();

    std::cout << std::hex << (int)bus.read(0x0000) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0001) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0002) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0003) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0004) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0005) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0006) << std::endl;


    std::cout << std::hex << (int)bus.read(0x0100) << std::endl;
    std::cout << std::hex << (int)bus.read(0x0101) << std::endl;

    return 0;
}
