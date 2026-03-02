#pragma once
#include <string>

#include "bus.h"
#include "cpu.h"
#include "interrupt.h"
#include "timer.h"

class GameBoy {
    public:
        GameBoy() :
            interrupt(),
            timer(interrupt),
            apu(),
            bus(timer, interrupt, apu),
            cpu(bus, timer, interrupt),
            running(false) {};

        void loadROM(std::string path);
        void run();
        
    private:
        Interrupt interrupt;
        Timer timer;
        APU apu;
        Bus bus;
        CPU cpu;

        bool running;
};
