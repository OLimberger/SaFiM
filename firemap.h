#ifndef FIREMAP_H
#define FIREMAP_H

#include "globals.h"
#include "cell.h"
#include "fire.h"

namespace wildland_firesim {

/*!
 * \brief The FireMap class
 * Class containing function to define the output of the model.
 */
class FireMap
{
public:
    FireMap();
    /*!
     * \brief writeBurnMapToASCII
     * Function to write cell states into an ascii-grid.
     * \param landscape
     * \param fileName
     */
    void writeBurnMapToASCII(Landscape &landscape, std::string fileName);
    /*!
     * \brief writeVegetationMapToASCII
     * Function to write vegetation types into an ascii-grid.
     * \param landscape
     * \param fileName
     */
    void writeVegetationMapToASCII(Landscape &landscape, std::string fileName);
    /*!
     * \brief writeVegetationDataToCSV
     * Function to write vegetation parameters into a csv-Table
     * \param landscape
     * \param fileName
     */
    void writeVegetationDataToCSV(Landscape &landscape, std::string fileName);
    /*!
     * \brief writeFireWeatherDataToCSV
     * Function to write weather data into a csv-Table
     * \param landscape
     * \param fileName
     */
    void writeFireWeatherDataToCSV(std::vector<std::string> weatherData , std::string fileName);

    /*!
     * \brief writeBurnDataToCSV
     * \param landscape
     * \param fileName
     */
    void writeBurnDataToCSV(Landscape &landscape, Fire &fire, std::string fileName);
    /*!
     * \brief setfileName
     * Function to generate file names.
     * \param baseName
     * \param extention
     * \param i
     * \return
     */
    std::string setfileName(std::string baseName, std::string extention, int i);
    /*!
     * \brief storeWeatherData
     * Function to extract weather data at a certain time.
     * \param weather
     * \param durationOfBurn
     * \return
     */
    std::string storeWeatherData(FireWeather &weather, float durationOfBurn);

    std::vector<std::string> weatherData;
};

} //namespace wildland_firesim
#endif // FIREMAP_H
