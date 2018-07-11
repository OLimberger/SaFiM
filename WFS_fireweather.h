#ifndef FIREWEATHER_H
#define FIREWEATHER_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include "csvreader.h"
#include "utility.h"
#include "globals.h"
#include "cell.h"
#include "fire.h"

namespace wildland_firesim {

template<typename T>
struct distrParameter {
    T param1;
    T param2;
};

struct WindCondition {
    float stayCalm;
    float windyToCalm;
};

constexpr int WindDirectionsCount = 8;

/*!
 * \brief The FireWeather class
 * Containing functions and parameter to simulate fire weather.
 */
class FireWeather
{
public:
    FireWeather();
    /*!
     * \brief importMeteorologicalParameter
     * extract meteorological parameters from file.
     * \param fileName
     */
    void importMeteorologicalParameter(const std::string fileName);

    /*!
     * \brief getFixedFireWeatherParameter
     * imports fixed fire weather parameter from file.
     * \param fileName
     */
    void getFixedFireWeatherParameter(const std::string fileName, FireWeatherVariables *weather);

    /*!
     * \brief calculateFireWeather
     * simulates hourly fire weather. Temperature is calculated using a model by
     * Cesaraccio et al. (2001). The parameters are received from temperature distribution, while
     * sunset and sunrise times are received from the parameter file. Relative humidity is chosen
     * from a normal distribution. For the simulation of wind, it has to be determined if wind is
     * blowing. It the conditions are windy, wind direction is set according to a certain probability
     * and the wind speed chosen from a Weilbull distribution.
     * \param month
     * \param durationOfBurn
     */
    void calculateFireWeather(int month, int durationOfBurn);

    void setStartingTime(int startOfFire);

    //Meteorological Variables
    bool windyConditions;
    float windSpeed;
    int windDirection;
    float relHumidity;
    float temperature;

private:
    distrParameter<float> m_minimumDailyTemperatureParameter[NumberOfMonths];
    distrParameter<float> m_maximumDailyTemperatureParameter[NumberOfMonths];
    distrParameter<float> m_relativeHumidityParameter[NumberOfMonths];
    distrParameter<float> m_windSpeedParameter[NumberOfMonths];
    WindCondition m_windConditionChange[NumberOfMonths];
    float m_windDirectionProbability[NumberOfMonths][WindDirectionsCount];
    float m_sunriseTime [NumberOfMonths];
    float m_sunsetTime [NumberOfMonths];
    // Tn: min temperature, Tx: max temperature, Tp: min temperature of following day
    float Tn, Tx, Tp;
    int startingTime; //time for fire weather
};

} //namespace wildland_firesim

#endif // FIREWEATHER_H
