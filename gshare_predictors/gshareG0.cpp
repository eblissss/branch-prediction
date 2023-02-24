#include <bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int main(int args, char* argv[]) {
    if (args < 4) {
        cout << "Usage: ./a.out <M1> <N> <tracefile>" << endl;
        return 0;
    }
    // Index size (M)
    int m = atoi(argv[1]);
    // Number of bits in the history register (N)
    int n = atoi(argv[2]);
    
    int bhr_largest_bit = 1 << (n - 1);

    vector<int> table(1 << m, 4);

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(argv[3]);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;
    double mis_rate = 0;
    int bhr = 0;
    int pc = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        actual_taken = (line[7] == 't');

        // Get PC 
        pc = stoi(line.substr(0,6), 0, 16);
        // Remove right 2 bits
        pc >>= 2;

        // Removing last 2 bits, and using m left
        // Adding N-M 0's to bhr
        // XOR
        // n from bhr
        // n from PC
        // m-n from PC
        // shift all of the above until they all have m bits
        
        // M to N bits of shifted PC
        // n = 3, m = 6
        // 11111111111111111 Original
        // 00011111111111111 Shift Right by N
        // 11111111111111000 Shift Left By N (now right N bits are 0)
        //           1000000 2^M
        // 00000000000111000 Mod by 2^M (bits before Mth place are 0)
        //            ^^^

        int index_A = (((pc >> n) << n) % (1 << m));
        // Rightmost N bits of shifted PC (XOR with bhr)
        int index_B = (pc % (1 << n)) ^ bhr;
        
        int index = index_A + index_B;

        // Predict and update
        int value = table[index];
        pred_taken = (value >= 4);
        
        if (actual_taken && value < 7)
           table[index]++;

        else if (!actual_taken && value > 0)
            table[index]--;

        bhr >>= 1;
        if (actual_taken)
            bhr += bhr_largest_bit;
        
        predictions++;
        if (actual_taken != pred_taken) mispredictions++;
    }
    InFile.close();

    mis_rate = (100.0 * mispredictions) / predictions;

    // Output formatted
    cout.precision(4);
    cout << setw(30) << left << "number of predictions: " << setw(8) << predictions << endl;
    cout << setw(30) << "number of mispredictions: " << setw(8) << mispredictions << endl;
    cout << setw(30) << "misprediction rate:" << setw(5) << mis_rate << "%" << endl;
    
    // for (int i = 0; i < (1 << m); i++) {
    //     cout << i << "  " << table[i] << endl;
    // }
    return 0;
}