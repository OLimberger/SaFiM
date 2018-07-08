#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <clocale>
#include "simulation.h"
#include "example_landscape.h"
#include "firemap.h"

using namespace ::wildland_firesim;

int main(int argc, char *argv[] )
{
    // Set up Command line parser and Options
    QCoreApplication app{argc, argv};
    std::setlocale(LC_NUMERIC, "C");
    QCoreApplication::setApplicationName("WildlandFireSimulator");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser commandlineParser;

    commandlineParser.setApplicationDescription("Wildland Fire Simulator");

    commandlineParser.addHelpOption();
    commandlineParser.addVersionOption();

    // Add option for the landscape file (-l).
    QCommandLineOption landscapeFileOption("l", "CSV file containing parameters for landscape generation",
                                           "landscape-file");
    commandlineParser.addOption(landscapeFileOption);

    // Add the option if the landscape should be imported from ascii files. It uses (-a).
    QCommandLineOption importLandscapeOption("a", "import landscape from file");
    commandlineParser.addOption(importLandscapeOption);

    // Add the option if the weather should be simulated. It uses (-s).
    QCommandLineOption simulateWeatherOption("s", "Simulate fire weather");
    commandlineParser.addOption(simulateWeatherOption);

    // Add option for the meteorological parameter file (-w).
    QCommandLineOption fireWeatherFileOption("w", "The CSV file containing the meteorological parameters",
                                             "weather-file");
    commandlineParser.addOption(fireWeatherFileOption);

    // Add option to select month for fire weather simulation (-m).
    QCommandLineOption monthForSimulationOption("m", "The month for which the fire weather is simulated",
                                                "month");
    commandlineParser.addOption(monthForSimulationOption);

    // Add options to set fire weather variables (-b).
    QCommandLineOption fixedFireWeatherOption("b", "file name for parameterliste",
                                              "fixed-weather-file-name");
    fixedFireWeatherOption.setDefaultValue("fixed_fireweather.txt");
    commandlineParser.addOption(fixedFireWeatherOption);

    // Add option to set length of timesteps (-t).
    QCommandLineOption timestepLengthOption("t", "length of timesteps",
                                            "timestep-length");
    timestepLengthOption.setDefaultValue("15");
    commandlineParser.addOption(timestepLengthOption);

    // Add option to set maximal fire duration (-d).
    QCommandLineOption maxFireDurationOption("d", "maximal duration of the fire simulation",
                                             "maximal-fire-duration");
    maxFireDurationOption.setDefaultValue("5400");
    commandlineParser.addOption(maxFireDurationOption);

    // Add option to set number of model runs (-r).
    QCommandLineOption numberOfRunsOption("r", "number of model runs",
                                          "number-of-runs");
    numberOfRunsOption.setDefaultValue("1");
    commandlineParser.addOption(numberOfRunsOption);

    // Add option to set ignition location to center (-c).
    QCommandLineOption centeredIgnition("c", "centered ignition point");
    commandlineParser.addOption(centeredIgnition);

    //process user options
    commandlineParser.process(app);

    //create instance of simulation class
    Simulation fireSimulation;
    //initialize fire weather
    FireWeather weather;
    //initialize output
    FireMap output;
    FireMap * outputPtr = &output;

    //setup simulation
    //general parameters
    fireSimulation.timestepLength = commandlineParser.value(timestepLengthOption).toInt();
    fireSimulation.maxFireDuration = commandlineParser.value(maxFireDurationOption).toInt();
    fireSimulation.numberOfRuns = commandlineParser.value(numberOfRunsOption).toInt();

    //import landscape option - uses files in folder
    fireSimulation.importLandscape = commandlineParser.isSet(importLandscapeOption);
    //enquire name of file for landscape creation
    if(!fireSimulation.importLandscape){
        fireSimulation.nameOfLandscapeParameterFile = commandlineParser.value(landscapeFileOption).toStdString();
        if(!commandlineParser.isSet(landscapeFileOption)){
               fireSimulation.nameOfLandscapeParameterFile = "landscape_s1.txt";
        }
    }
    //specification of ignition location
    fireSimulation.igniteCentralVertex = commandlineParser.isSet(centeredIgnition);

    //specification fire weather simulation
    fireSimulation.simulateFireWeather = commandlineParser.isSet(simulateWeatherOption);

    //if fire weather is simulated, import meteorological parameters
    if(fireSimulation.simulateFireWeather){
        if(commandlineParser.isSet(fireWeatherFileOption)){
            fireSimulation.nameOfMeteorologicalParameterFile = commandlineParser.value(fireWeatherFileOption).toStdString();
            weather.importMeteorologicalParameter(fireSimulation.nameOfMeteorologicalParameterFile);
        } else {
            std::cerr << "name of meteorological parameter file has to be specified.";
            std::exit(1);
        }
        if(commandlineParser.isSet(monthForSimulationOption)){
            fireSimulation.whichMonth = fireSimulation.stringToMonth(
                        commandlineParser.value(monthForSimulationOption).toStdString());
        } else {
            std::cerr << "Month for fire weather simulation has to be specified.";
            std::exit(1);
        }
    } else {
        if(commandlineParser.isSet(fixedFireWeatherOption)){
            std::string nameOfFixedWeatherParameterFile = commandlineParser.value(fixedFireWeatherOption).toStdString();
            weather.getFixedFireWeatherParameter(nameOfFixedWeatherParameterFile);
        } else {
            std::string nameOfFixedWeatherParameterFile = "fixed_fireweather.txt";
        }

    }

    //start simulation(s) and data log
    for(int i = 0; i<fireSimulation.numberOfRuns; i++){
        // create model landscape
        ExampleLandscape modelLandscape;
        if(fireSimulation.importLandscape){
            modelLandscape.importLandscapeFromFile();
        } else {
            modelLandscape.generateLandscapeFromFile(fireSimulation.nameOfLandscapeParameterFile);
        }
        //vegetation data before burn
        //output.writeVegetationMapToASCII(modelLandscape, output.setfileName("vegetation_map", ".asc", i));
        //output.writeVegetationDataToCSV(modelLandscape, output.setfileName("vegetation_data", ".csv", i));

        //fire simulation
        fireSimulation.runSimulation(&modelLandscape, weather, outputPtr);

        //creating simulation output
        output.writeBurnMapToASCII(modelLandscape, output.setfileName("burn_map", ".asc", i ));
        output.writeBurnDataToCSV(modelLandscape, fireSimulation.fire, output.setfileName("burndata", ".csv", i));
        if(fireSimulation.simulateFireWeather){
            output.writeFireWeatherDataToCSV(output.weatherData, output.setfileName("weatherdata", ".csv", i ) );
            output.weatherData.clear();
        }
    }

    return 0;
}
