#ifndef WILDLAND_FIRESIM_CSVREADER_H
#define WILDLAND_FIRESIM_CSVREADER_H

#include <sstream>
#include <istream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

namespace wildland_firesim {
namespace csv {
/*!
 * \brief The Reader class
 * Class containing the parser for csv-files.
 */
class Reader{

public:
    Reader(char delimiter, char comment);
    /*!
     * \brief parse
     * Function to parse .csv-files.
     * \param fileName
     * \return
     */
    std::vector<std::vector<std::string>> parse(std::string fileName);

private:
  char m_delimiter;
  //char m_quote;
  char m_comment;
};

} //namespace wildland_firesim
} //namespace csv


#endif // WILDLAND_FIRESIM_CSVREADER_H
