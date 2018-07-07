#ifndef WILDLAND_FIRESIM_UTILITY_H
#define WILDLAND_FIRESIM_UTILITY_H

#ifndef _NDEBUG
#define WILDLAND_ASSERT(c, m) wildland_firesim::utility::_assert((c), (m))
#else
#define WILDLAND_ASSERT(c, m)
#endif

#include <string>
#include <cstdlib>

namespace wildland_firesim {
namespace utility {

void _assert(bool condition, const char *message);

// Returns a pseudo-random float in [0..1)
float random();

// Returns a pseudo-random float in [0..top)
int random(int top);

// Returns a pseudo-random float in [min..max)
float random(float min, float max);

// Returns a weibull-distributed pseudo-random float
float weibull_random(float scale, float form);

//Returns a normal-distributed pseudo-random float
float normal_random(float mean, float sd);

//functions to cast strings to numeric values
float asFloat(const std::string &str);
int asInteger(const std::string &str);

}  // namespace utility
}  // namespace wildland_firesim

#endif // WILDLAND_FIRESIM_UTILITY_H
