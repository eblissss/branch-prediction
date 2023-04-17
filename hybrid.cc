#include <bits/stdc++.h>
#include <fstream>

using namespace std;

int* hybrid(int k, int m_1, int n, int m_2, char *tracefile,
            vector<int> table_g, vector<int> table_b, vector<uint8_t> chooser_table) {
    
    int bhr_largest_bit = 1 << (n - 1);

    bool actual_taken;
    bool pred_taken_g, pred_taken_b, pred_taken_h;

    // File
    ifstream InFile(tracefile);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;

    int bhr = 0;
    int pc = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        actual_taken = (line[7] == 't');

        // Get PC (without last 2 bits) 
        pc = stoi(line.substr(0,6), 0, 16);
        pc >>= 2;

        // Gshare Prediction
        // Explained in gshare.cc
        int index_A = (((pc >> n) << n) % (1 << m_1));
        int index_B = (pc % (1 << n)) ^ bhr;
        int index_g = index_A + index_B;

        int value_g = table_g[index_g];
        pred_taken_g = (value_g >= 4);

        // Update BHR
        bhr >>= 1;
        if (actual_taken)
            bhr += bhr_largest_bit;

        // Bimodal Prediction
        int index_b = (pc) % (1 << m_2);
        int value_b = table_b[index_b];
        pred_taken_b = (value_b >= 4);


        // Hybrid Prediction
        int index_choose = pc % (1 << k);
        int value_h = chooser_table[index_choose];

        bool gshare_selected = false;
        if (value_h >= 2) {
            pred_taken_h = pred_taken_g;
            gshare_selected = true;
        } else {
            pred_taken_h = pred_taken_b;
        }

        // UPDATE GSHARE
        if (gshare_selected) {
            if (actual_taken && value_g < 7)
                table_g[index_g]++;
            else if (!actual_taken && value_g > 0)
                table_g[index_g]--;
        }
        // UPDATE BIMODAL
        else { 
            if (actual_taken && value_b < 7)
                table_b[index_b]++;
            else if (!actual_taken && value_b > 0)
                table_b[index_b]--;
        }

        // Gshare is correct, Bimodal is wrong
        if (actual_taken == pred_taken_g && actual_taken != pred_taken_b) {
            chooser_table[index_choose] = min(3, value_h + 1);
        // Bimodal is correct, Gshare is wrong
        } else if (actual_taken == pred_taken_b && actual_taken != pred_taken_g) {
            chooser_table[index_choose] = max(0, value_h - 1);
        }

        predictions++;
        if (actual_taken != pred_taken_h) mispredictions++;
    }
    InFile.close();

    int *ret = (int *)malloc(sizeof(int) * 2);
    ret[0] = predictions;
    ret[1] = mispredictions;
    return ret;
}