#ifndef WILDLAND_FIRESIM_SIMULATION_H
#define WILDLAND_FIRESIM_SIMULATION_H

#include <iostream>
#include <string>
#include "utility.h"
#include "globals.h"
#include "WFS_landscape.h"
#include "WFS_fireweather.h"
#include "fire.h"
#include "WFS_output.h"

namespace wildland_firesim{
/*!
 * \brief The Simulation class
 * contains function to simulate fire within a landscape either with static weather conditions or a
 * weather simulation.
 */
class Simulation
{
public:
    Simulation();

    Month stringToMonth(std::string m);

    /*!
     * \brief runSimulation
     * runs the simulation and logs output data.
     * \param landscape
     * \param weather
     * \param output
     */
    void runSimulation(WFS_Landscape *landscape, const FireWeatherVariables &weather,
                       FireWeather weathersim, Output *output);

    int numberOfRuns;
    int maxFireDuration;
    bool simulateFireWeather;
    bool importLandscape;
    bool igniteCentralVertex;
    Month whichMonth;

    int timestepLength;
    float timeScalingFactor;

    std::string nameOfMeteorologicalParameterFile;
    std::string nameOfLandscapeParameterFile;

    Fire fire;
};

}  // namespace wildland_firesim
#endif // WILDLAND_FIRESIM_SIMULATION_H
