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

class WFS_Landscape : public LandscapeInterface
{
public:
    WFS_Landscape();

    /*!
     * \brief generateLandscapeFromFile
     * \param fileName
     */
    void generateLandscapeFromFile(const std::string &fileName);

    void importLandscapeFromFile();

    /*!
     * \brief getWidth
     * returns the width of the landscape
     * \return
     */
    int getWidth() const noexcept override;

    /*!
     * \brief getHeight
     * Returns the height in cells of the landscape.
     * \return
     */
    int getHeight() const noexcept override;

    Cell *getCellInformation(int x, int y) override;

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
