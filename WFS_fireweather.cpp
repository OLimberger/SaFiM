#include "WFS_fireweather.h"

namespace wildland_firesim {

FireWeather::FireWeather()
{

}
/*!
 * \brief FireWeather::getFixedFireWeatherParameter
 * \param fileName
 */
void
FireWeather::getFixedFireWeatherParameter(const std::string fileName, FireWeatherVariables *weather){
    auto reader = csv::Reader{',', '#'};
    auto parameters = reader.parse(fileName);

    size_t NumberOfMeteorologicalParameters = 4;

    // the csv should have one line per parameter
    if (parameters.size() != NumberOfMeteorologicalParameters) {
        std::cerr << "invalid configuration: there must be at least " << NumberOfMeteorologicalParameters
                  << " lines in the CSV data\n";
        std::exit(1);
    }
    weather->temperature = utility::asFloat(parameters[0][0]);
    weather->relHumidity = utility::asFloat(parameters[1][0]);
    weather->windSpeed = utility::asFloat(parameters[2][0]);
    weather->windDirection = utility::asInteger(parameters[3][0]);
}

/*!<
 * \brief Simulation::importMeteorologicalParameter
 * Function to import the meteorological parameters required for the calculation of the fire weather
 * from an external file and adding them to the landscape.
 * \param fileName
 * \param landscape
 */
void
FireWeather::importMeteorologicalParameter(const std::string fileName)
{
    auto reader = csv::Reader{',', '#'};
    auto parameters = reader.parse(fileName);

    size_t NumberOfMeteorologicalParameters = 20;

    // the csv should have one line per parameter
    if (parameters.size() != NumberOfMeteorologicalParameters) {

        std::cerr << "invalid configuration: there must be at least " << NumberOfMeteorologicalParameters
                  << " lines in the CSV data\n";
        std::exit(1);

    }

    for (const auto &line : parameters) {
        if (line.size() != NumberOfMonths) {
            std::cerr << "invalid configuration: there must be an entry per month on each line\n";
            std::exit(1);
        }
    }

    //application of parameters to landscape
    //TODO: remove unneccessary wind condition parameters
    for(size_t month = 0; month < NumberOfMonths; ++month) {
        m_relativeHumidityParameter[month].param1 = utility::asFloat(parameters[0][month].c_str());
        m_relativeHumidityParameter[month].param2 = utility::asFloat(parameters[1][month].c_str());
        m_minimumDailyTemperatureParameter[month].param1 = utility::asFloat(parameters[2][month].c_str());
        m_minimumDailyTemperatureParameter[month].param2 = utility::asFloat(parameters[3][month].c_str());
        m_maximumDailyTemperatureParameter[month].param1 = utility::asFloat(parameters[4][month].c_str());
        m_maximumDailyTemperatureParameter[month].param2 = utility::asFloat(parameters[5][month].c_str());
        m_windSpeedParameter[month].param1 = utility::asFloat(parameters[6][month].c_str());
        m_windSpeedParameter[month].param2 = utility::asFloat(parameters[7][month].c_str());
        m_windDirectionProbability[month][0] = utility::asFloat(parameters[8][month].c_str());
        m_windDirectionProbability[month][1] = utility::asFloat(parameters[9][month].c_str());
        m_windDirectionProbability[month][2] = utility::asFloat(parameters[10][month].c_str());
        m_windDirectionProbability[month][3] = utility::asFloat(parameters[11][month].c_str());
        m_windDirectionProbability[month][4] = utility::asFloat(parameters[12][month].c_str());
        m_windDirectionProbability[month][5] = utility::asFloat(parameters[13][month].c_str());
        m_windDirectionProbability[month][6] = utility::asFloat(parameters[14][month].c_str());
        m_windDirectionProbability[month][7] = utility::asFloat(parameters[15][month].c_str());
        m_windConditionChange[month].stayCalm = utility::asFloat(parameters[16][month].c_str());
        m_windConditionChange[month].windyToCalm = utility::asFloat(parameters[17][month].c_str());
        m_sunriseTime[month] = utility::asFloat(parameters[18][month].c_str());
        m_sunsetTime[month] = utility::asFloat(parameters[19][month].c_str());;
    }
}

void
FireWeather::setStartingTime(int startOfFire){
    startingTime = startOfFire;
}

void
FireWeather::calculateFireWeather(int month,int durationOfBurn)
{
    //WILDLAND_ASSERT(m_initialized, "landscape is not initialized");

    //temperature calculations
    //get time (t)
    int t = startingTime + durationOfBurn;
    t = t%24;
    // calculate temperature
    // chose additional time variables (struct with one value per month?)
    // hn: sunrise, ho: sunset, hx: time of max temperature, hp=hn+24: sunset at next day
    float hn = m_sunriseTime[month];
    float ho = m_sunsetTime[month];
    float hx = ho - 4; // assumed to be 4 hours before sunset
    float hp = hn;

    // chose minimal temperature, maximal temperature and minimum temperature of following day
    // from distribution. at start of the fire and every 24 h
    if((t == 0) || durationOfBurn == 0){
        Tn = utility::normal_random(m_minimumDailyTemperatureParameter[month].param1,
                                    m_minimumDailyTemperatureParameter[month].param2);
        Tx = utility::normal_random(m_maximumDailyTemperatureParameter[month].param1,
                                    m_maximumDailyTemperatureParameter[month].param2);
        Tp = utility::normal_random(m_minimumDailyTemperatureParameter[month].param1,
                                    m_minimumDailyTemperatureParameter[month].param2);
    }
    //temperature at sunset, c=0.39, empirical estimate from californian weather data
    float c = 0.39f;
    float To = Tx - c * (Tx - Tp);
    // estimate hourly temperature by mininmal and maximal temperatures
    float alpha = Tx-Tn;
    float P = Tx-To;
    float b=(Tp-To)/(std::sqrt(std::abs(hp-ho)));
    //TODO: check range of value
    if(t > hn && t <= hx){
        temperature = Tn+alpha*(((t-hn)/(hx-hn))*(Pi/2));
    }
    if(t > hx && t < ho){
        temperature = To + P * std::sin((Pi/2)+((t-hx)/4)*(Pi/2));
    }
    if(t >= ho || t <= hp){
        temperature = To + b * std::sqrt(std::abs(t-ho));
    }

    //derive relative humidity from distribution
    relHumidity = utility::random(m_relativeHumidityParameter[month].param1,
                                  m_relativeHumidityParameter[month].param2);

    //determine wind conditions
    windDirection = 0;
    // if it is not calm and staying calm, or wind does not cease, let there be wind.
    if(windyConditions == false){
        if(utility::random()<m_windConditionChange[month].stayCalm){
            windyConditions = false;
            windSpeed = 0.0;
        } else
        {
            windyConditions = true;
            //determine wind direction
            while(windDirection == 0){
                for(int k = 0; k<WindDirectionsCount ;++k) {
                    if (utility::random() < m_windDirectionProbability[month][k]) {
                        windDirection = k;
                    }
                }
            }
            //derive wind speed from distribution
            windSpeed = utility::weibull_random(m_windSpeedParameter[month].param1,
                                                m_windSpeedParameter[month].param2);
        }
    } else {
        if(utility::random()<m_windConditionChange[month].windyToCalm){
            windyConditions = false;
            windSpeed = 0.0;
        }
        else
        {
            windyConditions = true;
            //determine wind direction --> undefined behavior if wd is not set.
            while(windDirection == 0){
                for(int k = 0; k<WindDirectionsCount ;++k) {
                    if (utility::random() < m_windDirectionProbability[month][k]) {
                        windDirection = k;
                    }
                }
            }
            //derive wind speed from distribution
            windSpeed = utility::weibull_random(m_windSpeedParameter[month].param1,
                                                m_windSpeedParameter[month].param2);
        }
    }
}

}//namespace wildland_firesim
