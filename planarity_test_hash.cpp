/*
    A fast implementation of Nagamochi et al (2004) planarity test algorithm.
    The algorithm tests ONLY wether a graph is maximal planar or not.
    The following implementation uses hash.
*/

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <set>
#include <cstdio>
#include <ctime>
#define pb push_back
#define mp make_pair
#define MAX 5010

using namespace std;

typedef unsigned long long uint64;

/*
    Fast input
*/
bool read(int &n){
    n = 0;
    register bool neg = false;
    register char c = getchar_unlocked();
    if ( c == EOF ){ n = -1; return false; }
    while ( !('0' <= c && c <= '9') ){
        if( c == '-' ) neg = true;
        c = getchar_unlocked();
    }
    while ( '0' <= c && c <= '9' ){
        n = n * 10 + c - '0';
        c = getchar_unlocked();
    }
    n = (neg ? (-n) : (n));
    return true;
}

/*
    Number vertices and edges
*/
int N, M;

vector<int> planar[MAX];
int bib[MAX];
uint64 mat[81];

void printElapsedTime(clock_t start, clock_t stop){
    double elapsed = ((double)(stop - start)) / CLOCKS_PER_SEC;
    cout << fixed << setprecision(3) << "Elapsed time: " << elapsed << "s\n";
}

/*
    Hash functions
*/
bool find(int val){
    int pos = val/62;
    return mat[pos] & (1LL << (val % 63));
}

void insert(int val){
    mat[val/62] |= (1LL << (val % 63));
}

/*
    Check if the graph has a vertex with degree <= 5 and
    returns the first one found.
    Otherwise, returns -1
*/
int getVertex(){
    for ( int v = 0; v < N; v++ )
        if ( planar[v].size() <= 5 ) return v;
    return -1;
}

/*
    Check if three given vertices form a triangle
*/
bool isTriangle(int v1, int v2, int vn){
    int count = 0;
    for ( int i = 0; i < planar[v1].size(); i++ )
        if ( planar[v1][i] == v2 || planar[v1][i] == vn ) count++;

    for ( int i = 0; i < planar[v2].size(); i++ )
        if ( planar[v2][i] == v1 || planar[v2][i] == vn ) count++;

    return count == 4;
}

/*
    Given three vertices, returns a canonical order of V(G)
*/
vector<int> order(int v1, int v2, int vn){
    //vector<int> planar_[MAX];
    vector<int> VC, vi(N);
    //vi - vector with the output order of vertices
    vi[0] = v1; vi[1] = v2;
    VC.pb(v1); VC.pb(v2); VC.pb(vn);

    //x - number of elements in VC
    int x = VC.size(), pos = N-1;

    //set with remaining and removed vertices
    set<int> V, R;
    //insert all vertices but v1 and v2
    for ( int i = 0; i < N; i++ )
        if ( v1 != i && v2 != i ) V.insert(i);

    while ( !V.empty() ){
        int v, sz = -1;

        //choose a vertex v belonging to VC which is neither v1 nor v2
        //and which its neighbours' intersection with VC has size == 2.
        for ( int i = 0; i < x; i++ ){
            if ( VC[i] == v1 || VC[i] == v2 ) continue;
            v = VC[i];

            //intersection
            for ( int k = 0; k < 81; k++ ) mat[k] = 0;
            for ( int k = 0; k < VC.size(); k++ ){
                int at = VC[k]; 
                insert(at);
            }
            vector<int> aux;
            for ( int k = 0; k < planar[v].size(); k++ ){
                int at = planar[v][k];
                if ( find(at) && !R.count(at) ){
                    aux.pb(at);
                }
            }
            sz = aux.size();

            if ( sz != 2 ) continue;
            break;
        }
        //if such vertex does not exist, halt.
        if ( sz != 2 ) return vector<int>();

        //otherwise, remove this vertex from VC
        for ( int i = 0; i < x; i++ ){
            if ( VC[i] == v ){
                swap(VC[i], VC[x-1]);
                VC.pop_back();
                break;
            }
        }

        V.erase(v);
        R.insert(v);

        //and join VC with the chosen vertex's neighbours.
        //union
        vector<int> aux;
        for ( int k = 0; k < 81; k++ ) mat[k] = 0;
        for ( int k = 0; k < VC.size(); k++ ){
            int at = VC[k];
            insert(at);
            aux.pb(at);
        }
        for ( int k = 0; k < planar[v].size(); k++ ){
            int at = planar[v][k];
            if ( !find(at) && !R.count(at) ){
                aux.pb(at);
            }
        }
        x = aux.size();
        VC = aux;

        //add the chosen vertex to the answer
        vi[pos] = v; pos--;
    }

    return vi;
}

/*
    Check if a set of vertices appear consecutively into another sequence.
*/
bool areConsecutive(vector<int>& tmp, vector<int>& VC){
    int sz = VC.size(), t = tmp.size(), k = sz;
    
    if ( t > sz ) return false;

    //where is the lower bound occurence of an element on the intersection?
    for ( int k = 0; k < 81; k++ ) mat[k] = 0;
    for ( int i = 0; i < t; i++ ){
        insert(tmp[i]);
        k = min(k, bib[tmp[i]]);
    }

    if ( k+t > sz ) return false;

    for ( int i = 0; i < t; i++, k++ ){
        if ( !find(VC[k]) ) return false;
    }
    return true;
}

/*
    Check if the graph has a planar embedding
*/
bool embed(vector<int>& pi){
    vector<int> VC;
    VC.pb(pi[0]); VC.pb(pi[2]); VC.pb(pi[1]);

    for ( int i = 3; i < N; i++ ){
        //sublist {u_p, u_p+1, ..., u_p+n} = {v1, ..., v_i-1} inter NG(vi)
        //note: optimize this later...

        //intersection
        //node *inter = NULL;
        for ( int k = 0; k < 81; k++ ) mat[k] = 0;
        for ( int k = 0; k < i; k++ ){
            insert(pi[k]);
        }
        vector<int> tmp;
        for ( int k = 0; k < planar[pi[i]].size(); k++ ){
            int at = planar[pi[i]][k];
            if ( find(at) ){
                tmp.pb(at);
            }
        }

        for ( int j = 0; j < VC.size(); j++ )
            bib[VC[j]] = j;

        //where is the lower and higher bound occurences of the elements
        //on the intersection?
        int lb = VC.size(), hb = -1;
        for ( int i = 0; i < tmp.size(); i++ ){
            lb = min(lb, bib[tmp[i]]);
            hb = max(hb, bib[tmp[i]]);
        }

        if ( areConsecutive(tmp, VC) ){
            vector<int> cons;
            int k = 0;
            while ( k < lb ) cons.pb(VC[k++]);
            cons.pb(VC[lb]);
            cons.pb(pi[i]);
            cons.pb(VC[hb]);
            while ( k <= hb ) k++;
            while ( k < VC.size() ) cons.pb(VC[k++]);

            VC = cons;
        }
        else return false;
    }
    return true;
}

/*
    Recognizes if a given graph is either maximal planar or not
*/
bool recognize(){
    int v1 = getVertex(), v2, p = planar[v1].size();
    if ( M != (3*N-6) || v1 == -1 ) return false;
    v2 = planar[v1][p-1];
    for ( int i = 0; i < p-2; i++ ){
        int vn = planar[v1][i];
        cout << "vn " << vn+1 << endl;
        if ( !isTriangle(v1, v2, vn) ) continue;

        cout << v1+1 << " " << v2+1 << " " << vn+1 << " form a triangle.\n\n";
        
        vector<int> pi = order(v1, v2, vn);
        if ( !pi.size() ) continue;

        cout << "Order" << endl;
        for ( int j = 0; j < N; j++ ) bib[i] = -1;
        for ( int k = 0; k < N; k++ ){
            cout << pi[k]+1 << " ";
            bib[pi[k]] = k;
        }
        cout << endl;

        if ( embed(pi) ) return true;
    }
    return false;
}

int main(){
    //ios::sync_with_stdio(false);
    int vj;
    clock_t start, stop;
    start = clock();
    read(N);
    
    M = 0;
    int W = 0;
    for ( int i = 0; i < N; i++ ){
        for ( int j = i+1; j < N; j++ ){
            read(vj);
            if ( vj == -1 ) continue;
            W += vj;
            planar[i].pb(j);
            planar[j].pb(i);
            M++;
        }
    }

    puts(recognize() ? "YES" : "NO");
    cout << "Weight found: " << W << "\n";
    stop = clock();

    printElapsedTime(start, stop);
    
    return 0;
}