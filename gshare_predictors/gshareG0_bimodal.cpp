#include <bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

int main(int args, char* argv[]) {
    if (args < 3) {
        cout << "Usage: ./a.out <M2> <tracefile>" << endl;
        return 0;
    }
    // Index size (M)
    int m = atoi(argv[1]);

    vector<int> table(1 << m, 4);

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(argv[2]);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;
    double mis_rate = 0;
    int pc = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        actual_taken = (line[7] == 't');

        // Get PC 
        pc = stoi(line.substr(0,6), 0, 16);

        int index = (pc >> 2) % (1 << m);

        // Predict and update
        int value = table[index];
        pred_taken = (value >= 4);
        
        if (actual_taken && value < 7)
           table[index]++;

        else if (!actual_taken && value > 0)
            table[index]--;
        
        predictions++;
        if (actual_taken != pred_taken) mispredictions++;
    }
    InFile.close();

    mis_rate = (100.0 * mispredictions) / predictions;

    // Output formatted
    cout.precision(4);
    cout << setw(30) << left << "number of predictions: " << setw(8) << predictions << endl;
    cout << setw(30) << "number of mispredictions: " << setw(8) << mispredictions << endl;
    cout << setw(30) << "misprediction rate:" << setw(4) << fixed << setprecision(2) << mis_rate << "%" << endl;
    for (int i = 0; i < (1 << m); i++) {
        cout << i << "  " << table[i] << endl;
    }

    return 0;
}