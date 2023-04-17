#include <bits/stdc++.h>
using namespace std; 
#pragma once

int* smith(int num_bits, char *tracefile);

int* bimodal(int m, char *tracefile, vector<int> table);

int* gshare(int m, int n, char *tracefile, vector<int> table);

int* hybrid(int k, int m_1, int n, int m_2, char *tracefile,
            vector<int> table_g, vector<int> table_b, vector<uint8_t> chooser_table);