//
// Created by Administrator on 2019/5/17.
//

#ifndef DATA_STRUCTURE_FOR_LOVE_BENCHMARK_H
#define DATA_STRUCTURE_FOR_LOVE_BENCHMARK_H

#include <vector>
#include <chrono>
#include <fstream>
#include <utility>
#include <static_random_helper.hpp>

namespace benchmark {
    struct BenchMark;
    static std::vector<BenchMark *> benchmarks{};

    struct BenchMark {

        data_structure::utils::RandomIntGen<size_t> rand{};

        struct Result {
            std::string name;
            using outcome = std::pair<size_t, long long>;
            std::vector<outcome> outcomes;

            auto output() {
                std::ofstream fout(name + ".json");
                fout << "{\n"
                        "    \"name\" : \"" << name << "\", " << std::endl <<
                     "    \"results\" : [\n";
                for (auto i : outcomes) {
                    if (i != outcomes.back())
                        fout << "        " << "{\"" << i.first << "\" : " << i.second << " }, " << std::endl;
                    else
                        fout << "        " << "{\"" << i.first << "\" : " << i.second << " } " << std::endl;
                }
                fout << "]}" << std::endl;
            }
        };

        auto time_now() noexcept {
            return std::chrono::steady_clock::now();
        }

        auto gen_random_int(std::vector<int> &v, size_t size) {
            while (size--) v.push_back(rand());
        }

        auto gen_ordered_int(std::vector<int> &v, size_t size) {
            while (size) v.push_back(size--);
        }

        virtual Result run() = 0;

        BenchMark() {
            benchmarks.push_back(this);
        };
    };

    void run_all() {
        for (auto i : benchmarks) {
            i->run().output();
        }
    }
}
#endif //DATA_STRUCTURE_FOR_LOVE_BENCHMARK_H
