#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <set>
#include <cstdlib>
#include <cmath>

#define pb push_back
#define mp make_pair

#define C 4 // size of the combination
#define MAX 10100 // max number of vertices
#define MAX_E 2*MAX // upper-bound of regions on a planar graph
#define PERM 10 // (MAX*(MAX-1)*(MAX-2)*(MAX-3))/24 - upper-bound of permutations

using namespace std;

void printElapsedTime(clock_t start, clock_t stop){
    double elapsed = ((double)(stop - start)) / CLOCKS_PER_SEC;
    cout << fixed << setprecision(3) << "Elapsed time: " << elapsed << "s\n";
}

/*
    SIZE   ---> Number of vertices
    faces  ---> Quantity of triangular faces
    qtd    ---> Number of possible 4-cliques
    T      ---> Output graph
    R      ---> Output graph for an optimal solution
    F      ---> List containing triangular faces
    seeds  ---> Permutations of possible starting 4-cliques
    graph ---> The graph itself
*/
int graph[MAX][MAX], seeds[PERM][C], T[MAX][MAX], F[MAX_E][3], R[MAX][MAX];
int SIZE, faces = 0, qtd = 0;

void readInput()
{
    cin >> SIZE;
    for (int i = 0; i < SIZE; i++) {
        for (int j = i + 1; j < SIZE; j++) {
            cin >> graph[i][j];
            graph[j][i] = graph[i][j];
        }
        graph[i][i] = -1;
    }
}

/*
    v      ---> Size of the input array
    r      ---> Size of the combination
    index  ---> Current index in data[]
    data[] ---> Temporary array to store a current combination
    i      ---> Index of current element in vertices[]
*/
void combineUntil(int index, vector<int>& data, int i)
{
    // Current cobination is ready, print it
    if (index == C) {
        for (int j = 0; j < C; j++) {
            seeds[qtd][j] = data[j];
        }
        qtd++;
        return;
    }
 
    // When there are no more elements to put in data[]
    if (i >= SIZE) return;
    // current is inserted; put next at a next location
    data[index] = i;
    combineUntil(index+1, data, i+1);
    // current is deleted; replace it with next
    combineUntil(index, data, i+1);
}

void combine()
{
    vector<int> data(C);
    // print all combinations of size 'r' using a temporary array 'data'
    combineUntil(0, data, 0);
}

void clearGraph()
{
    for (int i = 0; i < SIZE; i++) {
        for (int j = i+1; j < SIZE; j++) {
            T[i][j] = T[j][i] = -1;
        }
    }

    for (int i = 0; i < 2*SIZE; i++) {
        for (int j = 0; j < 3; j++) {
            F[i][j] = -1;
        }
    }
}

void generateClique(int idx)
{
    for (int i = 0; i < C; i++) {
        for (int j = i+1; j < C; j++) {
            int va = seeds[idx][i], vb = seeds[idx][j];
            T[va][vb] = T[vb][va] = graph[va][vb];
        }
    }
}

// generates a list containing the vertices which are not
// on the planar graph
void generateList(int idx, set<int>& V)
{
    for (int i = 0; i < SIZE; i++) {
        if (i != seeds[idx][0] && i != seeds[idx][1]
            && i != seeds[idx][2] && i != seeds[idx][3])
            V.insert(i);
    }
}

// returns the weight of the planar graph so far
int generateTriangularFaceList(int idx)
{
    int resp = 0;
    int va = seeds[idx][0], vb = seeds[idx][1], vc = seeds[idx][2], vd = seeds[idx][3];

    // generate first triangle of the output graph
    F[faces][0] = va, F[faces][1] = vb, F[faces++][2] = vc;
    resp = graph[va][vb] + graph[va][vc] + graph[vb][vc];

    // generate the next 3 possible faces
    F[faces][0] = va, F[faces][1] = vb, F[faces++][2] = vd;
    F[faces][0] = va, F[faces][1] = vc, F[faces++][2] = vd;
    F[faces][0] = vb, F[faces][1] = vc, F[faces++][2] = vd;
    resp += graph[va][vd] + graph[vb][vd] + graph[vc][vd];

    return resp;
}

// inserts a new vertex, 3 new triangular faces
// and removes face 'f' from the list
int operationT2(int new_vertex, int f)
{
    // remove the chosen face and insert a new one
    int va = F[f][0], vb = F[f][1], vc = F[f][2];
    F[f][0] = new_vertex, F[f][1] = va, F[f][2] = vb;
    // and insert the other two possible faces
    F[faces][0] = new_vertex, F[faces][1] = va, F[faces++][2] = vc;
    F[faces][0] = new_vertex, F[faces][1] = vb, F[faces++][2] = vc;

    int resp = graph[va][new_vertex] + graph[vb][new_vertex] + graph[vc][new_vertex];

    T[new_vertex][va] = T[va][new_vertex] = graph[new_vertex][va];
    T[new_vertex][vb] = T[vb][new_vertex] = graph[new_vertex][vb];
    T[new_vertex][vc] = T[vc][new_vertex] = graph[new_vertex][vc];

    return resp;
}

// returns the vertex with the maximum gain inserting within a face 'f'
int maxGain(set<int>& V, int* f)
{
    set<int>::iterator it = V.begin();

    int gain = -1, vertex = -1;
    // iterate through the remaining vertices
    for (; it != V.end(); it++) {
        // and test which has the maximum gain with its insetion
        // within all possible faces
        for (int i = 0; i < faces; i++) {
            int va = F[i][0], vb = F[i][1], vc = F[i][2], new_vertex = *it;
            int tmpGain = graph[va][new_vertex] + graph[vb][new_vertex] + graph[vc][new_vertex];
            if (tmpGain > gain) {
                gain = tmpGain;
                *f = i;
                vertex = new_vertex;
            }
        }
    }
    return vertex;
}

int tmfg(set<int>& V, int tmpMax)
{
    int maxValue = tmpMax;

    while (!V.empty()) {
        int f = -1;
        int vertex = maxGain(V, &f);
        V.erase(vertex);
        maxValue += operationT2(vertex, f);
    }
    return maxValue;
}

int main(int argv, char** argc)
{
    ios::sync_with_stdio(false);
    clock_t start, stop;

    //read the input, which is given by a size of a graph and its weighted edges.
    //the graph given is dense.
    readInput();

    //generate multiple 4-clique seeds, given the number of vertices
    //combine();
    for (int j = 0; j < C; j++)
        seeds[0][j] = j;

    int respMax = -1;
    set<int> V;

    start = clock();
    //for ( int i = 0; i < qtd; i++ ){
    for (int i = 0; i < 1; i++) {
        faces = 0;
        clearGraph();
        // generate a 4-clique (tetrahedron), given a permutation of vertices,
        // a list, with the remaining vertices and a list with available faces
        generateClique(i);
        generateList(i, V);

        int tmpMax = generateTriangularFaceList(i);
        // call the triangular maximally filtered graph procedure,
        // passing a 4-clique as seed
        int ans = tmfg(V, tmpMax);

        if (ans >= respMax) {
            respMax = ans;
            for (int j = 0; j < SIZE; j++) {
                for (int k = j+1; k < SIZE; k++)
                    R[j][k] = R[k][j] = T[j][k];
                R[j][j] = -1;
            }
        }
    }
    stop = clock();
    
    // cout << "Printing generated graph: " << endl;
    cout << SIZE << endl;
    for (int i = 0; i < SIZE; i++) {
        for (int j = i+1; j < SIZE; j++) {
            cout << R[i][j] << " ";
        }
        cout << endl;
    }

    //printElapsedTime(start, stop);
    cout << "Maximum weight found: " << respMax << endl;

    return 0;
}