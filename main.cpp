#include <clocale>
#include <cstdio>
#include <cstdlib>

#include "simulation.h"
#include "WFS_landscape.h"
#include "WFS_output.h"
#include "fire.h"
#include "WFS_fireweather.h"

/*! \mainpage South African Savanna Fire Model
 * Fire is an important driver of vegetation dynamics in savanna ecosystems, yet often strongly
 * simplified in ecological models. This project aims to provide an integrative module as well as
 * a stand-alone appliction.
 * Fire behavior in SASaFiM emerges from landscape parameters and weather conditions. The model
 * landscape consists of grid cells with a specified vegetation type and certain amounts of live
 * and dead biomass. SASaFiM calculates a one-dimensional rate of spread using an empirical model.
 * It then extends it into a two-dimensional spread using a rasterization of Huygens wavelet
 * propagation principle. Overall, the model is capable to simulate the emergence, spread and
 * resulting burn patterns across spatial scales in a realistic manner taking into account
 * seasonality as a source of landscape heterogeneity.
 */


using namespace ::wildland_firesim;

#define VERSION_MAJOR 0
#define VERSION_MINOR 1

static const char *progname = nullptr;

static const char *landscapeFile = nullptr;
static const char *month = nullptr;
static const char *weatherFile = nullptr;

static const char *fixedWeatherFile = "fixed_fireweather.txt";

static int timestepLength = 15;
static int maximalFireDuration = 5400;
static int numberOfRuns = 1;

// import landscape option - uses files in folder
static bool importLandscape = false;
static bool simulateFireWeather = false;
static bool centeredIgnitionPoint = false;

[[noreturn]] static void
usage()
{
    fprintf(stderr, "usage: %s OPTIONS\n", progname);
    fprintf(stderr, "Wildland Fire Simulator\n\nOptions:\n");
    fprintf(stderr, "\t-h\tDisplays this help.\n");
    fprintf(stderr, "\t-v\tDisplays version information.\n");
    fprintf(stderr, "\t-l <landscape-file>\tCSV file containing parameters for landscape.\n");
    fprintf(stderr, "\t-a\tImport landscape from file.\n");
    fprintf(stderr, "\t-s\tSimulate fire weather.\n");
    fprintf(stderr, "\t-w <weather-file>\tThe CSV file containing the meteorological parameters.\n");
    fprintf(stderr, "\t-m <month>\tThe month for which the fire weather is simulated.\n");
    fprintf(stderr, "\t-b <fixed-weather-file-name>\tFile name for parameter list.\n");
    fprintf(stderr, "\t-t <timestep-length>\tLength of timesteps.\n");
    fprintf(stderr, "\t-d <maximal-fire-duration>\tMaximal duration of the fire simulation.\n");
    fprintf(stderr, "\t-r <number-of-runs>\tNumber of model runs.\n");
    fprintf(stderr, "\t-c\tCentered ignition point.\n");

    exit(1);
}

static void
parseArguments(int argc, char *argv[])
{
    progname = argv[0];

    while (argc > 1 && argv[1][0] == '-') {
        switch (argv[1][1]) {
        case 'h':
        default:
            usage();
        case 'v':
            printf("WildlandFireSimulator %d.%d\n", VERSION_MAJOR, VERSION_MINOR);
            exit(0);
        case 'l':
            landscapeFile = argv[2];
            argc--;
            argv++;
            break;
        case 'a':
            importLandscape = true;
            break;
        case 's':
            simulateFireWeather = true;
            break;
        case 'w':
            weatherFile = argv[2];
            argc--;
            argv++;
            break;
        case 'm':
            month = argv[2];
            argc--;
            argv++;
            break;
        case 'b':
            fixedWeatherFile = argv[2];
            argc--;
            argv++;
            break;
        case 't':
            timestepLength = atoi(argv[2]);
            argc--;
            argv++;
            break;
        case 'd':
            maximalFireDuration = atoi(argv[2]);
            argc--;
            argv++;
            break;
        case 'r':
            numberOfRuns = atoi(argv[2]);
            argc--;
            argv++;
            break;
        case 'c':
            centeredIgnitionPoint = true;
            break;
        }
        argc--;
        argv++;
    }
}

int main(int argc, char *argv[] )
{
    parseArguments(argc, argv);

    // We either need a landscape file (ASCII grid, option -a) or a parameter file to generate a
    // landscape (option -l)
    if (!importLandscape && !landscapeFile) {
        fprintf(stderr, "error: either an ASCII grid or a landscape parameter file must be supplied\n");
        exit(1);
    }
    if (importLandscape && landscapeFile) {
        fprintf(stderr, "error: an ASCII grid is imported, using a parameter file is invalid\n");
        exit(1);
    }
    if (simulateFireWeather) {
        if (!weatherFile) {
            fprintf(stderr, "error: name of meteorological parameter file has to be specified\n");
            exit(1);
        }
        if (!month) {
            fprintf(stderr, "error: month for fire weather simulation has to be specified\n");
            exit(1);
        }
    }

    if (landscapeFile != nullptr)
        printf("landscapeFile=%s\n", landscapeFile);
    if (month != nullptr)
        printf("month=%s\n", month);
    if (weatherFile != nullptr)
        printf("weatherFile=%s\n", weatherFile);
    printf("fixedWeatherFile=%s\n", fixedWeatherFile);

    //create instance of simulation class
    Simulation fireSimulation;
    //initialize fire weather
    FireWeather weatherSimulation;
    //initialize output
    Output output;

    //setup simulation
    //general parameters
    fireSimulation.timestepLength = timestepLength;
    fireSimulation.maxFireDuration = maximalFireDuration;
    fireSimulation.numberOfRuns = numberOfRuns;

    //import landscape option - uses files in folder
    fireSimulation.importLandscape = importLandscape;

    //enquire name of file for landscape creation
    if(!importLandscape){
        fireSimulation.nameOfLandscapeParameterFile = landscapeFile;
    }

    //specification of ignition location
    fireSimulation.igniteCentralVertex = centeredIgnitionPoint;

    //specification fire weather simulation
    fireSimulation.simulateFireWeather = simulateFireWeather;
    auto weather = FireWeatherVariables{};

    if (simulateFireWeather) {
        weatherSimulation.importMeteorologicalParameter(weatherFile);
        fireSimulation.whichMonth = fireSimulation.stringToMonth(month);
    } else {
       weatherSimulation.getFixedFireWeatherParameter(fixedWeatherFile, &weather);
    }

    //start simulation(s) and data log
    for (int i = 0; i < fireSimulation.numberOfRuns; i++) {
        // create model landscape
        WFS_Landscape modelLandscape;
        if (fireSimulation.importLandscape){
            modelLandscape.importLandscapeFromFile();
        } else {
            modelLandscape.generateLandscapeFromFile(fireSimulation.nameOfLandscapeParameterFile);
        }
        //vegetation data before burn
        //output.writeVegetationMapToASCII(modelLandscape, output.setfileName("vegetation_map", ".asc", i));
        //output.writeVegetationDataToCSV(modelLandscape, output.setfileName("vegetation_data", ".csv", i));

        //fire simulation
        fireSimulation.runSimulation(&modelLandscape, weather, weatherSimulation, &output);

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
