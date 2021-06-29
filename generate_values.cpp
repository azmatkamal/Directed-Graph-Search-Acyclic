
#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <math.h>
#include <fstream>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()

using namespace std;

int main() {
    const int idx1 = 100;
    const int idx2 = 2;
    int values[idx1][idx2];
    ofstream MyFile("values_data.txt");

    srand(time(NULL));
    for (int r = 0; r < idx1; r++)
    {
        values[r][0] = r;
        values[r][1] = (rand() % 10) * 10;
        MyFile << "{" << values[r][0] << "," << values[r][1] << "}," << endl;
    }
    MyFile.close();

    for (auto r : values)
    {
        cout << r[0] << " " << r[1] << endl;
    }

    return 0;
}
