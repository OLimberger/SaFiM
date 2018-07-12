#include "fire.h"

namespace wildland_firesim {
namespace {

constexpr int CellsBorderingVertex = 4;
constexpr int numberOfCellInternSpreadDirections = 3;

}

Fire::Fire() : burningCellInformationVector()
{

}

void
Fire::spreadFire(LandscapeInterface *landscape, const FireWeatherVariables &weather, int timestepLength)
{
    //create empty pointFireSource vector and correspondend vector with remaining time
    std::vector<pointFireSourceInformation> pointFireSourceInformationVector;

    //calculate distance to cell boundaries
    distance_to_cell_boundary[0] = static_cast<float>(landscape->getCellSize());
    distance_to_cell_boundary[1] = std::sqrt(2.f) * landscape->getCellSize();
    distance_to_cell_boundary[2] = static_cast<float>(landscape->getCellSize());

    // start cellwise routine
    // simulate fire spread within cells (per timestep)
    // iterate over vector with coordinates of all cells burning
    for(size_t i = 0; i<burningCellInformationVector.size(); i++){
        int x = burningCellInformationVector[i].xCoord;
        int y = burningCellInformationVector[i].yCoord;

        //access the respective cell
        Cell* cell = landscape->getCellInformation(x,y);

        //calculate input variables for fire spread
        float fuelLoad = cell->liveBiomass +
                cell->deadBiomass;
        float degreeOfCuring = cell->deadBiomass / fuelLoad;
        float fuelMoisture = estimateGrassFuelMoisture(weather.temperature,
                                                       weather.relHumidity,
                                                       degreeOfCuring);
        float availableFuel = fuelLoad * estimateFuelAvailability(fuelMoisture);

        //calculate headfire rate of spread
        //float headFireRateOfSpread = calculateHeadFireRateOfSpread(fuelMoisture, weather.windSpeed);
        float headFireRateOfSpread = calculateHeadFireRateOfSpread(fuelLoad, fuelMoisture,
                                                                   weather.relHumidity,
                                                                   weather.windSpeed);

        // initialize fireline Intensity sum for vegetation effects
        float sumIntensity = 0.0;

        // access burn status (cell intern routine)
        for(size_t k = 0; k<burningCellInformationVector[i].burnStatus.size(); k++){
            //get rate of spread within the burning cell
            // vector also required for ignition of point fire source
            float rateOfSpread = calculateDirectionalRateOfSpread(weather.windSpeed,
                                                                  weather.windDirection,
                                                                  headFireRateOfSpread,
                                                                  burningCellInformationVector[i].spreadDirection[k]);

            //sum fireline within cell
            sumIntensity = sumIntensity+calculateFirelineIntensity(rateOfSpread,availableFuel);

            //if vertex is already reached by the fire continue.
            if(burningCellInformationVector[i].burnStatus[k] >= 1.f) continue;

            //update burn status
            burningCellInformationVector[i].burnStatus[k] = (rateOfSpread*timestepLength)/distance_to_cell_boundary[k] +
                    burningCellInformationVector[i].burnStatus[k];

            if(burningCellInformationVector[i].burnStatus[k] >= 1.f){
                //new point fire source
                pointFireSourceInformation newPointFireSource;
                //establish new point fire source by adding vertex coordinates to pointFireSources vector
                if(burningCellInformationVector[i].spreadDirection[k]==1){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource+1;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==2){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource+1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource+1;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==3){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource+1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==4){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource+1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource-1;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==5){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource-1;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==6){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource-1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource-1;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==7){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource-1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource;
                }
                if(burningCellInformationVector[i].spreadDirection[k]==8){
                    newPointFireSource.uCoord = burningCellInformationVector[i].uCoordSource-1;
                    newPointFireSource.vCoord = burningCellInformationVector[i].vCoordSource+1;
                }
                //calculate fireline intensity of the section of the fire front
                newPointFireSource.fireIntensity = (calculateFirelineIntensity(rateOfSpread,availableFuel));
                //calculate remaining time at certain pointFireSource
                newPointFireSource.remainingTime = (static_cast<int>(round(((burningCellInformationVector[i].burnStatus[k]-1) *
                                                                            distance_to_cell_boundary[k]) / rateOfSpread)));
                //append new point fire source to vector
                pointFireSourceInformationVector.push_back(newPointFireSource);
                //set burnStatus to 1
                burningCellInformationVector[i].burnStatus[k] = 1.f;
            }
        }//end cell intern routine
        burningCellInformationVector[i].meanFirelineIntensity = sumIntensity /
                burningCellInformationVector[i].burnStatus.size();
    }//end cellwise routine for firespread

    //cellwise routine for burn-out of cells
    for(size_t i = 0; i < burningCellInformationVector.size(); i++){
        int x = burningCellInformationVector[i].xCoord;
        int y = burningCellInformationVector[i].yCoord;

        //access the respective cell
        Cell* cell = landscape->getCellInformation(x,y);

        //summation of burnstatus within respective cell
        float summarizedBurnStatus = 0.f;
        for(size_t q = 0; q<burningCellInformationVector[i].burnStatus.size(); q++){
            summarizedBurnStatus = summarizedBurnStatus + burningCellInformationVector[i].burnStatus[q];
        }

        //if all burnstates are 1 or mean intensity is lower 20.0 (self-extinguished fire in Gauteng), the cell burns out.
        if(summarizedBurnStatus >= burningCellInformationVector[i].burnStatus.size() ||
                ((burningCellInformationVector[i].meanFirelineIntensity) <= 20.f)){

            cell->state = CellState::BurnedOut;
            //remove burned-out cells from burning cell vector
            burningCellInformationVector.erase(burningCellInformationVector.begin() + i);
            numberOfCellsBurning--;
        }
    } //end cellwise routine for cell burn-out

    //ignition of cells
    //point fire source has to be active
    if(!pointFireSourceInformationVector.empty()){
        //iteration over each vertex
        for(size_t i = 0; i<pointFireSourceInformationVector.size(); i++){
            int u = pointFireSourceInformationVector[i].uCoord;
            int v = pointFireSourceInformationVector[i].vCoord;

            //create array of coordinates of cells sharing a vertex
            //direction vector has been changed so north will be shown up.
            int cellsToIgnite[CellsBorderingVertex][2] = {{u,v},{u,v-1},{u-1,v-1},{u-1,v}};
            int direction[CellsBorderingVertex][numberOfCellInternSpreadDirections] = {{1,2,3},{3,4,5},{5,6,7},{7,8,1}};

            //ignite cells
            for(size_t l = 0; l<CellsBorderingVertex; l++){
                int x = cellsToIgnite[l][0];
                int y = cellsToIgnite[l][1];
                // restrict to landscape dimensions.
                if(x > landscape->getWidth()-1 || x < 0) continue;
                if(y > landscape->getHeight()-1 || y < 0) continue;

                //access the respective cell
                Cell* cell = landscape->getCellInformation(x,y);
                if(cell->state == CellState::BurnedOut || cell->state == CellState::Burning) continue;
                //check for vegetation type
                if (cell->type == VegetationType::NonFlammable) continue;
                if (cell->type == VegetationType::Grass){
                    //calculate fire spread variables
                    float fuelLoad = cell->liveBiomass +
                            cell->deadBiomass;
                    float degreeOfCuring = (cell->deadBiomass/ fuelLoad);
                    float fuelMoisture = estimateGrassFuelMoisture(weather.temperature,
                                                                   weather.relHumidity,
                                                                   degreeOfCuring);

                    // test for ignition
                    if(utility::random() < calculateCellIgnitionProbability(pointFireSourceInformationVector[i].fireIntensity,
                                                                          fuelMoisture)){
                        //set cell state to burning
                        burningCellInformation newBurningCell;
                        newBurningCell.uCoordSource = pointFireSourceInformationVector[i].uCoord;
                        newBurningCell.vCoordSource = pointFireSourceInformationVector[i].vCoord;
                        cell->state = CellState::Burning;
                        newBurningCell.meanFirelineIntensity = 0.0;
                        numberOfCellsBurning++;
                        //add coordinates to cellsBurning vector
                        newBurningCell.xCoord = cellsToIgnite[l][0];
                        newBurningCell.yCoord = cellsToIgnite[l][1];
                        //add direction to spreadDirection vector
                        //add travel distance of 0 to burnStatus vector
                        for(size_t ll=0; ll<numberOfCellInternSpreadDirections; ll++){
                            newBurningCell.spreadDirection.push_back(direction[l][ll]);
                            //simulate fire spread using the remaining time
                            float headFireRateOfSpread = calculateHeadFireRateOfSpread(fuelLoad, fuelMoisture,
                                                                                       weather.relHumidity,
                                                                                       weather.windSpeed);
                            /*
                            float headFireRateOfSpread = calculateHeadFireRateOfSpread(fuelMoisture,
                                                                                       weather.windSpeed);
                            */
                            float rateOfSpread =  calculateDirectionalRateOfSpread(weather.windSpeed,
                                                                                   weather.windDirection,
                                                                                   headFireRateOfSpread,
                                                                                   newBurningCell.spreadDirection[ll]);
                            //update burn status
                            newBurningCell.burnStatus.push_back((rateOfSpread*pointFireSourceInformationVector[i].remainingTime) /
                                                                distance_to_cell_boundary[ll]);
                        }
                        burningCellInformationVector.push_back(newBurningCell);
                    }
                }
            }
        }
    }
}

void
Fire::initiateWildFire(LandscapeInterface *landscape, const FireWeatherVariables &weather)
{
    //set counter for burning cells to zero as no cell is burning before the fire is ignited
    numberOfCellsBurning = 0;
    burningCellInformationVector.clear();

    //choose random vertex within landscape grid
    int u = utility::random(landscape->getWidth() - 1);
    int v = utility::random(landscape->getHeight() - 1);

    //create array of coordinates of cells sharing a vertex
    int cellsToIgnite[CellsBorderingVertex][2] = {{u,v},{u,v-1},{u-1,v-1},{u-1,v}};
    int direction[CellsBorderingVertex][numberOfCellInternSpreadDirections] = {{1,2,3},{3,4,5},{5,6,7},{7,8,1}};

    //try to ignite all adjacent cells
    for(int l = 0; l<CellsBorderingVertex; l++){
        int x = cellsToIgnite[l][0];
        int y = cellsToIgnite[l][1];

        //check if coordinates are within landscape
        if(x > landscape->getWidth()-1 || x < 0) continue;
        if(y > landscape->getHeight()-1 || y < 0) continue;

        //access the respective cell
        Cell* cell = landscape->getCellInformation(x,y);
        //check for vegetation type
        if (cell->type == VegetationType::Grass){
            //check for minimal fuel requirement
            float fuelLoad = cell->deadBiomass + cell->liveBiomass;
            float minimumFuel = 200;
            if(fuelLoad >= minimumFuel){
                //calculate fuel moisture
                float degreeOfCuring = (cell->deadBiomass / fuelLoad);
                float fuelMoisture = estimateGrassFuelMoisture(weather.temperature,
                                                               weather.relHumidity,
                                                               degreeOfCuring);
                //check for ignition
                if(utility::random() < calculateInitialIgnitionProbability(fuelMoisture)){
                    //set cell state to burning
                    burningCellInformation newBurningCell;
                    newBurningCell.uCoordSource = u;
                    newBurningCell.vCoordSource = v;
                    cell->state = CellState::Burning;
                    //add coordinates to vector of burning cells
                    newBurningCell.xCoord = x;
                    newBurningCell.yCoord = y;
                    //add burnstatus into vector
                    for(int ll=0; ll<numberOfCellInternSpreadDirections; ll++){
                        newBurningCell.spreadDirection.push_back(direction[l][ll]);
                        newBurningCell.burnStatus.push_back(0.f);
                    }
                    burningCellInformationVector.push_back(newBurningCell);
                    newBurningCell.meanFirelineIntensity = 0.0;
                    numberOfCellsBurning++;
                }
            }
        }
        // if the requirements for initiation of the fire are not met, the cell is not ignited.
    }
}

void
Fire::setCenteredIgnitionPoint(LandscapeInterface *landscape)
{
    //set counter for burning cells to zero as no cell is burning before the fire is ignited
    numberOfCellsBurning = 0;
    burningCellInformationVector.clear();

    //calculate coordinates of central vertex
    int u = static_cast<int>(round(landscape->getWidth()/2));
    int v = static_cast<int>(round(landscape->getHeight()/2));

    //create array of coordinates of cells sharing a vertex
    int cellsToIgnite[CellsBorderingVertex][2] = {{u,v},{u,v-1},{u-1,v-1},{u-1,v}};
    int direction[CellsBorderingVertex][numberOfCellInternSpreadDirections] = {{1,2,3},{3,4,5},{5,6,7},{7,8,1}};

    /*
    * Fixed Ignition of Centered Cell in quadratic landscapes
    */
    //try to ignite all adjacent cells
    for(int l = 0; l<CellsBorderingVertex; l++){
        int x = cellsToIgnite[l][0];
        int y = cellsToIgnite[l][1];
        //access the respective cell
        Cell* cell = landscape->getCellInformation(x,y);
        //check for vegetation type
        if (cell->type == VegetationType::Grass){
            //check for minimal fuel requirement
            float fuelLoad = cell->deadBiomass + cell->liveBiomass;
            float minimumFuel = 0.0;
            if(fuelLoad >= minimumFuel){
                    //set cell state to burning
                    burningCellInformation newBurningCell;
                    newBurningCell.uCoordSource = u;
                    newBurningCell.vCoordSource = v;
                    cell->state = CellState::Burning;
                    //add coordinates to vector of burning cells
                    newBurningCell.xCoord = x;
                    newBurningCell.yCoord = y;
                    //add burnstatus into vector
                    for(int ll=0; ll<numberOfCellInternSpreadDirections; ll++){
                        newBurningCell.spreadDirection.push_back(direction[l][ll]);
                        newBurningCell.burnStatus.push_back(0.f);
                    }
                    newBurningCell.meanFirelineIntensity = 0.0;
                    burningCellInformationVector.push_back(newBurningCell);
                    numberOfCellsBurning++;
            }
        }
    }
}

void
Fire::initiatePrescribedBurning()
{
    //check year for ignition of prescribed fires (fire frequency)
    //check for ignition within the respective month
    //get fire weather suited for prescribed burning
    // if unsuited recalculate fire weather until suited conditions are received or
    // limit is reached. Limit is given by probability for month. (1=30, 0.5 =15 ...)
    //get camp identification for ignition
    //depending on wind direction ignite camps border to receive specified fire type
}


float
Fire::calculateHeadFireRateOfSpread(const float fuelload, const float moistureContent, const float relHumidity, const float windspeed)
{

    return 0.4745+(0.0002*fuelload)-(0.6648*moistureContent)-
            (0.0023*relHumidity)+(0.0441*windspeed);
}

float
Fire::calculateDirectionalRateOfSpread(const float windSpeed, const int windDirection,
                                       const float headFireRateOfSpread, const int directionOfFireSpread)
{
    //convert windspeed from m/s to km/h
    float totalWindSpeed = windSpeed * 3.6f;
    float lengthToBreadthRatio;
    float rateOfSpread;

    if(totalWindSpeed == 0.0f){
        rateOfSpread = headFireRateOfSpread;
    }else{
        lengthToBreadthRatio = 1.1f * std::pow(totalWindSpeed, .464f);
        if(lengthToBreadthRatio < 1.f) lengthToBreadthRatio = 1.f;

        float a = headFireRateOfSpread / (1 + std::sqrt((1 - std::pow(lengthToBreadthRatio, (-2.f)))));
        float b = a / lengthToBreadthRatio;
        float c = headFireRateOfSpread - a;

        int directionalDifference = windDirection - directionOfFireSpread;
        float theta= directionalDifference * 45 * (Pi/180); //degree to radians

        //rateOfSpread = (std::pow(a,2) - std::pow(c,2))/(a - c * std::cos(theta));
        rateOfSpread = (b*(a+c*cos(theta))) / std::sqrt(pow(a,2) * std::pow(std::sin(theta), 2) +
                                                        std::pow(b,2) * std::pow(std::cos(theta),2));
    }
    return static_cast<float>(rateOfSpread);
}

float
Fire::estimateGrassFuelMoisture(const float temperature, const float relativeHumidity,
                                const float curing)
{
    float fuelMoisture;
    if(curing == 0.f ){
        fuelMoisture = 1.f;
    } else (
        fuelMoisture = (((97.7f + 4.06f * relativeHumidity) /
                        (temperature + 6.f)) - .00854f * relativeHumidity +
                        (3000.f / (curing * 100)) - 30.f) / 100 );

    return fuelMoisture;
}

float
Fire::calculateCellIgnitionProbability(const float intensity, //const int criticalIntensity,
                                       const float moistureContent)
{
    //int latentHeat = 2600;
    return static_cast<float>(1.f/(1.f + std::exp(-(-5.6f - 6.8f * moistureContent
                                 + exp(-0.15f + std::pow(intensity, 0.117f))))));
    //return std::sqrt(intensity / (criticalIntensity + (latentHeat * moistureContent)));
}

float
Fire::calculateInitialIgnitionProbability(const float grassFuelMoisture){
    return 1 / ( 1 + std::exp(-(4.2f - .18f * (grassFuelMoisture*100))));
}


float
Fire::calculateFirelineIntensity(const float rateOfSpread, const float availableFuelLoad)
{
    // fuel load is converted from g/m³ to kg/m²
    return rateOfSpread * (availableFuelLoad / 1000) * heatYield;
}

float
Fire::estimateFuelAvailability(const float grassMoistureContent){
    float fuelAvailability;
    float mc = grassMoistureContent*100.f;

    if(mc > 20){
        fuelAvailability = (113.1407f - 0.6325f * mc)/ 100;
    }
    else{
        fuelAvailability = 1.f;
    }
    return fuelAvailability;
}

}  // namespace wildland_firesim
