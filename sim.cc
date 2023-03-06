#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
#include "sim.hh"

using namespace std;

int main(int argc, char* argv[]) {

    int *results;

    if (argc < 2) {
        cout << "Usage: sim <sim type> <sim args>" << endl;
        return 0;
    }

    if (strcmp(argv[1], "smith") == 0) {
        if (argc < 4) {
            cout << "Usage: sim smith <B> <tracefile>" << endl;
            return 0;
        }
        results = smith(stoi(argv[2]), argv[3]);
    }
    else if (strcmp(argv[1], "bimodal") == 0) {
        if (argc < 4) {
            cout << "Usage: sim bimodal <M2> <tracefile>" << endl;
            return 0;
        }
        results = bimodal(stoi(argv[2]), argv[3]);
    }
    else if (strcmp(argv[1], "gshare") == 0) {
        if (argc < 5) {
            cout << "Usage: sim gshare <M1> <N> <tracefile>" << endl;
            return 0;
        }
        results = gshare(stoi(argv[2]), stoi(argv[3]), argv[4]);
    }
    else if (strcmp(argv[1], "hybrid") == 0) {
        if (argc < 7) {
            cout << "Usage: sim hybrid <K> <M1> <N> <M2> <tracefile>" << endl;
            return 0;
        }
        results = hybrid(stoi(argv[2]), stoi(argv[3]), stoi(argv[4]), stoi(argv[5]), argv[6]);
    } else {
        cout << "Not a valid sim type! Choose from smith, bimodal, gshare, or hybrid" << endl;
        return 0;
    }
    

    // Stats
    int predictions = results[0];
    int mispredictions = results[1];
    double mis_rate = (100.0 * mispredictions) / predictions;

    // Output formatted
    cout.precision(4);
    cout << setw(30) << left << "number of predictions: " << setw(8) << predictions << endl;
    cout << setw(30) << "number of mispredictions: " << setw(8) << mispredictions << endl;
    cout << setw(30) << "misprediction rate:" << setw(4) << fixed << setprecision(2) << mis_rate << "%" << endl;
    return 0;
}