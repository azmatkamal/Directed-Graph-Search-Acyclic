# Directed-Graph-Search-Acyclic
<br/>
Compile program: g++ seq.cpp -o seq -O3 <br/>
Run Program: ./seq 0 50 1000 100 400<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
Where 10000 100 400 is the graph data file with 10000 Nodes and Edges between 100 to 400<br/>
<br/>
Compile program: g++ par.cpp -o par -pthread -O3 -std=c++17 <br/>
Run Program: ./par 0 50 5 1000 100 400<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
Where 5 is the number of workers.<br/>
Where 10000 100 400 is the graph data file with 10000 Nodes and Edges between 100 to 400<br/>
<br/>
<br/>
Compile program: g++ ff.cpp -o ff -pthread -O3 -std=c++17 <br/>
Run Program: ./ff 0 50 5 1000 100 400<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
Where 5 is the number of workers.<br/>
Where 10000 100 400 is the graph data file with 10000 Nodes and Edges between 100 to 400<br/>
<br/>
<br/>
Add -DACTIVEWAIT flag to add delay (500usec) in the iteration while visiting each node
