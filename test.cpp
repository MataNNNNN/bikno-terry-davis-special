#include <iostream>
#include <bitset>

using namespace std;

int main(int argc, char* argv[]) {
    int count = 0;
    for(int i = 1; i < argc; i++) {
        int j = 0;
        while(argv[i][j]) {
            cout << count++ << " " << bitset<8>(argv[i][j]) << argv[i][j++] << endl;
        }
        count++;
        cout << endl << endl;
    }
    return 0;
}