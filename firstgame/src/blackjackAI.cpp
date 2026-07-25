#include <iostream>
#include <cmath>
#include <vector>
#include "aiMath.cpp"
/* aiMath.cpp is a file I made to store some of the math done here to make it easier.
 * A lot of the functions are already in CMath, though.
 */
struct Matrix {
        std::string name ;
        const size_t length ;
        std::vector<double> list ;
        const int lengthPerSublist ;
        const size_t amountOfSublists { length / lengthPerSublist } ;
};
/* The following code is to print matrixes. 
 * I changed the << operator to change how std::cout works.
 * I had to look up how to actually change an operator, and the syntax for it, but the
 * logic is all mine.
 */
std::ostream& operator<<(std::ostream&os, Matrix& m) {
        os << m.name << " | [" ;
        for (int i = 0 ; i < m.list.size() ; ++i) {
                if (i == 0 || i % m.lengthPerSublist == 0) {
                        os << "[" ;
                }
                os << m.list[i] ;
                if (!((i + 1) % m.lengthPerSublist == 0)) {
                        os << ", " ;
                }
                if ((i + 1) % m.lengthPerSublist == 0 && !(i + 1 == m.list.size())) {
                        os << "], " ;
                }
                if (i + 1 == m.list.size()) {
                        os << "]" ;
                }
        }
        os << "]" ;
        return os ;
}
class MatrixFunctions {
        public:
        Matrix createMatrix(std::string newName, std::vector<double> list, int lengthOfSublist) {
                const size_t finalLen { list.size() } ;
                return Matrix { newName, finalLen, list, lengthOfSublist } ;
        }
        int indexMatrix(Matrix matrix, int row, int column) {
                // index assumes 0-based counting ; eg (matrix, 0, 2) would return the third item in the first sublist of the matrix
                int finalIndex { row * matrix.lengthPerSublist + column} ;
                return matrix.list[finalIndex] ;
        }
        Matrix matrixTimesScalar(Matrix matrix1, int scalar) {
            Matrix finalmatr {} ;    
            for (int i = 0 ; i < matrix1.list ; ++i) {
                    finalmatr.list += matrix1.list[i] * scalar
                }
            return finalmatr ;
        }
};
class Neuron {
        public:
        
        
};

int main() {
        MatrixFunctions MF ;
        std::vector<double> input { 1, 2, 3, 4, 5, 6, 7, 8, 9 } ;
        std::string first {"first"} ;
        Matrix y = MF.createMatrix("first", input, 3) ;
        std::cout << '\n' << y << '\n' ;
        std::vector<double> input2 { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } ;
        Matrix x = MF.createMatrix("second", input2, 4) ;
        std::cout << x << '\n' ;
        std::vector<double> input3 { 1 } ;
        Matrix z = MF.createMatrix("third", input3, 1) ;
        std::cout << z << '\n' ;
        std::cout << matrixTimesScalar(y, 3) << '\n' ;
        return 0 ;
}
