#include <iostream>
#include <iomanip>
#include <sstream>

template<typename T>
std::string as_hex(T val, int width = 2) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << (int)val;
    return ss.str();
}
