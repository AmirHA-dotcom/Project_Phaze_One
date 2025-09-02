//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_VCCS_H
#define PROJECT_PHAZE_ONE_VCCS_H

#include "Element.h"


class VCCS : public Element
{
private:
    std::shared_ptr<Node> ctrl_node1;
    std::shared_ptr<Node> ctrl_node2;
public:
    VCCS() : Element("default", Element_Type::VC_Current_Source, nullptr, nullptr, 0.0) {}
    VCCS(string _name,shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value, std::shared_ptr<Node> ctrl1, std::shared_ptr<Node> ctrl2) :
    Element(_name, Element_Type::VC_Current_Source, _node1, _node2, _value) {ctrl_node1 = ctrl1; ctrl_node2 = ctrl2;}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    double get_current(double time, double time_step) override;

    pair<std::shared_ptr<Node>, std::shared_ptr<Node>> get_dependent_nodes() { return {ctrl_node1, ctrl_node2}; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Element>(this));
    }
};
CEREAL_REGISTER_TYPE(VCCS);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, VCCS);


#endif //PROJECT_PHAZE_ONE_VCCS_H
