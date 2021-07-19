# Directed-Graph-Search-Acyclic
<br/>
both at root and v2 directory
<br/>
Compile program: g++ seq.cpp -o seq <br/>
Run Program: ./seq 0 50<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
<br/>
Compile program: g++ par.cpp -o par -pthread -std=c++17 <br/>
Run Program: ./par 0 50 5<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
Where 5 is the number of workers.<br/>
<br/>
<br/>
only in v2 directory
<br/>
Compile program: g++ ff.cpp -o ff -pthread -std=c++17 <br/>
Run Program: ./ff 0 50 5<br/>
<br/>
Where 0 is the starting vertex.<br/>
Where 50 is the value to find at nodes and get count.<br/>
Where 5 is the number of workers.
<br/>
<br/>
Add -DACTIVEWAIT flag to add delay in the iteration while visiting each node
