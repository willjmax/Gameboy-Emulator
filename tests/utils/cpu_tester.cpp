#include <nlohmann/json.hpp>
#include "cpu_tester.h"

using json = nlohmann::json;

void CPUTester::setup_sm83(json initial) {
    cpu.pc = initial.at("pc").get<uint16_t>();
    cpu.sp = initial.at("sp").get<uint16_t>();

    cpu.a = initial.at("a").get<uint8_t>();
    cpu.b = initial.at("b").get<uint8_t>();
    cpu.c = initial.at("c").get<uint8_t>();
    cpu.d = initial.at("d").get<uint8_t>();
    cpu.e = initial.at("e").get<uint8_t>();
    cpu.f = initial.at("f").get<uint8_t>();
    cpu.h = initial.at("h").get<uint8_t>();
    cpu.l = initial.at("l").get<uint8_t>();

    cpu.ime = initial.at("ime").get<int>() != 0;
    cpu.ei = initial.value("ei", 0) != 0;

    for (const auto& ram : initial["ram"]) {
        uint16_t loc = ram[0];
        uint8_t data = ram[1];
        cpu.memory.write(loc, data);
    }
}
