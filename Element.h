//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_ELEMENT_H
#define PROJECT_PHAZE_ONE_ELEMENT_H

#include "Libraries.h"
#include "Node.h"

enum class Rotation{Right, Left, Up, Down};

class Element
{
protected:
    string name;
    Element_Type type;
    Node* node1;
    Node* node2;
    double value;
    int x, y;
    Rotation rotation;
public:
    Element(string _name, Element_Type _type, Node* n1, Node* n2, double _value);
    string get_name() const;
    Element_Type get_element_type();
    pair<Node*, Node*> get_nodes();
    double get_voltage_at_time(double time);
    double get_value() const {return value;}
    Element_Type get_type() const {return type;}
    virtual void display_info() = 0;
    virtual void change_value(double new_value) = 0;
    virtual void change_name(string new_name) = 0;
    virtual void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) = 0;
    virtual double get_current(double time, double time_step) = 0;
    void replace_node(Node* node_to_merge, Node* node_to_keep);

    void set_coordinates(int x, int y) { this->x = x; this->y = y; }
    int get_x() const { return x; }
    int get_y() const { return y; }
    void set_rotation(Rotation r) { rotation = r; }
};


#endif //PROJECT_PHAZE_ONE_ELEMENT_H
