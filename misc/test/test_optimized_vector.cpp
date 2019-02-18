#define MEMORY_LEAK_TEST
#include <optimized_vector.hpp>
#include <cassert>
#include <vector>
#include <iostream>
#include <random>
#include <limits>
#include <chrono>
std::mt19937_64 eng {};
std::uniform_int_distribution<int> dist(0, std::numeric_limits<int>::max());

#define get_rand() dist(eng)

inline auto random_string() {
    std::string test{};
    auto len = get_rand() % 233;
    while(len--) {
        test.push_back(static_cast<char>(get_rand() % 128));
    }
    return test;
}
int main(){
    std::clog << "- tests for optimized_vector [WITH MEMORY LEAK TEST]" << std::endl;

    auto _time = std::chrono::steady_clock::now().time_since_epoch().count();
    eng.seed(static_cast<unsigned>(_time));

    std::clog << "\n-- testing simple data" << std::endl;
    std::vector<int> std_vec;
    data_structure::optimized_vector<int> test;
    std::clog << "--- simple data push_back";
    for(int i = 0; i < 10'000; ++i) {
        std_vec.push_back(get_rand());
        test.push_back(std_vec.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_vec[t] == test[t]);
        }
    }
    for(std::size_t i = 0; i < std_vec.size(); ++i) {
        assert(std_vec[i] == test[i]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- simple data emplace_back";
    for (int i = 0; i < 10'000; ++i) {
        std_vec.push_back(get_rand());
        test.emplace_back(std_vec.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_vec[t] == test[t]);
        }
    }
    for (std::size_t i = 0; i < std_vec.size(); ++i) {
        assert(std_vec[i] == test[i]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- simple data pop_back";
    for (int i = 0; i < 20'000; ++i) {
        std_vec.pop_back();
        test.pop_back();
        if(i % 100 == 0) {
            for(std::size_t j = 0; j < std_vec.size(); ++j) {
                assert(std_vec[j] == test[j]);
            }
        }
    }
    std::clog << " [SUCCESS]\n";

    for(int i = 0; i < 10'000; ++i) {
        std_vec.push_back(get_rand());
        test.push_back(std_vec.back());
    }
    std::clog << "--- simple data erase";
    for(int i = 0; i < 10'000; ++i) {
        auto t = get_rand() % std_vec.size();
        std_vec.erase(std_vec.begin() + t);
        test.erase(t);
        if(i % 100 == 0) {
            for(std::size_t j = 0; j < std_vec.size(); ++j) {
                assert(std_vec[j] == test[j]);
            }
        }
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "\n-- testing more on local memory" << std::endl;
    std::vector<int> std_vec_2;
    data_structure::optimized_vector<int, 500> test_2;

    std::clog << "--- local memory push_back";
    for (int i = 0; i < 250; ++i) {
        std_vec_2.push_back(get_rand());
        test_2.push_back(std_vec_2.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_vec_2[t] == test_2[t]);
        }
    }
    for(std::size_t j = 0; j < std_vec_2.size(); ++j) {
        assert(std_vec_2[j] == test_2[j]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- local memory emplace_back";
    for (int i = 0; i < 250; ++i) {
        std_vec_2.push_back(get_rand());
        test_2.emplace_back(std_vec_2.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_vec_2[t] == test_2[t]);
        }
    }
    for (std::size_t j = 0; j < std_vec_2.size(); ++j) {
        assert(std_vec_2[j] == test_2[j]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- local memory pop_back";
    for(int i = 0; i < 500; ++i) {
        std_vec_2.pop_back();
        test_2.pop_back();
        if(i % 10 == 0) {
            for(std::size_t j = 0; j < std_vec.size(); ++j) {
                assert(std_vec_2[j] == test_2[j]);
            }
        }
    }
    std::clog << " [SUCCESS]\n";

    for(int i = 0; i < 500; ++i) {
        std_vec_2.push_back(get_rand());
        test_2.push_back(std_vec_2.back());
    }
    std::clog << "--- local memory erase";
    for(int i = 0; i < 500; ++i) {
        auto t = get_rand() % std_vec_2.size();
        std_vec_2.erase(std_vec_2.begin() + t);
        test_2.erase(t);
        if(i % 10 == 0) {
            for(std::size_t j = 0; j < std_vec_2.size(); ++j) {
                assert(std_vec_2[j] == test_2[j]);
            }
        }
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "\n-- testing complex data" << std::endl;
    std::vector<std::string> std_complex_vec;
    data_structure::optimized_vector<std::string, 55> complex_test;
    int complex_test_size = 5'000;

    std::clog << "--- complex data push_back";
    for(int i = 0; i < complex_test_size; ++i) {
        std_complex_vec.push_back(random_string());
        complex_test.push_back(std_complex_vec.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_complex_vec[t] == complex_test[t]);
        }
    }
    for(std::size_t i = 0; i < std_vec.size(); ++i) {
        assert(std_complex_vec[i] == complex_test[i]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- complex data emplace_back";
    for (int i = 0; i < complex_test_size; ++i) {
        std_complex_vec.push_back(random_string());
        complex_test.emplace_back(std_complex_vec.back());
        if (i != 0) {
            auto t = get_rand() % i;
            assert(std_complex_vec[t] == complex_test[t]);
        }
    }
    for (std::size_t i = 0; i < std_vec.size(); ++i) {
        assert(std_complex_vec[i] == complex_test[i]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- complex data pop_back";
    for (int i = 0; i < complex_test_size * 2; ++i) {
        std_complex_vec.pop_back();
        complex_test.pop_back();
        if(i % 10 == 0) {
            for(std::size_t j = 0; j < std_vec.size(); ++j) {
                assert(complex_test[j] == std_complex_vec[j]);
            }
        }

    }
    assert(complex_test.size() == 0);
    std::clog << " [SUCCESS]\n";

    for(int i = 0; i < complex_test_size; ++i) {
        std_complex_vec.push_back(random_string());
        complex_test.push_back(std_complex_vec.back());
    }

    std::clog << "--- complex data erase";
    for(int i = 0; i < complex_test_size; ++i) {
        auto t = get_rand() % std_complex_vec.size();
        std_complex_vec.erase(std_complex_vec.begin() + t);
        complex_test.erase(t);
        if(i % 10 == 0) {
            for(std::size_t j = 0; j < std_complex_vec.size(); ++j) {
                assert(std_complex_vec[j] == complex_test[j]);
            }
        }
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "\n-- testing constructors\n";
    std::clog << "--- copy constructor";
    data_structure::optimized_vector<std::string> copy_constructor_test1;
    for(int i = 0; i < 2333; ++i) copy_constructor_test1.push_back(random_string());
    auto another1(copy_constructor_test1);
    for(int i = 0; i < 2333; ++i) {
        assert(another1[i] == copy_constructor_test1[i]);
    }
    data_structure::optimized_vector<int> copy_constructor_test2;
    for(int i = 0; i < 2333; ++i) copy_constructor_test2.push_back(get_rand());
    auto another2(copy_constructor_test2);
    for(int i = 0; i < 2333; ++i) {
        assert(another2[i] == copy_constructor_test2[i]);
    }
    std::clog << " [SUCCESS]\n";

    std::clog << "--- move constructor";
    std::vector<std::string> sample;
    data_structure::optimized_vector<std::string, 500> move_constructor_test1;
    for(int i = 0; i < 500; ++i) {
        sample.push_back(random_string());
        move_constructor_test1.push_back(sample.back());
    }
    auto temp(std::move(move_constructor_test1));
    for(int i = 0; i < 500; ++i) {
        assert(temp[i] == sample[i]);
    }
    for(int i = 0; i < 500; ++i) {
        sample.push_back(random_string());
        temp.push_back(sample.back());
    }
    auto temp2(std::move(temp));
    for(int i = 0; i < 500; ++i) {
        assert(temp2[i] == sample[i]);
    }

    std::clog << " [SUCCESS]\n";
    return 0;
}

#undef MEMORY_LEAK_TEST