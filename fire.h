#ifndef WILDLAND_FIRESIM_FIRE_H
#define WILDLAND_FIRESIM_FIRE_H

#include <random>
#include <math.h>
#include <vector>
#include "globals.h"
#include "landscape_interface.h"
#include "utility.h"

namespace wildland_firesim {

struct FireWeatherVariables {
    float windSpeed;
    int windDirection;
    float relHumidity;
    float temperature;
};

/*!
 * \brief The Fire class
 * Contains all functions and variables required for the simulation of fire spread within the supplied
 * landscape and at certain weather conditions.
 */
class Fire
{
public:
    Fire();
    /*!
     * \brief spreadFire
     * Function to simulate fire spread for one timestep. The fire spread algorithm iterates over each cell
     * burning, calculating the burn status of a cell using the rate of spread, timestep length and distance
     * to the next vertex. If the fire reaches a new vertex, its coordinates are appended to the point fire
     * source vector. Note that wind direction refers to the direction the wind is blowing to.
     * If all vertices of a cell are reached or the intensity of a fire is zero, the cell is considered burned
     * out and fire effects on the vegetation in the respective are applied.
     * Each ignited point fire source tries to ignite the cells sharing the vertex and calculates its burn
     * status with the remaining time at the particular point fire source.
     * \param landscape
     * \param weather
     * \param timestepLength
     */
    void spreadFire(LandscapeInterface *landscape, const FireWeatherVariables &weather, int timestepLength);
    /*!
     * \brief fire::initiateWildFire
     * Function to set a point ignition source at a random cell vertex within the model landscape. Then the
     * cells of the vegetation type "grass" and a minimum fuel load sharing this vertex are ignited according
     * to the initial ignition probability.
     * \param landscape
     */
    void initiateWildFire(LandscapeInterface *landscape, const FireWeatherVariables &weather);

    void initiatePrescribedBurning();

    /*!
     * \brief setCenteredIgnitionPoint
     * Set ignition point into center of landscape.
     * \param landscape
     * \param weather
     */
    void setCenteredIgnitionPoint(LandscapeInterface *landscape);

    struct burningCellInformation{
        int xCoord;
        int yCoord;
        int uCoordSource;
        int vCoordSource;
        float meanFirelineIntensity;
        std::vector<float> burnStatus;
        std::vector<int> spreadDirection;
    };

    std::vector<burningCellInformation> burningCellInformationVector;

    float distance_to_cell_boundary[3];
    int numberOfCellsBurning;

private:
    int heatYield = 17000;

    struct pointFireSourceInformation{
        int uCoord;
        int vCoord;
        int remainingTime;
        float fireIntensity;
    };

    /*!
     * \brief calculateHeadFireRateOfSpread
     * Calculation of the head fire rate of spread in m/s after a semi-empirical model
     * by Higgins et al., 2008 which was parameterized using an exponential wind function.
     * The heatflux adjusted by a wind factor is devided by the heat of preignition, devided
     * into the fraction of energy required to drive off fuel moisture and the energy required
     * to ignite the grass fuel.
     * \param fuelLoad
     * \param moistureContent
     * \param windSpeed
     * \return head fire rate of spread
    */
    float calculateHeadFireRateOfSpread(const float fuelload, const float moistureContent,
                                        const float relHumidity, const float windspeed);

    /*!
     * \brief calculateDirectionalRateOfSpread
     * The rate of spread of the fire into a certain direction is calculated from the head fire
     * rate of spread using an ellipse template. The length-to-breadth ratio is given by an empirical
     * formula by McArthur in Alexander et al. (1985). The geometrics are hence used to receive the
     * ellipse semiaxis denoted as a and b as well as the linear eccentricity c.
     * which allow for the calculation of the distance between the focal point of the ellipse (point fire
     * source and its rim at a certain angle.
     * The angle is given by the difference between the wind direction (angle = 0) and the direction of fire
     * spread multiplied by the width of the direction classes.
     * \param windSpeed wind speed in m/s
     * \param windDirection wind direction as integer
     * \param headFireRateOfSpread maximum rate of spread in m/s
     * \param directionOfFireSpread
     * \return
     */
    float calculateDirectionalRateOfSpread(const float windSpeed, const int windDirection,
                                           const float headFireRateOfSpread, const int directionOfFireSpread);

    /*!
     * \brief estimateGrassFuelMoisture
     * Estimation of the fuel moisture content of grass fuels using the function of the Mark V
     * firemeter by McArthur as stated in Noble, 1980. The estimate is based on the relative humidity,
     * air temperature and the degree of curing.
     * \param relativeHumidity
     * \param temperature
     * \param curing
     * \return
     */
    float estimateGrassFuelMoisture(const float temperature, const float relativeHumidity,
                                    const float curing);

    /*!
     * \brief estimateAvailableFuelLoad
     * Estimation of the available fuel load by using the combustion factor by Hely (2002) as a substitute.
     * \param grassMoistureContent
     * \return
     */
    float estimateFuelAvailability(const float grassMoistureContent);

    /*!
     * \brief calculateFirelineIntensity
     * Calculates Byrams fireline Intensity at a certain point of the fire front in KJ/s/m.
     * The required parameters are rate of spread in m/s, available Fuel load in kg/m^2,
     * and the heat yield in kJ/kg.
     * \param rateOfSpread
     * \param availableFuelLoad
     * \param heatYield
     * \return
     */
    float calculateFirelineIntensity(const float rateOfSpread, const float availableFuelLoad);

    /*!
     * \brief calculateCellIgnitionProbability
     *
     * \param Intensity
     * \param criticalIntensity
     * \param moistureContent
     * \return
     */
    float calculateCellIgnitionProbability(const float intensity,
                                           const float moistureContent);

    /*!
     * \brief calculateInitialIgnitionProbability
     * Calculates ignition probability for starting a fire. It was paramerized by the fuel model for
     * Acacia nolica savannas by Wills (1987) so the probability of ignition approaches zero when the
     * extinction moisture of 20% is reached.
     * \param grassFuelMoisture
     * \return
     */
    float calculateInitialIgnitionProbability(const float grassFuelMoisture);

};


} // namespace wildland_firesim
#endif // FIRE_H
