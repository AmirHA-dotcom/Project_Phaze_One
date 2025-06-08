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

int Circuit::element_index_finder_by_name(const string& name)
{
    for (int i = 0; i < Elements.size(); i++)
    {
        if (Elements[i]->get_name() == name)
            return i;
    }
    return -1;
}

void Circuit::change_name(string new_name)
{
    name = new_name;
}

string Circuit::get_name() const
{
    return name;
}

const vector<Element *> Circuit::get_Elements()
{
    return Elements;
}

void Circuit::change_name_of_node(string old_name, string new_name)
{
    int node_index = node_index_finder_by_name(old_name);
    if(node_index == -1)
    {
        cout << "Node not found" << endl;
        return;
    }

}

const vector<Node *> Circuit::get_Nodes()
{
    return Nodes;
}

const vector<Element *> Circuit::get_Elements_of_type(Element_Type type)
{
    vector<Element*> elements;
    for (auto* e : Elements)
        if (e->get_type() == type)
            elements.push_back(e);
    return elements;
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

void Circuit::change_value_of_element(string name, double value)
{
    int element_index = element_index_finder_by_name(name);
    if (element_index == -1)
    {
        cerr << "ELEMENT doesnt exist" << endl;
        return;
    }
    Elements[element_index]->change_value(value);
}

void Circuit::change_name_of_element(string old_name, string new_name)
{
    int element_index = element_index_finder_by_name(old_name);
    if (element_index == -1)
    {
        cerr << "ELEMENT doesnt exist" << endl;
        return;
    }
    Elements[element_index]->change_name(new_name);
}

void Circuit::make_node_ground(string name)
{
    int node_index = node_index_finder_by_name(name);
    if (node_index == -1)
    {
        cerr << "node NOt found" << endl;
        return;
    }
    Nodes[node_index]->make_ground();
}

void Circuit::make_node_NOT_ground(string name)
{
    int node_index = node_index_finder_by_name(name);
    if (node_index == -1)
    {
        cerr << "node NOt found" << endl;
        return;
    }
    Nodes[node_index]->return_to_normal();
}

void Circuit::create_new_resistor(string name, string node1_name, string node2_name, double resistance)
{
    // finding if we need to make a new node
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    // if we need new nodes
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    // again getting the indexes
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    // connection logic
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    // actual creation
    Elements.push_back(new Resistor(name, Nodes[node1_index], Nodes[node2_index], resistance));
}

void Circuit::create_new_capacitor(string name, string node1_name, string node2_name, double capacitance)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Capacitor(name, Nodes[node1_index], Nodes[node2_index], capacitance));
}
void Circuit::create_new_inductor(string name, string node1_name, string node2_name, double inductance)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], inductance));
}

void Circuit::create_new_voltage_source(string name, string node1_name, string node2_name, double voltage)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], voltage));
}

void Circuit::create_new_current_source(string name, string node1_name, string node2_name, double current)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], current));
}

void Circuit::create_new_VCCS(string name, string node1_name, string node2_name, double gain)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], gain));
}

void Circuit::create_new_CCCS(string name, string node1_name, string node2_name, double gain)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], gain));
}

void Circuit::create_new_VCVS(string name, string node1_name, string node2_name, double gain)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], gain));
}

void Circuit::create_new_CCVS(string name, string node1_name, string node2_name, double gain)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], gain));
}

void Circuit::create_new_real_diode(string name, string node1_name, string node2_name, double dummy_number)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], dummy_number));
}

void Circuit::create_new_zener_diode(string name, string node1_name, string node2_name, double dummy_number)
{
    int node1_index = node_index_finder_by_name(node1_name);
    int node2_index = node_index_finder_by_name(node2_name);
    if (node1_index == -1)
        Nodes.push_back(new Node(node1_name));
    if (node2_index == -1)
        Nodes.push_back(new Node(node2_name));
    node1_index = node_index_finder_by_name(node1_name);
    node2_index = node_index_finder_by_name(node2_name);
    Nodes[node1_index]->connect_element();
    Nodes[node2_index]->connect_element();
    Elements.push_back(new Inductor(name, Nodes[node1_index], Nodes[node2_index], dummy_number));
}

void Circuit::delete_element(string name)
{
    int element_index = -1;
    for (int i = 0; i < Elements.size(); i++)
    {
        if (name == Elements[i]->get_name())
        {
            element_index = i;
            break;
        }
    }
    if (element_index == -1)
    {
        cerr << "Element '" << name << "' NOT found!" << endl;
        return;
    }
    auto delete_node_if_unused = [this](Node* node){
        if (node->connected_elements_count() == 0)
        {
            auto it = find(Nodes.begin(), Nodes.end(), node);
            if (it != Nodes.end())
            {
                delete *it;
                Nodes.erase(it);
            }
        }
    };
    Element* element_to_delete = Elements[element_index];
    pair<Node*, Node*> nodes_pair = element_to_delete->get_nodes();
    Node* node1 = nodes_pair.first;
    Node* node2 = nodes_pair.second;

    node1->disconnect_element();
    node2->disconnect_element();

    delete_node_if_unused(node1);

    // dont delete twice.
    if (node1 != node2)
    {
        delete_node_if_unused(node2);
    }

    delete element_to_delete;
    Elements.erase(Elements.begin() + element_index);
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

//  checks the convergencr of NR
bool check_convergence(const vector<double>& b, double tolerance = 1e-9)
{
    double norm = 0.0;
    for (double val : b)
    {
        norm += abs(val);
    }
    return norm < tolerance;
}

void Circuit::transient()
{
    const int max_NR_its = 100;

    vector<double> x_previous(total_unknowns, 0.0);

    // outer loop that goes in time
    for (double t = t_start; t < t_end; t += time_step)
    {
        // the best first guess for the current time step's solution is the previous one
        vector<double> x_k = x_previous;
        bool converged = false;

        // this inner loop solves the circuits int a single time (NR)
        for (int k = 0; k < max_NR_its; ++k)
        {
            // making the linearized system using the stamps
            vector<Triplet> triplets;
            vector<double>  b(total_unknowns, 0.0);

            for (auto* e : Elements)
            {
                e->stamp(time_step, triplets, b, x_k, x_previous);
            }

            // making G
            vector<vector<double>> G(total_unknowns, vector<double>(total_unknowns, 0.0));
            for (const auto& tr : triplets)
            {
                G[tr.Row][tr.Column] += tr.Value;
            }

            // checking for convergence _ if the sum of all current errors is close to zero thats the answer
            if (check_convergence(b))
            {
                converged = true;
                break;
            }

            // we solve for delta_x so it will possibly converge
            for(double& val : b) { val = -val; }

            vector<double> delta_x = algorithems.solve_LU(G, b);

            if (delta_x.empty())
            {
                break;
            }

            // guess of the solution
            for(int i = 0; i < x_k.size(); ++i)
            {
                x_k[i] += delta_x[i];
            }
        }
        // if it didnt converge in time the circuit is not solvable(not my problem :)!)
        if (!converged)
        {
            cout << "Error: Newton-Raphson failed to converge at time t = " << t << endl;
            return;
        }

        // updating history
        x_previous = x_k;

        // saving data in the time
        for (auto* n : Active_Nodes)
        {
            n->set_voltage(x_previous[n->get_index()], t);
        }
    }
    cout << "transient worked! DEBUG" << endl;
}