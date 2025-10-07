#include <iostream>
#include <cstdlib>
#include<string.h>
using namespace std;


// 1. DYNAMIC SAFE ARRAY 
template <typename T>
class safeArray {
    private:
    int ncols; 
    T *dynamicArray;

    public:
        safeArray() : ncols(0), dynamicArray(NULL) {}

        //copy constructor (rule of 3 implementation)
        safeArray (const safeArray & rhs) {
            ncols = rhs.ncols;
            dynamicArray = new T[ncols];
            for(int i = 0; i < ncols; i++)
                dynamicArray[i] = rhs.dynamicArray[i];
        }

        void pushback(T val) {
            T *newArray = new T[ncols + 1];
            for (int i = 0; i < ncols; ++i) {
                newArray[i] = dynamicArray[i]; 
            }
            newArray[ncols] = val; 
            delete[] dynamicArray; 
            dynamicArray = newArray;
            ncols++;
        }

        //destructor
        ~safeArray (){
            delete [] dynamicArray; //free allocated memory
            dynamicArray=NULL;
        }

        // provides bound checking before accessing array 
        T &operator[] (int i) {
            if(i<0 || i>ncols-1 ) {
                cout << "Boundary Error\n";
                exit(1);
            }
            return dynamicArray[i];
        }

        //assignment operator
        safeArray &operator= (const safeArray & rhs) {
            if (this == &rhs) // Self assignment check
                return *this;

            delete[] dynamicArray;
            dynamicArray = NULL;
            ncols = rhs.ncols;
                dynamicArray = new T[ncols];
            for(int i = 0; i < ncols; i++)
                dynamicArray[i] = rhs.dynamicArray[i];

            return *this;
        }
        void print() {
            for(int i = 0; i < ncols; i++)
                cout << dynamicArray[i] << ", ";
            cout << endl;
        }
};

// int main() {
//     // 1. Get array size from user
//     int columns;
//     cout << "enter cols" << endl;
//     cin >> columns;
    
//     // 2. Create first safeArray object
//     safeArray ob1(columns);
    
//     // 3. Fill the array with user input
//     ob1.fillArray();
    
//     // 4. Create a copy using copy constructor
//     safeArray ob2 = ob1;
    
//     // 5. Access and print elements
//     cout << ob1[1] << endl;    // Valid access
//     cout << ob2[4] << endl;    // Testing bounds checking
// }