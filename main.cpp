#include "include/Headers.h"
#include <iostream>

using namespace std;



int main()
{
    // prepare data
    csv_result iris = read_csv("data/iris.csv");
    vec_s *columns = iris.first;
    samples all_samples = iris.second;
    // init the tree
    // TREE model(20, 20, "cls");
    TREE model(20, 20, "reg", 20);
    // fit the tree
    model.fit(all_samples);
    model.check_tree_structure(model.root);
    cout << model.predict(all_samples[100]) << endl;
    return 1;
}