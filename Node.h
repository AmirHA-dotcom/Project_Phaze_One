//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_NODE_H
#define PROJECT_PHAZE_ONE_NODE_H

#include "Libraries.h"

class Node
{
private:
    string name;
    vector<pair<double, double>> voltage;   // voltage, time
    bool is_ground;
    int elements_connected_count;
    int index;
public:
    Node(string _name);
    string get_name() const;
    void set_voltage(double volt, double time);
    double get_voltage_in_time(double time);
    void make_ground();
    void return_to_normal();
    bool is_the_node_ground();
    void set_index(int i);
    int get_index();
    void connect_element();
    void disconnect_element();
    int connected_elements_count() const;
};


#endif //PROJECT_PHAZE_ONE_NODE_H
