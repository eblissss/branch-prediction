#include <bits/stdc++.h>
#include <fstream>
#include <iostream>

using namespace std;

//THIS FILE IS INCOMPLETE
//Things I have learned while writing this: I do not know c++. We need an algorithim that is Unsupervised!!!
//I will try to finish the implementation on 3/23/2023 or before the end of 3/24/2023
//Explination will be in the document. Will comment later.
//This document is an approach to Items 2 and 3

// complete branch perdictors
int* bimodal(int m, string tracefile) {

    vector<int> table(1 << m, 4);

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

int* gshare(int m, int n, string tracefile) {
    
    int bhr_largest_bit = 1 << (n - 1);

    vector<int> table(1 << m, 4);

    bool actual_taken;
    bool pred_taken;

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

    int *ret = (int *)malloc(sizeof(int) * 2);
    ret[0] = predictions;
    ret[1] = mispredictions;
    return ret;
}

int* smith(int num_bits, string tracefile) {


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


// segmented branch perdictors
int* bimodal_segmented(int m, string tracefile, int seg_len, int seg_offset) {

    vector<int> table(1 << m, 4);

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(tracefile);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;
    int pc = 0;

    int linecounter = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {

        linecounter++;

        if ((seg_offset <= linecounter) && (linecounter < (seg_offset + seg_len))){

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

    }

        
    InFile.close();

    int *ret = (int *)malloc(sizeof(int) * 4);
    ret[0] = predictions;
    ret[1] = mispredictions;
    ret[2] = seg_offset;
    ret[3] = seg_len + seg_offset;
    return ret;
}

int* gshare_segmented(int m, int n, string tracefile, int seg_len, int seg_offset) {
    
    int bhr_largest_bit = 1 << (n - 1);

    vector<int> table(1 << m, 4);

    bool actual_taken;
    bool pred_taken;

    // File
    ifstream InFile(tracefile);
    string line;

    // Stats
    int predictions = 0;
    int mispredictions = 0;

    int bhr = 0;
    int pc = 0;

    int linecounter = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        linecounter ++;

        if ((seg_offset <= linecounter) && (linecounter < (seg_offset + seg_len))){

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
    }
    InFile.close();

    int *ret = (int *)malloc(sizeof(int) * 4);
    ret[0] = predictions;
    ret[1] = mispredictions;
    ret[2] = seg_offset;
    ret[3] = seg_len + seg_offset;
    return ret;
}

int* smith_segmented(int num_bits, string tracefile, int seg_len, int seg_offset) {


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

    int linecounter = 0;

    // Find and predict at each branch
    while (getline(InFile, line)) {
        linecounter ++;

        if ((seg_offset <= linecounter) && (linecounter < (seg_offset + seg_len))){

            actual_taken = (line[7] == 't');
            pred_taken = (counter >= largest_bit);

            if (actual_taken && counter < 7)
                counter++;
            else if (!actual_taken && counter > 0)
                counter--;

            predictions++;
            if (actual_taken != pred_taken) mispredictions++;
        
        }
    }

    int *ret = (int *)malloc(sizeof(int) * 2);
    ret[0] = predictions;
    ret[1] = mispredictions;
    ret[2] = seg_offset;
    ret[3] = seg_len + seg_offset;
    return ret;
}

void autosqueeze(int segment_length, int iterations, int amount_traces, string path_to_trace){

// to implement squeezing
//for(int iteration = 0; iteration < iterations; iteration++)
//SQUEEZING HAS NOT YET BEEN IMPLEMENTED YET!!!!!

//basic branch perdictor fitting below
int perdictor_amount = 3; //amount of perdictors we're using
int segement_amount = amount_traces / segment_length; //amount of segments trace is divided into

// configure branch perdictor defualts
int bimodal_m = 6;

int gshare_m = 9;
int gshare_n = 3;

int smith_bits = 3;

// intialize biases for branch perdictors
float perdictor_bias[segement_amount][perdictor_amount];

cout << segement_amount;

// calculate bias for each section

int offset = 0; //inital offset for determining where in trace this segment begins

for(int segment = 0; segment <= segement_amount; segment++){
    
    int* bimodal_inital_bias = bimodal_segmented(bimodal_m, path_to_trace, segment_length, offset);
    int* gshare_inital_bias = gshare_segmented(gshare_m, gshare_n, path_to_trace, segment_length, offset);
    int* smith_inital_bias = smith_segmented(smith_bits, path_to_trace, segment_length, offset);

    perdictor_bias[segment][0] = 1 - (static_cast<float>(bimodal_inital_bias[1]) / static_cast<float>(bimodal_inital_bias[0]));
    perdictor_bias[segment][1] = 1 - (static_cast<float>(gshare_inital_bias[1]) / static_cast<float>(gshare_inital_bias[0]));
    perdictor_bias[segment][2] = 1 - (static_cast<float>(smith_inital_bias[1]) / static_cast<float>(smith_inital_bias[0]));

    // this is to visualize the parameters in the actual result array

    cout << "biases for Segment " << segment;
    cout << " bimodal, gshare, smith. ";
    cout << "\n\n";
    cout << perdictor_bias[segment][0];
    cout << "\n";
    cout << perdictor_bias[segment][1];
    cout << "\n";
    cout << perdictor_bias[segment][2];
    cout << "\n";

    offset = segment + offset;
    
}




}

void mv_autosqueeze(int segment_length, int iterations, int amount_traces, string paths_to_traces){

// to implement squeezing
//for(int iteration = 0; iteration < iterations; iteration++)

//SQUEEZING HAS NOT YET BEEN IMPLEMENTED YET!!!!!
//This version is diffrent!!! it takes in an array of string traces, and attempts to average biases across all traces!!
//This is basically the auto squeeze function implemented over a list of strings, averaging out biases over a bunch of trace files.
//To use this one we need multiple trace files, with their paths in a list of strings. Perferably with them all having the same amount of
//perdictions in them. 

//Initialization stuff
int perdictor_amount = 3; //amount of perdictors we're using
int segement_amount = amount_traces / segment_length; //amount of segments trace is divided into

// configure branch perdictor defualts
int bimodal_m = 6;

int gshare_m = 9;
int gshare_n = 3;

int smith_bits = 3;

// intialize biases for branch perdictors
float perdictor_bias[segement_amount][perdictor_amount];

cout << segement_amount;

// calculate bias for each section
for(int trace = 0; trace <= sizeof(paths_to_traces); trace++){

    int offset = 0; //inital offset for determining where in trace this segment begins

    for(int segment = 0; segment <= segement_amount; segment++){
        
        int* bimodal_inital_bias = bimodal_segmented(bimodal_m, paths_to_traces, segment_length, offset);
        int* gshare_inital_bias = gshare_segmented(gshare_m, gshare_n, paths_to_traces, segment_length, offset);
        int* smith_inital_bias = smith_segmented(smith_bits, paths_to_traces, segment_length, offset);

        if(trace == 0){

            perdictor_bias[segment][0] = 1 - (static_cast<float>(bimodal_inital_bias[1]) / static_cast<float>(bimodal_inital_bias[0]));
            perdictor_bias[segment][1] = 1 - (static_cast<float>(gshare_inital_bias[1]) / static_cast<float>(gshare_inital_bias[0]));
            perdictor_bias[segment][2] = 1 - (static_cast<float>(smith_inital_bias[1]) / static_cast<float>(smith_inital_bias[0]));
        
        }
        else{

            perdictor_bias[segment][0] = (perdictor_bias[segment][0] + (1 - (static_cast<float>(bimodal_inital_bias[1]) / static_cast<float>(bimodal_inital_bias[0]))))/2;
            perdictor_bias[segment][1] = (perdictor_bias[segment][1] + (1 - (static_cast<float>(gshare_inital_bias[1]) / static_cast<float>(gshare_inital_bias[0]))))/2;
            perdictor_bias[segment][2] = (perdictor_bias[segment][2] + (1 - (static_cast<float>(smith_inital_bias[1]) / static_cast<float>(smith_inital_bias[0]))))/2;

        }

        //this is to visualize the parameters in the actual result array

        cout << "biases for Segment " << segment;
        cout << " bimodal, gshare, smith. ";
        cout << "\n\n";
        cout << perdictor_bias[segment][0];
        cout << "\n";
        cout << perdictor_bias[segment][1];
        cout << "\n";
        cout << perdictor_bias[segment][2];
        cout << "\n";

        offset = segment + offset;
        
    }

    




}






}

int main(string path_to_tracefile){

    // For this to work, we need to know the aproximate total amount of traces (branches) in the program

    string tracefile = path_to_tracefile;
    int amount_traces = 2000000; //originally used gcc1
    cout << "testing \n";

    autosqueeze(100000, 1, amount_traces, tracefile);

    return 0;
}

