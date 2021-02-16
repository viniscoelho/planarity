#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

int main(){
    ios::sync_with_stdio(false);
    int n;
    cin >> n;
    srand(time(NULL));
    cout << n << endl;
    for ( int i = 0; i < n; i++ ){
        for ( int j = i+1; j < n; j++ ){
            cout << rand() % 200 << " ";
        }
        cout << endl;
    }
    return 0;
}