#include<iostream>
#include <stdlib.h>
#include <list>
#include <fstream>

using namespace std;
void GenRandomGraphs(int NOEdge, int NOVertex)
{
    ofstream MyFile("graph_data.cpp");
    int i, j, edge[NOEdge][2], count;
    i = 0;
    //Assign random values to the number of vertex and edges
    //    of the graph, Using rand().
    while (i < NOEdge)
    {
        edge[i][0] = rand() % NOVertex + 1;
        edge[i][1] = rand() % NOVertex + 1;
        //Print the connections of each vertex, irrespective of
        //   the direction.
        if (edge[i][0] == edge[i][1])
            continue;
        else
        {
            for (j = 0; j < i; j++)
            {
                if ((edge[i][0] == edge[j][0] &&
                     edge[i][1] == edge[j][1]) ||
                    (edge[i][0] == edge[j][1] &&
                     edge[i][1] == edge[j][0]))
                    i--;
            }
        }
        i++;
    }
    cout << "\nThe generated random graph is: ";
    MyFile << "int edges[][2] = { " << endl;
    for (i = 0; i < NOVertex; i++)
    {
        count = 0;
        int aaa = i;
        // cout << "\n\t" << aaa << "-> { ";
        list<int> aa;
        for (j = 0; j < NOEdge; j++)
        {
            if (edge[j][0] == i + 1)
            {
                aa.push_back(edge[j][1]);
                // cout << edge[j][1] << " ,";
                count++;
            }
            else if (edge[j][1] == i + 1)
            {
                aa.push_back(edge[j][0]);
                // cout << edge[j][0] << " ,";
                count++;
            }
            else if (j == NOEdge - 1 && count == 0)
                cout << "Isolated Vertex!"; //Print “Isolated vertex” for the vertex having no degree.
        }
        aa.sort();
        // cout << " }";

        list<int>::iterator i;
        cout << endl
             << aaa << " >>> ";
        int x = 0;
        for (i = aa.begin(); i != aa.end(); ++i)
        {
            cout << *i << " - ";
            if (aaa == NOVertex - 1)
            {
                if (x == aa.size() - 1)
                {
                    MyFile << "{" << aaa << "," << ((*i == NOVertex) ? NOVertex - 1 : *i) << "}" << endl;
                }
                else
                {
                    MyFile << "{" << aaa << "," << ((*i == NOVertex) ? NOVertex - 1 : *i) << "}," << endl;
                }
            }
            else
            {
                MyFile << "{" << aaa << "," << ((*i == NOVertex) ? NOVertex - 1 : *i) << "}," << endl;
            }
            x++;
        }
        cout << endl;
    }
    MyFile << " }; ";

    MyFile.close();
}

int main(int argc, char *argv[])
{
   int i, e;
   cout << "Random graph generation: ";
   int n = atoi(argv[1]);
   if (!n)
   {
       n = 100;
   }
   cout<< "\nThe graph has " << n << " vertices";
   e = ((n*(n-1))/10);
   cout << "\nand has " << e << " edges.";
   cout << endl;
   GenRandomGraphs(e, n);
   cout << "\nThe graph has " << n << " vertices";
   cout << "\nand has " << e << " edges.";
   cout << endl;
}