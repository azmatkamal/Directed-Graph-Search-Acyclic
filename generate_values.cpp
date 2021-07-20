
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

int main(int argc, char *argv[])
{
    int idx1 = atoi(argv[1]);
    if(!idx1) {
        idx1 = 100;
    }
    const int idx2 = 2;
    int values[idx1][idx2];
    ofstream MyFile("graph_values.cpp");

    srand(time(NULL));
    MyFile << "int nodes[][2] = { " << endl;
    for (int r = 0; r < idx1; r++)
    {
        values[r][0] = r;
        values[r][1] = (rand() % 10) * 10;
        if (r == idx1 - 1)
        {
            MyFile << "{" << values[r][0] << "," << values[r][1] << "}" << endl;
        }
        else
        {
            MyFile << "{" << values[r][0] << "," << values[r][1] << "}," << endl;
        }
    }
    MyFile << " }; " << endl << endl;
    MyFile << "int values2[] = { " << endl;
    for (int r = 0; r < idx1; r++)
    {
        values[r][0] = r;
        values[r][1] = (rand() % 10) * 10;
        if (r == idx1-1)
        {
            MyFile << values[r][1] << endl;
        }
        else
        {
            MyFile << values[r][1] << "," << endl;
        }
    }
    MyFile << " }; ";
    MyFile.close();

    for (auto r : values)
    {
        cout << r[0] << " " << r[1] << endl;
    }

    return 0;
}
