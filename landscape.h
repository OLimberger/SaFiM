#ifndef WILDLAND_FIRESIM_LANDSCAPE_H
#define WILDLAND_FIRESIM_LANDSCAPE_H

#include <cstdlib>
#include <cmath>
#include <iostream>
#include "csvreader.h"
#include "globals.h"
#include "utility.h"
#include "cell.h"

namespace wildland_firesim {

class Landscape
{
public:
    Landscape();

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
    int getWidth() const;

    /*!
     * \brief getHeight
     * Returns the height in cells of the landscape.
     * \return
     */
    int getHeight() const;

    Cell *getCellInformation(int x, int y);

    std::vector<Cell> cellInformation;
    int cellSize;

private:
    //Landscape dimensions
    int m_width;
    int m_height;

    std::size_t datasize;
};

}  // namespace wildland_firesim

#endif // WILDLAND_FIRESIM_LANDSCAPE_H
