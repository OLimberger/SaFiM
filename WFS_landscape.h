#ifndef WILDLAND_FIRESIM_LANDSCAPE_H
#define WILDLAND_FIRESIM_LANDSCAPE_H

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "csvreader.h"
#include "globals.h"
#include "utility.h"
#include "cell.h"
#include "landscape_interface.h"

namespace wildland_firesim {

/*!
 * \brief The WFS_Landscape class
 * is derived from the landscape_interface class. It contains all relevant landscape information for the fire
 * spread simulation.
 */
class WFS_Landscape : public LandscapeInterface
{
public:
    WFS_Landscape();

    /*!
     * \brief generateLandscapeFromFile
     * to generates a landscape from a parameter file specified by the committed file name.
     * Grass-dominated vegetation is used as the default vegetation type. If such is specified,
     * rectangular, non-flammable vegetation clusters are placed within the landscape.
     * \param fileName
     */
    void generateLandscapeFromFile(const std::string &fileName);

    /*!
     * \brief importLandscapeFromFile
     * imports landscape from three ASCII grid files. The files have to be named "GroundCover.asc",
     * "liveBiomass.asc" and "deadBiomass.asc" in respect to their content.
     */
    void importLandscapeFromFile();

    /*!
     * \brief getWidth
     * returns the width of the landscape.
     * \return
     */
    int getWidth() const noexcept override;

    /*!
     * \brief getHeight
     * returns the height in cells of the landscape.
     * \return
     */
    int getHeight() const noexcept override;

    /*!
     * \brief getCellInformation
     * returns properties of a grid cell at the specified coordinates.
     * \param x
     * \param y
     * \return
     */
    Cell *getCellInformation(int x, int y) override;

    /*!
     * \brief getCellSize
     * returns the cell size.
     * \return
     */
    int getCellSize() const noexcept override;

private:
    std::vector<Cell> cellInformation;

    //Landscape dimensions
    int m_width;
    int m_height;
    int m_cellSize;

    std::size_t datasize;
};

}  // namespace wildland_firesim

#endif // WILDLAND_FIRESIM_LANDSCAPE_H
