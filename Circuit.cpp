//
// Created by amirh on 2025-06-03.
//

#include "Circuit.h"

int Circuit::node_index_finder_by_name(const string& name)
{
    for (int i = 0; i < Nodes.size(); i++)
    {
        if (Nodes[i]->get_name() == name)
            return i;
    }
    return -1;
}

vector<Element *> Circuit::get_Elements()
{
    return Elements;
}

void Circuit::set_time_step(double ts)
{
    time_step = ts;
}

void Circuit::set_time_start(double ts)
{
    t_start = ts;
}

void Circuit::set_time_end(double ts)
{
    t_end = ts;
}

void Circuit::create_new_resistor(string name, string node1_name, string node2_name, double resistance)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1 || node2_index == -1)
    {
        cout << "One or both of the nodes can`t be found !!!" << endl;  // later add exception
        return;
    }
    Elements.push_back(new Resistor(name, Nodes[node1_index], Nodes[node2_index], resistance));
}

void Circuit::create_new_capacitor(string name, string node1_name, string node2_name, double capacitance)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1 || node2_index == -1)
    {
        cout << "One or both of the nodes can`t be found !!!" << endl;  // later add exception
        return;
    }
    Elements.push_back(new Capacitor(name, Nodes[node1_index], Nodes[node2_index], capacitance));
}
void Circuit::create_new_inductor(string name, string node1_name, string node2_name, double inductance)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1 || node2_index == -1)
    {
        cout << "One or both of the nodes can`t be found !!!" << endl;  // later add exception
        return;
    }
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], inductance));
}

void Circuit::create_new_node()
{
    string name = "DooDooL"; // should be change and add a counting logic
    Nodes.push_back(new Node(name));
}

void Circuit::analyse_data()
{
    // getting non_ground nodes
    vector<Node*> Active_Nodes;
    for (int i = 0; i < Nodes.size(); i++)
    {
        if (!Nodes[i]->is_the_node_ground())
            Active_Nodes.push_back(Nodes[i]);
    }
    // indexing nodes
    for (int i = 0; i < Active_Nodes.size(); i++)
        Active_Nodes[i]->set_index(i);
    // indexing aux
    int aux_index = Active_Nodes.size();
    for (auto* e : Elements)
    {
        if (auto* L = dynamic_cast<Inductor*>(e))
            L->set_aux_index(aux_index++);
        else if (auto* V = dynamic_cast<Voltage_Source*>(e))
            V->set_aux_index(aux_index++);
    }
    for (auto* e : Elements)
    {
        if (auto* Dv = dynamic_cast<VCVS*>(e))
            Dv->set_aux_index(aux_index++);
    }
    int total_unknowns = aux_index;
    // saving data
    this-> Active_Nodes = Active_Nodes;
    this-> total_unknowns = total_unknowns;
}

void Circuit::transient()
{
    // initials
    vector<double> x_previous(total_unknowns, 0.0);
    // the transient loop
    for (double t = t_start; t < t_end; t += time_step)
    {
        vector<Triplet> triplets;
        vector<double>  b(total_unknowns, 0.0);
        // stamping elementss
        for (auto* e : Elements)
            e->stamp(time_step, triplets, b, x_previous);
        // building the matrix
        vector<vector<double>> G(total_unknowns, vector<double> (total_unknowns, 0.0));
        for (auto tr : triplets)
            G[tr.Row][tr.Column] = tr.Value;
        // solve
        vector<double> x(total_unknowns, 0.0);
        x = algorithems.solve_cramer(G, b);
        // saving previous
        x_previous = x;
        // saving data
        for (auto* n : Active_Nodes)
            n->set_voltage(x_previous[n->get_index()], t);
    }
}

