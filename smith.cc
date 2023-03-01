#include <fstream>

using namespace std;

int* smith(int num_bits, char *tracefile) {

    int largest_bit = 1 << (num_bits - 1);
    int counter = largest_bit;

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(tracefile);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;

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

    int *ret = (int *)malloc(sizeof(int) * 2);
    ret[0] = predictions;
    ret[1] = mispredictions;
    return ret;
}