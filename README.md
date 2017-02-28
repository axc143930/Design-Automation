# Design-Automation
Academic Project

1. The aim of this project is to partition a circuit design in two so that the areas between the two partitions are balanced. 
2. Each cell is a part of one or more nets. If a net contains atleast one cell in different partition from the other, it means there is a wire running between the two partitions. 
3. Our aim is to automate the process of placing the cells in the two partitions in such a way that there are minimum number of wires running between the two partitions. 
4. In this process, the benchmark deals with 200,000 cells. Brute force method would be inefficient and time consuming. Heance we use Simulated Annealing algorithm which is a probabilistic technique to approximate the optimal solution in a much efficient manner while keeping the areas balanced.
5. The inputs to this program are a net file consisting of a list of nets and the names of cells in that paticular net. An area file which consists the area of each cell.
6. The data is extracted from the files and stored in the data structures over which the algorithm will be implemented.
