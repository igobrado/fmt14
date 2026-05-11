#include <atomic>
#include <cstring>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <fmt/fmt.hpp>

struct my_complex_type
{
    int         type_one;
    std::string type_two;
    float       type_three;
};

namespace fmt {

template<>
struct formatter<my_complex_type>
{
    constexpr fixed_format_string operator()(my_complex_type const& a_value)
    {
        return fixed_format_string{
            "type_one: ",
            a_value.type_one,
            ", type_two: ",
            a_value.type_two,
            ", type_three: ",
            a_value.type_three};
    }
};

}  // namespace fmt

static std::uint64_t k_format_counter = 0;
static std::uint64_t k_stream_counter = 0;
static std::uint64_t k_printf_counter = 0;

static std::atomic_bool k_should_stop{false};

static inline void format_tests(std::vector<my_complex_type> const& a_data_array)
{
    while (!k_should_stop)
    {
        for (auto const& item : a_data_array)
        {
            std::puts(fmt::format("{}", item));
            ++k_format_counter;
        }
    }

    std::cout << "Number of format calls: " << k_format_counter << std::endl;
}

static inline void stream_tests(std::vector<my_complex_type> const& a_data_array)
{
    while (!k_should_stop)
    {
        for (auto const& item : a_data_array)
        {
            std::cout << "type_one: " << item.type_one << ", type_two: " << item.type_two << ", type_three"
                      << item.type_three << std::endl;
            ++k_stream_counter;
        }
    }

    std::cout << "Number of stream calls: " << k_stream_counter << std::endl;
}

static inline void printf_tests(std::vector<my_complex_type> const& a_data_array)
{
    while (!k_should_stop)
    {
        for (auto const& item : a_data_array)
        {
            std::printf(
                "type_one: %d, type_two: %s, type_three: %.5f\n",
                item.type_one,
                item.type_two.c_str(),
                item.type_three);
            ++k_printf_counter;
        }
    }

    std::cout << "Number of printf calls: " << k_printf_counter << std::endl;
}

std::vector<my_complex_type> init_random_data(std::size_t count)
{
    std::vector<my_complex_type> data_array;
    static const char            charset[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::mt19937                          rng{std::random_device{}()};
    std::uniform_int_distribution<int>    int_dist(0, 9999);
    std::uniform_int_distribution<int>    char_len_dist(5, 12);
    std::uniform_int_distribution<int>    char_dist(0, sizeof(charset) - 2);
    std::uniform_real_distribution<float> float_dist(0.0f, 100.0f);  // random float range

    data_array.resize(count);
    for (auto& item : data_array)
    {
        item.type_one   = int_dist(rng);    // integer
        item.type_three = float_dist(rng);  // new float field

        item.type_two.clear();
        item.type_two.reserve(20);
        for (int i = 0; i < 20; ++i)
            item.type_two.push_back(charset[char_dist(rng)]);
    }

    return data_array;
}

int main(int argc, char** argv)
{
    std::thread thr{};
    auto        data_array = init_random_data(30);

    if (argc > 1 && strcmp(argv[1], "fmt") == 0)
    {
        thr = std::thread{[&]() { format_tests(data_array); }};
    }
    else if (strcmp(argv[1], "stream") == 0)
    {
        thr = std::thread{[&]() { stream_tests(data_array); }};
    }
    else if (strcmp(argv[1], "printf") == 0)
    {
        thr = std::thread{[&]() { printf_tests(data_array); }};
    }
    else
    {
        std::puts("Wrong input");
        return -1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(100));
    k_should_stop.store(true);
    if (thr.joinable())
    {
        thr.join();
    }
}
