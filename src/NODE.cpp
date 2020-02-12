#include "../include/Headers.h"

using namespace std;

NODE::NODE(int split_feature_index, double split_value)
{
    this->split_feature_index = split_feature_index;
    this->split_value = split_value;
}

NODE::~NODE() {}
