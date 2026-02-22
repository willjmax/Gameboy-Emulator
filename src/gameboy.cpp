#include <fstream>
#include <filesystem>

#include "gameboy.h"

GameBoy::GameBoy() : memory(), cpu(memory) {
    running = false;
}

void GameBoy::loadROM(std::string path) {
    std::ifstream file(path, std::ios::binary);
    std::uintmax_t size = std::filesystem::file_size(path);
    memory.loadROM(file, size);
}

void GameBoy::run() {
    running = true;

    while (running) {
        cpu.step();
    }

}
