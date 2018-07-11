#ifndef LANDSCAPE_INTERFACE_H
#define LANDSCAPE_INTERFACE_H

#include "cell.h"

namespace wildland_firesim {

class LandscapeInterface {
public:
    virtual ~LandscapeInterface() = default;

    /*!
     * \brief getWidth
     * returns the width of the landscape
     * \return
     */
    virtual int getWidth() const noexcept = 0;

    /*!
     * \brief getHeight
     * Returns the height in cells of the landscape.
     * \return
     */
    virtual int getHeight() const noexcept = 0;

    virtual Cell *getCellInformation(int x, int y) = 0;

    virtual int getCellSize() const noexcept = 0;

};

}  // namespace wildland_firesim

#endif  // LANDSCAPE_INTERFACE_H
