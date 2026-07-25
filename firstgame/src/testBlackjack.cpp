#include <iostream>
#include <string>
struct Matrix {
        std::string name ;
        const size_t length ;
        std::vector<double> list ;
        const int lengthPerSublist ;
        const size_t amountOfSublists { length / lengthPerSublist } ;
};