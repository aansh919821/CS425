# CS425 Assignment 4 – DVR & LSR Routing Algorithms

## Objective

This assignment implements the **Distance Vector Routing (DVR)** and **Link State Routing (LSR)** algorithms using **C++**. The goal is to:

- Understand the internal workings of routing algorithms used in network communication.
- Simulate routing table updates using DVR (based on Floyd-Warshall).
- Simulate path discovery using LSR (based on Dijkstra’s algorithm).
- Generate and print the routing tables for each node.

---

##  How to Run

###  Requirements

- A *Linux system* (or any OS with a g++ compiler).
- *g++ compiler*

###  Steps

1. **Compile the code:**

   ```bash
   g++ -std=c++11 -o routing_sim routing_sim.cpp
   ``` 

2. **Prepare an input file (e.g., input.txt) in the following format:**
    4
    0 10 100 30
    10 0 20 40
    100 20 0 10
    30 40 10 0

    - First line: number of nodes (N)

    - Next N lines: adjacency matrix (use 9999 to represent infinite/unreachable cost)

3. **Run the simulation:**

   ```bash
   ./routing_sim inputfile.txt
   ```

##  Explanation of the Code

1. **Reading the Graph:**

   ```cpp
   vector<vector<int>> readGraphFromFile(const string& filename);
   ```

   - Reads an N×N adjacency matrix from the input file where each element represents the cost between nodes.

2. **Simulating Distance Vector Routing (DVR):**

   ```cpp
   void simulateDVR(const vector<vector<int>>& graph);
   ```

   - Uses Floyd-Warshall-style updates.

   - Updates routing tables by minimizing the path cost via intermediate nodes.

   - Outputs final routing table per node.

3. **Simulating Link State Routing (LSR):**

   ```cpp
   void simulateLSR(const vector<vector<int>>& graph);
   ```

   - Applies Dijkstra’s algorithm from each source node.

   - Determines shortest paths and next hops.

   - Prints routing table with costs and next hops.

4. **Main Program Logic:**

   ```cpp
   int main(int argc, char *argv[]);
   ```

###  Notes & Assumptions

1. The adjacency matrix must be symmetric if the graph is undirected.

2. A value of 0 denotes no link (or cost to self), and 9999 represents an unreachable path.

3. The program prints the final DVR tables and final LSR tables for each node.

##  Sample Outputs

###  Output for input.txt

4
0 10 30 30
10 0 20 30
30 20 0 10
30 30 10 0

```bash
$ ./routing_sim input.txt
```

## Distance Vector Routing Simulation

### DVR Final Tables

#### Node 0 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 0    | -        |
| 1    | 10   | 1        |
| 2    | 30   | 1        |
| 3    | 30   | 3        |

#### Node 1 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 10   | 0        |
| 1    | 0    | -        |
| 2    | 20   | 2        |
| 3    | 30   | 2        |

#### Node 2 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 30   | 1        |
| 1    | 20   | 1        |
| 2    | 0    | -        |
| 3    | 10   | 3        |

#### Node 3 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 30   | 0        |
| 1    | 30   | 2        |
| 2    | 10   | 2        |
| 3    | 0    | -        |

---

## Link State Routing Simulation

#### Node 0 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 1    | 10   | 1        |
| 2    | 30   | 1        |
| 3    | 30   | 3        |

#### Node 1 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 10   | 0        |
| 2    | 20   | 2        |
| 3    | 30   | 2        |

#### Node 2 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 30   | 1        |
| 1    | 20   | 1        |
| 3    | 10   | 3        |

#### Node 3 Routing Table:
| Dest | Cost | Next Hop |
|------|------|----------|
| 0    | 30   | 0        |
| 1    | 30   | 2        |
| 2    | 10   | 2        |

##  Testing: Conditions for Success

###  DVR Should Output Correct Routing Tables If:

- The shortest path costs are correctly calculated across nodes.
- The next hop reflects the correct intermediate node.

###  LSR Should Output Correct Tables If:

- The Dijkstra’s shortest path tree is correctly reflected.
- The next hop is the first node along the path from source.

## Contributors 

All team members have equally contributed to this assignment:

- **Yash Chauhan** (221217)
- **Ansh Agarwal** (220165)
- **Pushkar Aggarwal** (220839)