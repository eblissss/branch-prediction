// Cesar Hernandez

#include <iostream>
#include <fstream>
#include "string"
#include "math.h"
#include <bits/stdc++.h>
#include <iomanip>

using namespace std;

int main(int args, char* argv[]) {

    if (args <= 2){
        cout << "Usage Syntax: ./a.out <M2> <tracefile>" << endl;
        cout << "Where <M2> is an integer and <tracefile> is a valid file path." << endl;
    }

    // Create counter to count number of missed predictions and total predictions.
    int missedPrediction = 0;
    int totalPredictions = 0;

    // Open file to read.
    string fileName = R"(C:\Users\cmgh7\Desktop\Spring23_BranchPrediction\Spring23_BranchPrediction\traces\gcc_trace.txt)";//argv[2];
    ifstream file;
    file.open(fileName);
    string word;

    // Initialize M and N values.
    int M = std::atoi("6");//argv[1]);
    int N = 0;

    // Create table
    int mTable[static_cast<int>(pow(2, M))];

    // Initialize table to all values of two.
    for (int i = 0; i < static_cast<int>(pow(2, M)); ++i) {
        mTable[i] = 4;
    }

    // Set up GBH
    int GHB = 0;

    // Read file
    while (file >> word){

        // Get hex branch pc and outcome.
        int pc = stoi(word,nullptr,16);
        file >> word;
        string outcome = word;

        // XOR to get the index and prediction
        pc /= 4;
        int mIndex = pc % static_cast<int>(pow(2, M));
        int nExt = GHB << (M-N);
        int index = mIndex ^ nExt;

        // Get prediction from the computed index within the table.
        int prediction = mTable[index];

        // Compare prediction with outcome.
        // If the prediction is for taken but the outcome is not taken or
        //  the prediction is for not taken but the outcome is for taken,
        //  then increment the missed prediction counter.
        if(((prediction >= 4) && outcome == "n") || ((prediction < 4) && outcome == "t")){
            missedPrediction++;
        }

        // Increment the number of total predictions for every prediction made.
        totalPredictions++;

        // Shift global history buffer to the right by one bit to prepare for outcome evaluation.
        GHB = (GHB >> 1);

        // If the outcome is taken, add 2^(N - 1) to the global history buffer
        //  (N is the number of global history register bits used to index the gshare table)
        //  and increment the table index's prediction value by 1 as long as the value does
        //  not exced three.
        if (outcome == "t"){
            GHB += static_cast<int>(pow(2, static_cast<double >(N-1)));

            if(prediction != 7){
                mTable[index] = prediction + 1;
            }
        }
        // If the outcome is not taken, decrement the table index's prediction value by 1
        //  as long as the value does not go below zero.
        else{
            if(prediction != 0){
                mTable[index] = prediction - 1;
            }
        }
    }

    // Close file
    file.close();

    // Output results of prediction.
    cout.precision(4);
    cout << setw(30) << left << "# of Predictions: " << totalPredictions << endl;
    cout << setw(30) << "# of Misses: " << setw(8) << missedPrediction << endl;
    cout << setw(30) << "Missed Prediction Rate: " << setw(4) << fixed << setprecision(2) << (static_cast<double >(missedPrediction) / static_cast<double >(totalPredictions) * 100) << "%" << endl;

    cout << "FINAL BIMODAL CONTENTS" << endl;
    for (int i = 0; i < static_cast<int>(pow(2, M)); i++) {
        cout << i << " " << mTable[i] << endl;
    }

    return 0;
}
