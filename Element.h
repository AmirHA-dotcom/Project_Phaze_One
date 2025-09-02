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
    shared_ptr<Node> node1;
    shared_ptr<Node> node2;
    double value;
    int x, y;
    Rotation rotation;
    int amplitude = 0;
public:
    Element(string _name, Element_Type _type, shared_ptr<Node> n1, shared_ptr<Node> n2, double _value);
    string get_name() const;
    Element_Type get_element_type();
    pair<shared_ptr<Node>, shared_ptr<Node>> get_nodes();
    double get_voltage_at_time(double time);
    double get_value() const {return value;}
    Element_Type get_type() const {return type;}
    virtual void display_info() = 0;
    virtual void change_value(double new_value) = 0;
    virtual void change_name(string new_name) = 0;
    virtual void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) = 0;
    virtual double get_current(double time, double time_step) = 0;
    void replace_node(shared_ptr<Node> node_to_merge, shared_ptr<Node> node_to_keep);

    void set_coordinates(int x, int y) { this->x = x; this->y = y; }
    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_rotation_as_int ();
    void set_rotation_by_int (int r);
    void set_rotation(Rotation r) { rotation = r; }
    virtual double getAmplitude() const { return amplitude; }
    virtual ~Element() = default;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(name, type, value, x, y, rotation, amplitude);
        // برای nodeها فقط شناسه‌شون یا index ذخیره بشه، نه خود pointer
        ar(node1->get_index(), node2->get_index()); // فرض بر اینه که get_index وجود داره
    }
};


#endif //PROJECT_PHAZE_ONE_ELEMENT_H
