#include "WFS_output.h"
#include <sstream>
#include <fstream>

namespace wildland_firesim {

Output::Output(): weatherData()
{

}

std::string
Output::setfileName(std::string baseName, std::string extention, int i)
{
    std::stringstream ss;
    ss << baseName << i << extention;
    return ss.str();
}


//functions for printing map to ASCII grid
void
Output::writeBurnMapToASCII(LandscapeInterface &landscape, std::string fileName)
{
    // create variable for output file stream
    std::ofstream burnDataFile;
    //open output file stream
    //burnDataFile.open("output/"+fileName);
    burnDataFile.open(fileName);

    burnDataFile << "NCOLS " << landscape.getWidth() << std::endl;
    burnDataFile << "NROWS " << landscape.getHeight() << std::endl;
    burnDataFile << "XLLCORNER " << "0"  << std::endl;
    burnDataFile << "YLLCORNER " << "0"  << std::endl;
    burnDataFile << "CELLSIZE " << "1" << std::endl; //may use cellsize later
    burnDataFile << "NODATA_VALUE " << "-9999" << std::endl;

    for (int y = (landscape.getHeight()-1); y >= 0; y--)
        for (int x = 0; x <landscape.getWidth(); x++) {
            burnDataFile << static_cast<int>(landscape.getCellInformation(x,y)->state) << " ";
        }
    burnDataFile << "\n";
    burnDataFile.close();
}

void
Output::writeVegetationMapToASCII(LandscapeInterface &landscape, std::string fileName)
{
    std::ofstream vegetationTypeDataFile;
    //vegetationTypeDataFile.open("output/"+fileName);
    vegetationTypeDataFile.open(fileName);
    vegetationTypeDataFile << "nCols " << landscape.getWidth() << std::endl;
    vegetationTypeDataFile << "nRows " << landscape.getHeight() << std::endl;
    vegetationTypeDataFile << "xllcorner " << "0"  << std::endl;
    vegetationTypeDataFile << "yllcorner " << "0"  << std::endl;
    vegetationTypeDataFile << "cellsize " << 1  << std::endl; //may use cellsize later
    vegetationTypeDataFile << "nodata_value " << "-9999" << std::endl;

    for (int y = (landscape.getHeight()-1); y >= 0; y--)
        for (int x = 0; x <landscape.getWidth(); x++) {
            vegetationTypeDataFile << static_cast<int>(landscape.getCellInformation(x,y)->type) << " ";
        }
    vegetationTypeDataFile << "\n";
    vegetationTypeDataFile.close();
}


//functions to write csv-files
void
Output::writeVegetationDataToCSV(LandscapeInterface &landscape, std::string fileName)
{
    std::ofstream vegetationDataFile;
    //vegetationDataFile.open("output/"+fileName);
    vegetationDataFile.open(fileName);
    //header
    vegetationDataFile << "x,y,type,live,dead" << "\n";
    //data
    for (int y = (landscape.getHeight()-1); y >= 0; y--)
        for (int x = 0; x <landscape.getWidth(); x++) {
            vegetationDataFile << x << "," << y << ",";
            VegetationType type = landscape.getCellInformation(x,y)->type;
            float dead_biomass = landscape.getCellInformation(x,y)->deadBiomass;
            float live_biomass = landscape.getCellInformation(x,y)->liveBiomass;
            vegetationDataFile << static_cast<int>(type) << ",";
            vegetationDataFile << live_biomass << ",";
            vegetationDataFile << dead_biomass;
            vegetationDataFile << "\n";
        }
    vegetationDataFile.close();
}

void
Output::writeBurnDataToCSV(LandscapeInterface &landscape, Fire &fire, std::string fileName)
{
    std::ofstream burnDataFile;
    //burnDataFile.open("output/"+fileName);
    burnDataFile.open(fileName);
    //header
    burnDataFile << "x,y,state,intensity" << "\n";
    //data
    for (int y = (landscape.getHeight()-1); y >= 0; y--)
        for (int x = 0; x <landscape.getWidth(); x++) {
            burnDataFile << x << "," << y << ",";
            CellState state = landscape.getCellInformation(x,y)->state;
            burnDataFile << static_cast<int>(state) << ",";

            if(state==CellState::Burning){
                for(size_t i = 0; i < fire.burningCellInformationVector.size(); i++){
                    if(y == fire.burningCellInformationVector[i].yCoord &&
                            x == fire.burningCellInformationVector[i].xCoord){
                        burnDataFile << fire.burningCellInformationVector[i].meanFirelineIntensity;
                    }
                }
            }else{
                burnDataFile << "0.0";
            }
            burnDataFile << "\n";
        }
    burnDataFile.close();
}

void
Output::writeFireWeatherDataToCSV(std::vector<std::string> weatherData , std::string fileName)
{
    std::ofstream weatherDataFile;
    //weatherDataFile.open("output/"+fileName);
    weatherDataFile.open(fileName);
    weatherDataFile << "t, temperature, relHumidity, windSpeed, windDirection" << std::endl;
    for(size_t i = 0; i<weatherData.size(); i++){
        weatherDataFile << weatherData[i] << "\n";
    }
    weatherDataFile.close();
}

//functions to store data in strings
std::string
Output::storeWeatherData(const FireWeatherVariables &weather, float durationOfBurn)
{
    std::stringstream weatherData;
    weatherData << durationOfBurn << ",";
    weatherData << weather.temperature << "," << weather.relHumidity << ",";
    weatherData << weather.windSpeed << "," << weather.windDirection;
    return weatherData.str();
}

}//namespace wildland_firesim
