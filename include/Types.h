#pragma once
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>

using namespace std;

typedef vector<vector<string>> vec_vec_s;
typedef vector<vector<double>> vec_vec_d;
typedef vector<string> vec_s;
typedef vector<int> vec_i;
typedef vector<double> vec_d;
typedef set<double> set_d;
typedef vector<vec_d*> samples;
typedef vector<samples> splited_data;
typedef pair<int, double> split_info;
typedef pair<vec_s*, samples> csv_result;  // use pointer to void make_pair() copy, save memory