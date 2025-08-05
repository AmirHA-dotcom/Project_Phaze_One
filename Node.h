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
    int x = 0;
    int y = 0;  // net label
public:
    Node(string _name);
    string get_name() const;
    void change_name(string new_name);
    void set_voltage(double volt, double time);
    vector<pair<double, double>> get_all_voltages();
    double get_voltage_in_time(double time);
    void make_ground();
    void return_to_normal();
    bool is_the_node_ground();
    void set_index(int i);
    int get_index();
    void connect_element();
    void disconnect_element();
    int connected_elements_count() const;
    void reset_voltages();

    void set_coordinates(int x_, int y_);
    pair<int, int> get_coordinates() { return {x, y}; }

    string net_name;
};


#endif //PROJECT_PHAZE_ONE_NODE_H
