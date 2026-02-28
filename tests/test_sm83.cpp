#include <filesystem>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <nlohmann/json.hpp>

#include "utils/cpu_tester.h"
#include "utils/helpers.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

auto get_test_files(const std::string& path) {
    std::vector<std::string> files;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".json") {
            files.push_back(entry.path().filename().string());
        }
    }

    std::sort(files.begin(), files.end());
    return files;
}

TEST_CASE("SM83", "[cpu][sm83]") {
    CPUTester tester;

    static auto test_files = get_test_files("tests/sm83/v1");
    auto filename = GENERATE(from_range(test_files));

    SECTION("File: " + filename) {
        std::ifstream f(std::string("tests/sm83/v1/") + filename);
        json tests = json::parse(f);

        // | std::views::take(1)
        for (auto& test_case : tests | std::views::take(15)) {
            DYNAMIC_SECTION("Test: " << test_case["name"]) {

                INFO(test_case["name"]);

                json final = test_case["final"];
                tester.setup_sm83(test_case["initial"]);

                tester.step();

                REQUIRE(tester.get_pc() == final["pc"]);
                REQUIRE(tester.get_sp() == final["sp"]);

                CHECK(as_hex(tester.get_a()) == as_hex(final["a"]));
                CHECK(as_hex(tester.get_b()) == as_hex(final["b"]));
                CHECK(as_hex(tester.get_c()) == as_hex(final["c"]));
                CHECK(as_hex(tester.get_d()) == as_hex(final["d"]));
                CHECK(as_hex(tester.get_e()) == as_hex(final["e"]));
                CHECK(as_hex(tester.get_f()) == as_hex(final["f"]));
                CHECK(as_hex(tester.get_h()) == as_hex(final["h"]));
                CHECK(as_hex(tester.get_l()) == as_hex(final["l"]));
                CHECK(as_hex(tester.get_ime()) == as_hex(final["ime"]));

                for (const auto& entry : final["ram"]) {
                    uint16_t loc = entry[0];
                    uint8_t data = entry[1];

                    REQUIRE(as_hex(tester.get_memory(loc)) == as_hex(data));
                }
            }
        }
    }
}
