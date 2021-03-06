# Design-Automation
Academic Project

Annealing1:
1. The aim of this project is to partition a circuit design in two so that the areas between the two partitions are balanced. 
2. Each cell is a part of one or more nets. If a net contains atleast one cell in different partition from the other, it means there is a wire running between the two partitions. 
3. Our aim is to automate the process of placing the cells in the two partitions in such a way that there are minimum number of wires running between the two partitions. 
4. In this process, the benchmark deals with 200,000 cells. Brute force method would be inefficient and time consuming. Heance we use Simulated Annealing algorithm which is a probabilistic technique to approximate the optimal solution in a much efficient manner while keeping the areas balanced.
5. The inputs to this program are a net file consisting of a list of nets and the names of cells in that paticular net. An area file which consists the area of each cell.
6. The data is extracted from the files and stored in the data structures over which the algorithm will be implemented.

Annealing2:
1. The aim of this project is to place the cells on a chip in such a way that the ratio between the height to width of the entire chip is equal to 1. Each cell has the same dimensions which is 16x4. The cells are placed in rows. Each row is seperated from it's neighbouring rows by 16 units.
2. Each cell is a part of a net and the co-ordinats of each cell on the plane are stored in a structure. Using the bounding box model, we can find the wire length of a net if we know the maxmum X, maximum Y, minimum X and minimum Y co-ordinates among all the cells in a net.
3. Our aim is to automate the process of placing the cells on the plane in such a way that the sum of wire lengths of the all the nets combined after the placement is minimum.
4. Due to the high number of cells, exhaustive method is not optimal and heance we use simulated annealing algorithm to obtain an approximate optimal solution.
5. The inputs to this program are net and area files. The net file consists of the list of nets and the cells present in the nets. The area file consists of cell names and their areas. But since we consider the dimensions to be the same, we will not be considering the areas and only use the area file to extract the names of cells by keeping the area as a standard 64 units.
6. Data extracted from these files is stored in data structures over which the algorithm is implemented.
