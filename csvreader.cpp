#include "csvreader.h"

#include <sstream>

namespace wildland_firesim {

namespace csv {
Reader::Reader(char delimiter, char comment)
    : m_delimiter{delimiter}, m_comment{comment}
{}

std::vector<std::vector<std::string>>
Reader::parse(std::string fileName)
{
    std::ifstream input_stream{fileName};

    std::vector<std::vector<std::string>> result;
    std::string line;
    std::string currentValue;

    // Function within Function. Functeption! (lamda expression)
    auto push_result = [&result, &currentValue]() {
        //remove whitespace
        const char *whitespace = " \t\r\n";
        auto start = currentValue.find_first_not_of(whitespace);
        auto end = currentValue.find_last_not_of(whitespace);
        if (start == std::string::npos || end == std::string::npos){
            currentValue.clear();
        }else
        {
        currentValue.substr(start, end-start);
        //copy string stream content into last element of vector in last element of vector
        result.back().emplace_back(currentValue);
        //currentValue.clear();should clear the bit set.. but there were still fucking values in it!
        currentValue.clear();
        }
    };

    while (std::getline(input_stream, line)) {
        //comment lines and empty lines are skipped
        if (line.length() == 0 || line[0] == m_comment){
            continue;
        }
        //new line
        result.emplace_back();
        //if only one value in line get that value
        auto delim = line.find(m_delimiter);
        if(delim == std::string::npos){
            currentValue = line;
            push_result();
            continue;
        }else{

            //get positions of delimiter within string
            std::vector<size_t> pos;

            for(size_t i = 0; i < line.length(); i++){
                if(line[i] == m_delimiter){
                    pos.push_back(i);
                }
            }
            //extract first value
            currentValue = line.substr(0, pos[0]-1);
            push_result();
            for(size_t j = 0; j < pos.size(); j++){
                // starting position,
                currentValue = line.substr(pos[j]+1,pos[j+1]-pos[j]-1);
                push_result();
            }
            //push_result();
        }
    }
    input_stream.close();
    return result;
}

}  // namespace csv
}  // namespace wildland_firesim
