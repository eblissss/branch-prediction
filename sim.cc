#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include <bits/stdc++.h>
#include <vector>
#include "sim.hh"

using namespace std;

void print_output(int *results);

int main(int argc, char* argv[]) {

    int *results;
    long unsigned int i;

    vector<int> table;
    vector<uint8_t> hybrid_chooser_table;
    vector<int> hybrid_table_b;
    vector<int> hybrid_table_g;

    // Print command back out
    cout << "COMMAND" << endl;
    for (int j = 0; j < argc - 1; j++) {
        cout << argv[j] << " ";
    }
    cout << argv[argc-1] << endl;
    cout << "OUTPUT" << endl;

    // Wrong input
    if (argc < 2) {
        cout << "Usage: sim <sim type> <sim args>" << endl;
        return 0;
    }

    // Run Smith
    if (strcmp(argv[1], "smith") == 0) {
        if (argc < 4) {
            cout << "Usage: sim smith <B> <tracefile>" << endl;
            return 0;
        }
        results = smith(stoi(argv[2]), argv[3]);

        // Print results
        print_output(results);
        cout << "FINAL COUNTER CONTENT:" << "\t\t" << results[2];
    }
    // Run Bimodal
    else if (strcmp(argv[1], "bimodal") == 0) {
        if (argc < 4) {
            cout << "Usage: sim bimodal <M2> <tracefile>" << endl;
            return 0;
        }
        table.assign (1 << stoi(argv[2]), 4);
        results = bimodal(stoi(argv[2]), argv[3], table);

        // Print results
        print_output(results);
        cout << "FINAL BIMODAL CONTENTS"<< endl;
        for (i = 0; i < table.size(); i++)
            cout << i << "\t" << results[i+2] << endl;
    }
    // Run Gshare
    else if (strcmp(argv[1], "gshare") == 0) {
        if (argc < 5) {
            cout << "Usage: sim gshare <M1> <N> <tracefile>" << endl;
            return 0;
        }
        table.assign (1 << stoi(argv[2]), 4);
        results = gshare(stoi(argv[2]), stoi(argv[3]), argv[4], table);

        // Print results
        print_output(results);
        cout << "FINAL GSHARE CONTENTS"<< endl;
        for (i = 0; i < table.size(); i++)
            cout << i << "\t" << results[i+2] << endl;
    }
    // Run Hybrid
    else if (strcmp(argv[1], "hybrid") == 0) {
        if (argc < 7) {
            cout << "Usage: sim hybrid <K> <M1> <N> <M2> <tracefile>" << endl;
            return 0;
        }
        hybrid_table_g.assign (1 << stoi(argv[3]), 4);
        hybrid_table_b.assign (1 << stoi(argv[5]), 4);
        hybrid_chooser_table.assign (1 << stoi(argv[2]), 1);
        results = hybrid(stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), argv[6],
                         hybrid_table_g, hybrid_table_b, hybrid_chooser_table);

        // Print results
        print_output(results);
        // Print tables
        int b = 0, g = 0;
        cout << "FINAL CHOOSER CONTENTS"<< endl;
        for (i = 0; i < hybrid_chooser_table.size(); i++)
        {
            cout << i << "\t" << results[i+2] << endl;
            g = i+1;
        }
        cout << "FINAL GSHARE CONTENTS"<< endl;
        for (i = 0; i < hybrid_table_g.size(); i++)
        {
            cout << i << "\t" << results[g + 2] << endl;
            g++;
            b = g;
        }
        cout << "FINAL BIMODAL CONTENTS"<< endl;
        for (i = 0; i < hybrid_table_b.size(); i++)
        {
            cout << i << "\t" << results[b+2] << endl;
            b++;
        }

    } else {
        cout << "Not a valid sim type! Choose from smith, bimodal, gshare, or hybrid" << endl;
        return 0;
    }

    return 0;
}

// Print formatted output
void print_output(int *results)
{
    // Stats
    int predictions = results[0];
    int mispredictions = results[1];
    double mis_rate = (100.0 * mispredictions) / predictions;

    // Output formatted
    cout.precision(4);
    cout << "number of predictions:" << "\t\t" << predictions << endl;
    cout << "number of mispredictions:" << "\t" << mispredictions << endl;
    cout << "misprediction rate:" << fixed << setprecision(2)  << "\t\t"<< mis_rate << "%" << endl;
}