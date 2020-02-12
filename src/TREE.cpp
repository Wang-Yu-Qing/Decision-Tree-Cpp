#include "../include/Headers.h"
#include <stdexcept>
#include <cfloat>
#include <iostream>
#include <cmath>

using namespace std;

TREE::TREE(int min_node_size, int max_depth, string type, double impurity_thred)
{
    this->min_node_size = min_node_size;
    this->max_depth = max_depth;
    if (!((type == "cls") || (type == "reg")))
    {
        throw invalid_argument("Wrong tree type, type must be \"cls\" or \"reg\"");
    }
    this->type = type;
    if (this->type == "cls")
    {
        this->gini_thred_cls = impurity_thred;
    }
    else
    {
        this->gini_thred_reg = impurity_thred;
    }
};

TREE::~TREE(){};

void TREE::check_tree_structure(NODE *root)
{
    // pre-order traverse
    cout << "node split feature index: "
         << root->split_feature_index
         << " node split feature value: "
         << root->split_value
         << " node majority label: "
         << root->majority_y << endl;
    if (root->type == "leaf")
    {
        return;
    }
    this->check_tree_structure(root->left);
    this->check_tree_structure(root->right);
}

void TREE::fit(samples &train_data)
{
    this->root = this->grow(train_data, 0);
    cout << "Tree grow done!" << endl;
};

double TREE::predict(vec_d *sample)
{
    return this->make_predict(this->root, sample);
}

double TREE::make_predict(NODE *root, vec_d *sample)
{
    if (root->type == "leaf")
    {
        if (this->type == "cls")
        {
            return root->majority_y;
        }
        else
        {
            return root->average_y;
        }
    }
    // unlike traverse, select one way to go
    if ((*sample)[root->split_feature_index] < root->split_value)
    {
        return this->make_predict(root->left, sample);
    }
    else
    {
        return this->make_predict(root->right, sample);
    }
}

NODE *TREE::grow(samples &data, int current_depth)
{
    // don't make current depth as class member!
    // Pass the copy of current depth into next depth
    current_depth += 1;
    cout << "Current depth: " << current_depth
         << "  "
         << "Current sample size: " << data.size()
         << endl;
    // find majority label in the data set
    pair<double, int> majority = this->find_majority_y(data);
    // compute gini for the data set
    double gini = this->gini(data);
    // base condition
    if (this->is_leaf(data, current_depth, gini))
    {
        cout << "Grow terminated" << endl;
        NODE *node = new NODE(-1, -1);
        node->type = "leaf";
        node->majority_y = majority.first;
        return node;
    }
    // find optimal split
    split_info info = this->find_optimal_split(data);
    int split_feature_index = info.first;
    double split_value = info.second;
    // make the split with max gini
    splited_data splits = this->make_split(data, split_feature_index, split_value);
    // create node for this layer, using static to ensure node not destoried after function call
    NODE *node = new NODE(split_feature_index, split_value);
    node->type = "node";
    node->majority_y = majority.first;
    // recursively grow
    node->left = this->grow(splits[0], current_depth);
    node->right = this->grow(splits[1], current_depth);
    return node;
}

split_info TREE::find_optimal_split(samples &data)
{
    double min_gini = DBL_MAX;
    int split_feature_index = 0;
    double split_value = 0;
    int n = n_features(data);
    // iter through all features and its values to find split
    // with max gini
    for (int feature_index = 0; feature_index < n; feature_index++)
    {
        // get column unique values
        vec_d column_values = get_column_values(feature_index, data);
        set_d column_unique_values(column_values.begin(), column_values.end());
        for (double value : column_unique_values)
        {
            splited_data splits = this->make_split(data, feature_index, value);
            double gini = this->compute_weighted_gini(splits[0], splits[1]);
            if (gini < min_gini)
            {
                min_gini = gini;
                split_feature_index = feature_index;
                split_value = value;
            }
        }
    }
    cout << "Split feature index: " << split_feature_index << ", "
         << "Split feature value: " << split_value << endl;
    split_info info = make_pair(split_feature_index, split_value);
    return info;
}

splited_data TREE::make_split(samples &data, int feature_index, double feature_value)
{
    // store sub data's address
    samples left_data;
    samples right_data;
    // these address will not change even tough reallocate happened.
    for (vec_d *row_ptr : data)
    {
        vec_d &row = *row_ptr; // create a reference to the row
        if (row[feature_index] < feature_value)
        {
            left_data.push_back(&row); // push back row's address
        }
        else
        {
            right_data.push_back(&row);
        }
    }
    splited_data split = {left_data, right_data};
    return split;
}

pair<double, int> TREE::find_majority_y(samples &data)
{
    // count label
    map<double, int> label_count;
    for (vec_d *sample_ptr : data)
    {
        double label = (*sample_ptr).back();
        if (label_count.find(label) == label_count.end())
        {
            label_count[label] = 1; // key not found
        }
        else
        {
            label_count[label] += 1;
        }
    }
    // find label with biggest count
    int majority_count = 0;
    double majority_label = 0;
    for (auto const &k_v : label_count)
    {
        int count = k_v.second;
        double label = k_v.first;
        if (count > majority_count)
        {
            majority_count = count;
            majority_label = k_v.first;
        }
    }
    return make_pair(majority_label, majority_count);
}

double TREE::gini_cls(samples &data)
{
    if (data.size() == 0)
    {
        return 0;
    }
    pair<double, int> majority_label_count = this->find_majority_y(data);
    //  use majority label as predicted result,
    // the portion of samples can be correctly predicted (precision)
    double P = double(majority_label_count.second) / double(data.size());
    return P * (1 - P);
}

double TREE::compute_average_y(samples &data)
{
    double average = 0;
    for (vec_d *sample : data)
    {
        average += sample->back();
    }
    return average / data.size();
}

double TREE::gini_reg(samples &data)
{
    if (data.size() == 0)
    {
        return 0;
    }
    // user average y as predicted result
    double average_y = this->compute_average_y(data);
    // compute SSE
    double sse = 0;
    for (vec_d *sample : data)
    {
        sse += pow(sample->back() - average_y, 2);
    }
    return sse;
}

double TREE::gini(samples &data)
{
    if (this->type == "cls")
    {
        return this->gini_cls(data);
    }
    else
    {
        return this->gini_reg(data);
    }
}

double TREE::compute_weighted_gini(samples &data_left, samples &data_right)
{
    double left_gini, right_gini;
    left_gini = this->gini(data_left);
    right_gini = this->gini(data_right);
    // weighed average
    int total_size = data_left.size() + data_right.size();
    double gini_value = (left_gini * data_left.size() + right_gini * data_right.size()) / total_size;
    return gini_value;
}

bool TREE::same_labels(samples &data)
{
    set<double> unique_labels;
    for (vec_d *sample : data)
    {
        unique_labels.insert((*sample).back());
    }
    if (unique_labels.size() == 1)
    {
        return true;
    }
    return false;
}

bool TREE::is_leaf(samples &data, int current_depth, double gini)
{
    // check gini
    if (this->type == "cls")
    {
        if (gini < this->gini_cls(data))
        {
            return true;
        }
    }
    else
    {
        if (gini < this->gini_reg(data))
        {
            return true;
        }
    }
    // gini is ok, check other condition
    // once a condition is true, won't compute following coditions
    if ((current_depth == this->max_depth) || // reach max depth
        (data.size() < this->min_node_size))  // reach min node size
        return true;
    // all conditions not met
    return false;
}