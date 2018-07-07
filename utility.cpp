#include "utility.h"
#include <iostream>
#include <random>

namespace wildland_firesim {
namespace utility {

class RandomNumberGenerator {

public:
    static RandomNumberGenerator *getInstance()
    {
        if(!s_instance){
            s_instance = new RandomNumberGenerator;
            std::atexit([]() { delete s_instance; });
        }
        return s_instance;
    }

    float random()
    {
        return m_distribution(m_engine);
    }

    int random(int top)
    {
        auto distribution = std::uniform_int_distribution<int>{0, top};
        return distribution(m_engine);
    }

    float random(float min, float max)
    {
        auto distribution = std::uniform_real_distribution<float>{min, max};
        return distribution(m_engine);
    }

    float normal_random(float mean, float sd)
    {
        auto distribution = std::normal_distribution<float>{mean, sd};
        return distribution(m_engine);
    }

    float weibull_random(float shape, float form)
    {
        auto distribution = std::weibull_distribution<float>{shape, form};
        return distribution(m_engine);
    }

private:
    int fixedSeed = 42; //fixed seed for sensitivity analysis
    static RandomNumberGenerator *s_instance;
    //RandomNumberGenerator() : m_engine{rd()}, m_distribution{0.f, 1.f} {}
    RandomNumberGenerator() : m_engine{fixedSeed}, m_distribution{0.f, 1.f} {}
    std::random_device rd;
    std::mt19937 m_engine;
    std::uniform_real_distribution<float> m_distribution;
    std::normal_distribution<float> m_normalDistribution;
    std::weibull_distribution<float> m_weibullDistribution;
};

RandomNumberGenerator *RandomNumberGenerator::s_instance;

void _assert(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << message << std::endl;
        std::terminate();
    }
}

float random()
{
    return RandomNumberGenerator::getInstance()->random();
}

int random(int top)
{
    return RandomNumberGenerator::getInstance()->random(top);
}
float random(float min, float max)
{
    return RandomNumberGenerator::getInstance()->random(min, max);
}

float normal_random(float mean, float sd)
{
    return RandomNumberGenerator::getInstance()->normal_random(mean, sd);
}

float weibull_random(float scale, float form)
{
    return RandomNumberGenerator::getInstance()->weibull_random(scale, form);
}

float asFloat(const std::string &str)
{
    return static_cast<float>(std::atof(str.c_str()));
}

int asInteger(const std::string &str)
{
   return (std::atoi(str.c_str()));
}

}  // namespace utility
}  // namespace wildland_firesim
