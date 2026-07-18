#pragma once
#include <string>

#include "bus.h"
#include "cpu.h"
#include "interrupt.h"
#include "timer.h"
#include "display.h"

class GameBoy {
    public:
        GameBoy() :
            mbc1(),
            interrupt(),
            joypad(interrupt),
            timer(interrupt),
            apu(),
            ppu(interrupt),
            bus(timer, interrupt, apu, ppu, joypad, mbc1),
            cpu(bus, timer, interrupt),
            display(),
            running(false) {};

        void loadROM(std::string path);
        void run();
        
    private:
        MBC1 mbc1;
        Interrupt interrupt;
        Joypad joypad;
        Timer timer;
        APU apu;
        PPU ppu;
        Bus bus;
        CPU cpu;
        Display display;

        bool running;

        void handle_joypad_input(SDL_Keycode key, bool pressed);
};
