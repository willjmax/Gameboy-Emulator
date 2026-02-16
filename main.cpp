#include "gameboy.h"

int main(int argc, char* argv[]) {

    GameBoy gb;

    gb.loadROM(argv[1]);

    return 0;
}
