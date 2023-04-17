#include <bits/stdc++.h>
#include <fstream>

using namespace std;

// Index size (M)
int* bimodal(int m, char *tracefile, vector<int> table) {

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(tracefile);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;
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

    int *ret = (int *)malloc(sizeof(int) * 2);
    ret[0] = predictions;
    ret[1] = mispredictions;
    return ret;
}