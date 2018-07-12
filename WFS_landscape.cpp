#include "WFS_landscape.h"

namespace wildland_firesim {

WFS_Landscape::WFS_Landscape()
{}

int
WFS_Landscape::getWidth() const noexcept
{
    return m_width;
}

int
WFS_Landscape::getHeight() const noexcept
{
    return m_height;
}

Cell *WFS_Landscape::getCellInformation(int x, int y)
{
    return &cellInformation[static_cast<size_t>(y * m_width + x)];
}

int
WFS_Landscape::getCellSize() const noexcept {
    return m_cellSize;
}

void
WFS_Landscape::importLandscapeFromFile()
{
    // variables to store ascii grid information
    int width;
    int height;
    //int specifiedCellSize;
    std::vector<std::string> extractedInformation;

    auto readASCIIgrid = [&width, &height, /*&specifiedCellSize,*/ &extractedInformation](std::string fileName){
        int headerSize = 6;
        std::ifstream input_stream{fileName};
        std::string line;

        for(int i = 0; i<headerSize ;i++){
            std::getline(input_stream, line);
            if(line.find("NCOLS")!=std::string::npos){
                std::string param = line.substr(line.find_first_of(" "), line.npos);
                width = utility::asInteger(param);
            }
            if(line.find("NROWS")!=std::string::npos){
                std::string param = line.substr(line.find_first_of(" "), line.npos);
                height = utility::asInteger(param);
            }
            /*
            if(line.find("CELLSIZE")!=std::string::npos){
                std::string param = line.substr(line.find_first_of(" "), line.npos);
                specifiedCellSize = utility::asFloat(param);
            }
            */
        }
        //read ASCIIgrid data
        size_t i = 0;
        extractedInformation.resize(static_cast<size_t>(width * height));
        while(!input_stream.eof()){
            std::getline(input_stream, line);
            std::stringstream ss{line};
            while(ss >> extractedInformation[i]){
                i++;
            }
        }
    };

    //import vegetation data
    readASCIIgrid("GroundCover.asc");
    //resize landscape
    m_width = width;
    m_height = height;
    datasize = static_cast<size_t>(width * height);
    cellInformation.resize(datasize);
    //set cell size
    m_cellSize = 30;
    //cellSize = specifiedCellSize;
    //set vegetation type
    for(size_t i = 0; i < cellInformation.size(); i++){
        if(utility::asInteger(extractedInformation[i]) == 0 ||
                utility::asInteger(extractedInformation[i]) == 1 ||
                utility::asInteger(extractedInformation[i]) == 6){
            cellInformation[i].type = VegetationType::NonFlammable;
        };
        if((utility::asInteger(extractedInformation[i]) == 2) ||
                (utility::asInteger(extractedInformation[i]) == 3) ||
                (utility::asInteger(extractedInformation[i]) == 4)){
            cellInformation[i].type = VegetationType::Grass;
        };
        if(utility::asInteger(extractedInformation[i]) == 5 ){
            cellInformation[i].type = VegetationType::NonFlammable;
        };
    }
    //set dead biomass
    readASCIIgrid("dead-biomass.asc");
    /*
    if(cellSize != specifiedCellSize){
        std::cerr << "cell sizes differ between grid-files!";
        std::exit(1);
    }
    */
    if(m_width != width){
        std::cerr << "dimensions of grid files differ!";
        std::exit(1);
    }
    if(m_height != height){
        std::cerr << "dimensions of grid files differ!";
        std::exit(1);
    }
    for(size_t i = 0; i < cellInformation.size(); i++){
        cellInformation[i].deadBiomass = utility::asFloat(extractedInformation[i]);
    }
    //set live biomass
    readASCIIgrid("live-biomass.asc");
    /*
    if(cellSize != specifiedCellSize){
        std::cerr << "cell sizes differ between grid-files!";
        std::exit(1);
    }
    */
    if(m_width != width){
        std::cerr << "dimensions of grid files differ!";
        std::exit(1);
    }
    if(m_height != height){
        std::cerr << "dimensions of grid files differ!";
        std::exit(1);
    }
    for(size_t i = 0; i < cellInformation.size(); i++){
        cellInformation[i].liveBiomass = utility::asFloat(extractedInformation[i]);
    }
    //set cell states
    for(auto &cell : cellInformation){
        cell.state = CellState::Unburned;
    };
}

void
WFS_Landscape::generateLandscapeFromFile(const std::string &fileName)
{

    size_t NumberOfLandscapeCreationParameters = 10;

    auto reader = csv::Reader{',', '#'};
    auto parameters = reader.parse(fileName);
    // the csv should have one line per parameter
    if (parameters.size() != NumberOfLandscapeCreationParameters) {

        std::cerr << "invalid configuration: there must be at least" << NumberOfLandscapeCreationParameters
                  << " lines in the CSV data\n";
        std::exit(1);
    }
    //parameters for landscape creation
    int width = utility::asInteger(parameters[0][0]);
    int height = utility::asInteger(parameters[1][0]);
    int specificatedCellSize = utility::asInteger(parameters[2][0]);
    float percentageGrass = utility::asFloat(parameters[3][0]);
    float percentageNonFlammable = utility::asFloat(parameters[4][0]);
    float equilibriumGrassFuelLoad = utility::asFloat(parameters[5][0]);
    float degreeOfCuring = utility::asFloat(parameters[6][0]);
    //float variationOfCuring = utility::asFloat(parameters[7][0]);
    float clusterDensity = utility::asFloat(parameters[8][0]);
    size_t meanClusterSize = static_cast<size_t>(utility::asInteger(parameters[9][0]));

    //resize landscape
    m_width = width;
    m_height = height;
    datasize = static_cast<size_t>(width * height);
    cellInformation.resize(datasize);

    //set cell size
    m_cellSize = specificatedCellSize;

    //assign parameters to individual cells within landscape

    //set vegetation type
    //standard vegetation is grass
    for(auto &cell : cellInformation){
        cell.type = VegetationType::Grass; //use grass as standard vegetation type
        cell.state = CellState::Unburned;
    }

    //vegetation type of certain number of cells is then changed according to percentage of the type,
    //cluster size and density.

    if(percentageGrass < 1.f){
        //calculate number of clusters
        int numberOfClusters = static_cast<int>(std::round((percentageNonFlammable * static_cast<float>(datasize)) /
                                                           (clusterDensity * meanClusterSize * meanClusterSize) ));
        //create each cluster by assigning vegetation parameters to individual cells within landscape
        for(int m = 0; m<numberOfClusters; m++){
            //selection of random coordinates (central point in cluster)
            int x = utility::random(m_width - 1);
            int y = utility::random(m_height - 1);

            //create vectors containing coordinates within range given by cluster size
            //empty vectors of size meanClusterSize
            std::vector<int> xRange(meanClusterSize);
            std::vector<int> yRange(meanClusterSize);

            //create sequences with coordinates
            for(size_t i = 0; i < meanClusterSize; i++){
                if(i<meanClusterSize/2){
                    xRange[i] = x+static_cast<int>(i);
                    if(xRange[i]>width) xRange[i] = xRange[i]-width;
                    yRange[i] = y+static_cast<int>(i);
                    if(yRange[i]>height) yRange[i] = yRange[i]-height ;
                }else
                {
                    xRange[i] = x+static_cast<int>(i-meanClusterSize);
                    if(xRange[i]< 0) xRange[i] = xRange[i]+width;
                    yRange[i] = y+static_cast<int>(i-meanClusterSize);
                    if(yRange[i]< 0) yRange[i] = yRange[i]+height;
                }
            }

            //get Cartesian product
            std::vector<std::pair<int, int>> cartesianProduct;
            for (size_t i = 0; i < meanClusterSize; i++)
                for (size_t j = 0; j < meanClusterSize; j++)
                {
                    cartesianProduct.push_back(std::make_pair(xRange[i],yRange[j]));
                }
            // set vegetation type of respective cell
            for(size_t i = 0; i < cartesianProduct.size(); i++){
                Cell *cell = getCellInformation(std::get<0>(cartesianProduct[i]),
                                                std::get<1>(cartesianProduct[i]));
                if(clusterDensity > utility::random()){
                    cell->type = VegetationType::NonFlammable;
                }else
                {
                    if(percentageNonFlammable > random()){
                        cell->type = VegetationType::NonFlammable;
                    }
                }
            }
        }
    }
    for(auto &cell : cellInformation){
        //set biomass correspondent to vegetation type
        if(cell.type == VegetationType::Grass){
            cell.deadBiomass = equilibriumGrassFuelLoad*degreeOfCuring;
            cell.liveBiomass = equilibriumGrassFuelLoad*(1-degreeOfCuring);
        }
        /*if(cell.type == VegetationType::YoungWoody){
            cell.deadBiomass = deadYoungWoodyBiomass;
            cell.liveBiomass = liveYoungWoodyBiomass;
        }
        */
        if(cell.type == VegetationType::NonFlammable){
            cell.deadBiomass = 0.0;
            cell.liveBiomass = 0.0;
        }
    }
}

}  // namespace wildland_firesim
