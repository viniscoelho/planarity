#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
#define MAX 5010

using namespace std;

#include "includes/helpers.hpp"
#include "includes/avl.hpp"

/*
    V -> number of vertices
    E -> number of edges
*/
int V, E;

vector<int> graph[MAX];
int vertex_map[MAX];

/*
    getVertex checks if the graph has a vertex with degree <= 5 and
    returns the first one found. Otherwise, returns -1.
*/
int getVertex()
{
    for (int v = 0; v < V; v++)
        if (graph[v].size() <= 5) return v;
    return -1;
}

/*
    isTriangle checks if three vertices form a triangle.
*/
bool isTriangle(int v1, int v2, int vn)
{
    int count = 0;
    for (int i = 0; i < graph[v1].size(); i++)
        if (graph[v1][i] == v2 || graph[v1][i] == vn) count++;

    for (int i = 0; i < graph[v2].size(); i++)
        if (graph[v2][i] == v1 || graph[v2][i] == vn) count++;

    return count == 4;
}

/*
    order returns a canonical order of V(G) of three given vertices.
*/
vector<int> order(int v1, int v2, int vn)
{
    // vector<int> graph_[MAX];
    vector<int> VC, vi(V);
    // vi -> vector with the output order of vertices
    vi[0] = v1; vi[1] = v2;
    VC.pb(v1); VC.pb(v2); VC.pb(vn);

    // x -> number of elements in VC
    int x = VC.size(), pos = V-1;

    // set with remaining and removed vertices
    set<int> vertices, R;
    // insert all vertices but v1 and v2
    for (int i = 0; i < V; i++)
        if (v1 != i && v2 != i)
            vertices.insert(i);

    while (!vertices.empty()) {
        int v, sz = -1;

        // choose a vertex v belonging to VC which is neither v1 nor v2
        // and which its neighbours' intersection with VC has size == 2.
        for (int i = 0; i < x; i++) {
            if (VC[i] == v1 || VC[i] == v2) continue;
            v = VC[i];

            // intersection
            node *root = NULL;
            for (int k = 0; k < VC.size(); k++) {
                root = insert(root, VC[k]);
            }
            vector<int> aux;
            for (int k = 0; k < graph[v].size(); k++) {
                int at = graph[v][k];
                if (find(root, at) && !R.count(at)) {
                    aux.pb(at);
                }
            }
            sz = aux.size();

            if (sz != 2) continue;
            break;
        }
        // if such vertex does not exist, halt.
        if (sz != 2) return vector<int>();

        // otherwise, remove this vertex from VC
        for (int i = 0; i < x; i++) {
            if (VC[i] == v) {
                swap(VC[i], VC[x-1]);
                VC.pop_back();
                break;
            }
        }

        vertices.erase(v);
        R.insert(v);

        // and join VC with the chosen vertex's neighbours.
        // union
        node *root = NULL;
        vector<int> aux;
        for (int k = 0; k < VC.size(); k++) {
            int at = VC[k];
            // if ( R.count(at) ) continue; 
            root = insert(root, at);
            aux.pb(at);
        }
        for (int k = 0; k < graph[v].size(); k++) {
            int at = graph[v][k];
            if (!find(root, at) && !R.count(at)) {
                aux.pb(at);
            }
        }
        x = aux.size();
        VC = aux;

        // add the chosen vertex to the answer
        vi[pos] = v; pos--;
    }

    return vi;
}

/*
    areConsecutive checks if a set of vertices appear
    consecutively into another sequence.
*/
bool areConsecutive(vector<int>& tmp, vector<int>& VC)
{
    int sz = VC.size(), t = tmp.size(), k = sz;
    
    if (t > sz) return false;

    // where is the lower bound occurence of an element on the intersection?
    node *root = NULL;
    for (int i = 0; i < t; i++) {
        root = insert(root, tmp[i]);
        k = min(k, vertex_map[tmp[i]]);
    }

    if (k+t > sz) return false;

    for (int i = 0; i < t; i++, k++) {
        if (!find(root, VC[k])) return false;
    }
    return true;
}

/*
    embed checks if the graph has a planar embedding.
*/
bool embed(vector<int>& pi)
{
    vector<int> VC;
    VC.pb(pi[0]); VC.pb(pi[2]); VC.pb(pi[1]);

    for (int i = 3; i < V; i++) {
        // sublist {u_p, u_p+1, ..., u_p+n} = {v1, ..., v_i-1} inter NG(vi)
        // note: optimize this later...

        // intersection
        node *inter = NULL;
        for (int k = 0; k < i; k++) {
            inter = insert(inter, pi[k]);
        }
        vector<int> tmp;
        for (int k = 0; k < graph[pi[i]].size(); k++) {
            int at = graph[pi[i]][k];
            if (find(inter, at)) {
                tmp.pb(at);
            }
        }

        // for ( int j = 0; j < N; j++ ) vertex_map[i] = -1;
        for (int j = 0; j < VC.size(); j++)
            vertex_map[VC[j]] = j;

        // where is the lower and higher bound occurences of the elements
        // on the intersection?
        int lb = VC.size(), hb = -1;
        for (int i = 0; i < tmp.size(); i++) {
            lb = min(lb, vertex_map[tmp[i]]);
            hb = max(hb, vertex_map[tmp[i]]);
        }

        if (areConsecutive(tmp, VC)) {
            vector<int> cons;
            int k = 0;
            while (k < lb) cons.pb(VC[k++]);
            cons.pb(VC[lb]);
            cons.pb(pi[i]);
            cons.pb(VC[hb]);
            while (k <= hb) k++;
            while (k < VC.size()) cons.pb(VC[k++]);

            VC = cons;
        }
        else return false;
    }
    return true;
}

/*
    recognize checks if a given graph is either maximal planar or not.
*/
bool recognize()
{
    int v1 = getVertex(), v2, p = graph[v1].size();
    if (E != (3 * V - 6) || v1 == -1) return false;
    v2 = graph[v1][p-1];
    for (int i = 0; i < p-2; i++) {
        int vn = graph[v1][i];
        cout << "vn " << vn+1 << endl;
        if (!isTriangle(v1, v2, vn)) continue;

        cout << v1+1 << " " << v2+1 << " " << vn+1 << " form a triangle.\n\n";
        
        vector<int> pi = order(v1, v2, vn);
        if (!pi.size()) continue;

        cout << "Order" << endl;
        for (int j = 0; j < V; j++) vertex_map[i] = -1;
        for (int k = 0; k < V; k++) {
            cout << pi[k]+1 << " ";
            vertex_map[pi[k]] = k;
        }
        cout << endl;

        if (embed(pi)) return true;
    }
    return false;
}

int main()
{
    ios::sync_with_stdio(false);
    int vj;
    cin >> V;

    E = 0;
    for (int i = 0; i < V; i++) {
        for (int j = i+1; j < V; j++) {
            cin >> vj;
            if (vj == -1) continue;
            graph[i].pb(j);
            graph[j].pb(i);
            E++;
        }
    }

    clock_t start, stop;
    start = clock();
    cout << (recognize() ? "YES" : "NO") << endl;
    stop = clock();

    printElapsedTime(start, stop);

    return 0;
}