#pragma once
#include "Types.h"

using namespace std;

// data util functions
csv_result read_csv(string file_path);
string join_strings_in_vector(vec_s &vec, string &delimma);
void write_csv(string file_path, vec_s rows);
void show_data(vec_vec_s &data);
vec_d get_column_values(int col_index, samples &data);
void show_column_values(vec_d&);
int n_features(samples &data);

// print util functions
void print(string info);

// classes
class NODE
{
public:
    // members
    NODE *right;
    NODE *left;
    int split_feature_index;
    double split_value;
    string type;
    double majority_y;  // for classification
    double average_y;  // for regression
    // functions
    NODE(int split_feature_index, double split_value);
    ~NODE();
};

class TREE
{
public:
    // members
    NODE* root;
    int min_node_size;
    int max_depth;
    double gini_thred_cls;
    double gini_thred_reg;
    string type;
    // functions
    TREE(int min_node_size, int max_depth, string type, double impurity_thred=0.005);
    ~TREE();
    void check_tree_structure(NODE* root);
    void fit(samples &train_data);
    NODE* grow(samples &data, int current_depth);
    splited_data make_split(samples &data, int feature_index, double feature_value);
    pair<double, int> find_majority_y(samples &data);
    double compute_average_y(samples &data);
    double compute_weighted_gini(samples &data_left, samples &data_right);
    double gini(samples &data);
    double gini_cls(samples &data);
    double gini_reg(samples &data);
    bool same_labels(samples &data);
    bool is_leaf(samples &data, int current_depth, double gini);
    split_info find_optimal_split(samples &data);
    bool valid_split(splited_data &splits);
    double predict(vec_d *sample);
    double make_predict(NODE *root, vec_d *sample);
};

void traverse(NODE *root);
