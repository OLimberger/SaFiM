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

/*!
 * \brief random
 * returns a pseudo-random float in [0..1)
 * \return
 */
float random();

/*!
 * \brief random
 * returns a pseudo-random float in [0..top)
 * \param top
 * \return
 */
int random(int top);

/*!
 * \brief random
 * returns a pseudo-random float in [min..max)
 * \param min
 * \param max
 * \return
 */
float random(float min, float max);

/*!
 * \brief weibull_random
 * returns a weibull-distributed pseudo-random float
 * \param scale
 * \param form
 * \return
 */
float weibull_random(float scale, float form);

/*!
 * \brief normal_random
 * returns a normal-distributed pseudo-random float
 * \param mean
 * \param sd
 * \return
 */
float normal_random(float mean, float sd);

//functions to cast strings to numeric values
float asFloat(const std::string &str);
int asInteger(const std::string &str);

}  // namespace utility
}  // namespace wildland_firesim

#endif // WILDLAND_FIRESIM_UTILITY_H
