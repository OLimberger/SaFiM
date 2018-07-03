#ifndef WILDLAND_FIRESIM_GLOBALS_H
#define WILDLAND_FIRESIM_GLOBALS_H

namespace wildland_firesim {

constexpr float Pi = 3.14159265358979323846264338327950288419716939937510f;

enum Month {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December,
};

constexpr int NumberOfMonths = 12;

enum class VegetationType {
    Grass,
    NonFlammable
};

enum class CellState {
    Unburned,
    Burning,
    BurnedOut
};

}  // namespace wildland_firesim

#endif // WILDLAND_FIRESIM_GLOBALS_H
