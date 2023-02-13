// Ethan Bliss

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

int main(int args, char* argv[]) {
    int num_bits = 3;
    int largest_bit = 1 << (num_bits - 1);
    int counter = largest_bit;

    bool actual_taken;
    bool pred_taken;

    // File
    if (args < 2) {
        cout << "Usage: ./smith [filename]" << endl;
        return 0;
    }
    ifstream InFile(argv[1]);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;
    double mis_rate = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        actual_taken = (line[7] == 't');
        pred_taken = (counter >= largest_bit);

        if (actual_taken && counter < 7)
            counter++;
        else if (!actual_taken && counter > 0)
            counter--;

        predictions++;
        if (actual_taken != pred_taken) mispredictions++;
    }

    mis_rate = (100.0 * mispredictions) / predictions;

    // Output formatted
    cout.precision(4);
    cout << setw(30) << left << "number of predictions: " << setw(8) << predictions << endl;
    cout << setw(30) << "number of mispredictions: " << setw(8) << mispredictions << endl;
    cout << setw(30) << "misprediction rate:" << setw(5) << mis_rate << "%" << endl;

    return 0;
}