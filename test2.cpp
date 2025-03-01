#include <iostream>
#include <bitset>

using namespace std;

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        for(int j = 0; argv[i][j]; ) {
            if(argv[i][j] > 0) {
                cout << bitset<8>(argv[i][j]) << " " << argv[i][j++] << endl;
                continue;
            }

            wchar_t ch = 0;
            int count = 0;
            for(int t = (unsigned char)argv[i][j] >> 4; t > 0; t >>= 1) {
                if(t&1)
                    ch = (ch << 8) | (unsigned char)argv[i][j + count++];
                else {
                    count = 0;
                    ch = 0;
                }
            }
            wcout << bitset<32>(ch) << " " << ch << endl;
            j += count;
        }
    }
    return 0;
}