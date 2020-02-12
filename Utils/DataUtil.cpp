#include "../include/Headers.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

csv_result read_csv(string file_path)
{
    cout << "Read csv file from " << file_path << endl;
    ifstream file_stream(file_path);
    string line;
    // lines's scope is this function, if not returned, cannot user 
    // variable name "lines" to reach it outside the function
    // lines's lifetime ends till the whole program ends, can be 
    // reached by pointer outside the function
    static vec_s column;
    static vec_vec_d lines;  // using static to ensure vairable not destories after function call
    int line_index = 0;
    while (getline(file_stream, line))
    {
        string line_value;
        vec_d line_values;
        stringstream line_stream(line);
        // get column
        if (line_index == 0) {
            while (getline(line_stream, line_value, ',')){
                column.push_back(line_value);
            }
            line_index += 1;
            continue;
        }
        while (getline(line_stream, line_value, ','))
        {
            line_values.push_back(stod(line_value));
        }
        lines.push_back(line_values);
        line_index += 1;
    }
    file_stream.close();
    // get each line's address in line
    samples all_samples;
    // must iter by reference, 
    // otherwise the address is copied object's address
    // will copied objec will be destroied after each loop
    for (vec_d &sample : lines) {
        all_samples.push_back(&sample);  // push address, ensure this address is always valid
    }
    return make_pair(&column, all_samples);
}

string join_strings_in_vector(vec_s &vec, string &delimma)
{
    string result = "";
    for (string value : vec)
    {
        result += (value + delimma);
    }
    result.pop_back(); //remove \n
    return result;
}

void write_csv(string file_path, vec_s rows)
{
    ofstream file_stream(file_path);
    for (string row_to_write : rows)
    {
        // cout << row_to_write << endl;
        file_stream << row_to_write;
    }
    file_stream.close();
}

void show_data(vec_vec_s &data)
{
    for (vec_s row : data)
    {
        string row_string = "";
        for (string value : row)
        {
            row_string += (value + ";");
        }
        cout << row_string << endl;
    }
}

vec_d get_column_values(int col_index, samples &data)
{
    // ensure that data contains no column names!
    vec_d column(static_cast<int>(data.size()));
    int j = 0;
    for (int i=0; i<data.size(); i++)
    {
        vec_d row = *(data[i]);
        double field_value = row[col_index];
        column[i] = field_value;
        j += 1;
    }
    return column;
}

void show_column_values(vec_d &column)
{
    for (double &value: column) {
        cout << value << endl;
    }
}

int n_features(samples &data)
{
    // data without lables
    return (*(data[0])).size() - 1;
}
