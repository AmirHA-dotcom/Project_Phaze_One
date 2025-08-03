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
void Circuit::ground(bool b){
    this->haveGround = b;
}
bool Circuit::isGround(){
    return haveGround;
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
    Nodes[node_index]->change_name(new_name);
}

const vector<Node *> Circuit::get_Nodes()
{
    return Nodes;
}

const vector<Element *> Circuit::get_Elements_of_type(Element_Type type)
{
    if (type == Element_Type::Real_Diode)
    {
        vector<Element*> elements;
        for (auto* e : Elements)
            if (e->get_type() == Element_Type::Real_Diode || e->get_type() == Element_Type::Zener_Diode)
                elements.push_back(e);
        return elements;
    }
    else
    {
        vector<Element*> elements;
        for (auto* e : Elements)
            if (e->get_type() == type)
                elements.push_back(e);
        return elements;
    }
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
    haveGround = true;
}
void Circuit::checkHaveGround() {
    for (auto node : Nodes)
    {
        if (node->is_the_node_ground())
        {
            haveGround = true;
            return;
        }
    }
}

Element* Circuit::findElement (string name){
    for (auto& e : get_Elements())
    {
        if (e->get_name() == name)
            return e;
    }
    return nullptr;
}
Node* Circuit::findNode (string name){
    for (auto& n : get_Nodes())
    {
        if (n->get_name() == name)
            return n;
    }
    return nullptr;
}

void Circuit::make_node_NOT_ground(string name)
{
    int node_index = node_index_finder_by_name(name);
    if (node_index == -1)
    {
        cerr << "node Not found" << endl;
        return;
    }
    Nodes[node_index]->return_to_normal();
}

void Circuit::create_new_resistor(string name, string node1_name, string node2_name, double resistance)
{
    //cout << "ojn" << endl;
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
    Elements.push_back(new Current_Source(name, Nodes[node1_index], Nodes[node2_index], current));
}

void Circuit::create_new_VCCS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain)
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
    Elements.push_back(new VCCS(name, Nodes[node1_index], Nodes[node2_index], gain, Nodes[node_index_finder_by_name(ctrl1)], Nodes[node_index_finder_by_name(ctrl2)]));
}

void Circuit::create_new_CCCS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain)
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
    Elements.push_back(new CCCS(name, Nodes[node1_index], Nodes[node2_index], gain, Nodes[node_index_finder_by_name(ctrl1)], Nodes[node_index_finder_by_name(ctrl2)]));
}

void Circuit::create_new_VCVS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain)
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
    Elements.push_back(new VCVS(name, Nodes[node1_index], Nodes[node2_index], gain, Nodes[node_index_finder_by_name(ctrl1)], Nodes[node_index_finder_by_name(ctrl2)]));
}

void Circuit::create_new_CCVS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain)
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
    Elements.push_back(new CCVS(name, Nodes[node1_index], Nodes[node2_index], gain, Nodes[node_index_finder_by_name(ctrl1)], Nodes[node_index_finder_by_name(ctrl2)]));
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
    Elements.push_back(new Real_Diode(name, Nodes[node1_index], Nodes[node2_index], dummy_number));
    is_diode_added = true;
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
    Elements.push_back(new Zener_Diode(name, Nodes[node1_index], Nodes[node2_index], dummy_number));
    is_diode_added = true;
}

void Circuit::create_new_DC_voltage_source(std::string name, std::string node1_name, std::string node2_name, double voltage)
{
    //cout << "meow" << endl;
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
    Elements.push_back(new DC_Source(name, Nodes[node1_index], Nodes[node2_index],voltage));
    //cout << "ADDING DC S\n" << "node 1 name " << Nodes[node1_index]->get_name() << " node 2 name " << Nodes[node2_index]->get_name() << endl;
    //cout << Elements[Elements.size() - 1]->get_name() << endl;
}

void
Circuit::create_new_Sin_voltage_source(std::string name, std::string node1_name, std::string node2_name, double offset, double amplitude, double frequency)
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
    Elements.push_back(new Sine_Source(name, Nodes[node1_index], Nodes[node2_index],offset, amplitude, frequency, 0.0));
}

void Circuit::create_new_Pulse_voltage_source(std::string name, std::string node1_name, std::string node2_name, double period, double value)
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
    Elements.push_back(new Pulse_Source(name, Nodes[node1_index], Nodes[node2_index], 0.0, value, 0.0, period/100, period/100, 2, period));
}

void Circuit::create_new_Delta_voltage_source(std::string name, std::string node1_name, std::string node2_name, double time)
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
    Elements.push_back(new Delta_Dirac(name, Nodes[node1_index], Nodes[node2_index], time));
}

void Circuit::create_new_Square_voltage_source(string name, string node1_name, string node2_name, double period, double value)
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
    Elements.push_back(new Square_Source(name, Nodes[node1_index], Nodes[node2_index], -1 * value, value, 0.0, 2, period));
}

void Circuit::create_new_Triangle_voltage_source(string name, string node1_name, string node2_name, double period, double value)
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
    Elements.push_back(new Triangular_Source(name, Nodes[node1_index], Nodes[node2_index], 0.0, value, 0.0, period));
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

    auto delete_node_if_unused = [this](Node* node) {
        if (node->connected_elements_count() == 0)
        {
            auto it = std::find(Nodes.begin(), Nodes.end(), node);
            if (it != Nodes.end())
            {
                Nodes.erase(it);
                delete node;
            }
        }
    };

    Element* element_to_delete = Elements[element_index];

    pair<Node*, Node*> nodes_pair = element_to_delete->get_nodes();
    Node* node1 = nodes_pair.first;
    Node* node2 = nodes_pair.second;

    node1->disconnect_element();
    if (node1 != node2)
    {
        node2->disconnect_element();
    }

    Elements.erase(Elements.begin() + element_index);

    delete element_to_delete;

    delete_node_if_unused(node1);
    if (node1 != node2)
    {
        delete_node_if_unused(node2);
    }
//    for (auto& element : get_Elements())
//        cout << element->get_name() << endl;
}

vector<pair<double, double>> Circuit::get_node_voltages(string name)
{
    return Nodes[node_index_finder_by_name(name)]->get_all_voltages();
}

void Circuit::analyse_data()
{
    // getting non_ground nodes
    vector<Node*> Active_Nodes_;
    for (int i = 0; i < Nodes.size(); i++)
    {
        if (!Nodes[i]->is_the_node_ground())
            Active_Nodes_.push_back(Nodes[i]);
        else
            Nodes[i]->set_index(-1);
    }
    // indexing nodes
    for (int i = 0; i < Active_Nodes_.size(); i++)
        Active_Nodes_[i]->set_index(i);
    // indexing aux
    int aux_index = Active_Nodes_.size();
    for (auto* e : Elements)
    {
        if (auto* L = dynamic_cast<Inductor*>(e))
            L->set_aux_index(aux_index++);
        else if (auto* V = dynamic_cast<DC_Source*>(e))
            V->set_aux_index(aux_index++);
    }
    for (auto* e : Elements)
    {
        if (auto* Dv = dynamic_cast<VCVS*>(e))
            Dv->set_aux_index(aux_index++);
    }
    // saving data
    this-> Active_Nodes = Active_Nodes_;
    this-> total_unknowns = aux_index;
    //cout << "analyse_data finished. Active nodes found: " << this->Active_Nodes.size() << endl;
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

vector<double> multiply_matrix_vector(const vector<vector<double>>& A, const vector<double>& x)
{
    int n = A.size();
    vector<double> result(n, 0.0);
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            result[i] += A[i][j] * x[j];
        }
    }
    return result;
}

vector<double> subtract_vectors(const vector<double>& a, const vector<double>& b)
{
    int n = a.size();
    vector<double> result(n, 0.0);
    for (int i = 0; i < n; ++i)
    {
        result[i] = a[i] - b[i];
    }
    return result;
}

void Circuit::transient_NR()
{
    //cout << t_start << " " << time_step << " " << t_end << endl;
    const int max_NR_its = 100;

    vector<double> x_previous(total_unknowns, 0.0);

    // outer loop that goes in time
    for (double t = t_start; t < t_end; t += time_step)
    {
        //cout << "tran start" << endl;
        vector<double> x_k = x_previous;
        bool converged = false;

        for (int k = 0; k < max_NR_its; ++k)
        {
            //cout << "NR started" << endl;
            // stamping elements
            vector<Triplet> triplets;
            vector<double>  b_rhs(total_unknowns, 0.0);
            for (auto* e : Elements) {
                e->stamp(t, time_step, triplets, b_rhs, x_k, x_previous);
            }
            //cout << "\n--- Checking Element Types before Stamping (t=" << t << ", k=" << k << ") ---" << endl;
//            for (auto* e : Elements) {
//                if (e == nullptr) {
//                    cout << "FATAL: Found a nullptr Element!" << endl;
//                    continue;
//                }
//
//                cout << "Object '" << e->get_name() << "' at address " << e << ":" << endl;
//
//                // Ask the object what it *really* is
//                if (dynamic_cast<Resistor*>(e)) {
//                    cout << "  -> Type check: OK (Resistor). Calling stamp..." << endl;
//                    e->stamp(t, time_step, triplets, b_rhs, x_k, x_previous);
//                } else if (dynamic_cast<DC_Source*>(e)) {
//                    cout << "  -> Type check: OK (DC_Source). Calling stamp..." << endl;
//                    e->stamp(t, time_step, triplets, b_rhs, x_k, x_previous);
//                }
////                else if (dynamic_cast<DC_Source*>(e)) {
////                    cout << "  -> Type check: OK (Some other Voltage_Source). Calling stamp..." << endl;
////                    e->stamp(t, time_step, triplets, b_rhs, x_k, x_previous);
////                }
//                else {
//                    // This is what we expect to see for the broken object
//                    cout << "  -> Type check: FAILED! Object is of an UNKNOWN OR CORRUPTED TYPE." << endl;
//                    cout << "  -> NOT calling stamp() on this corrupted object." << endl;
//                }
//            }
            //cout << "--- Finished Stamping ---" << endl;
            //cout << "stamp works" << endl;
            vector<vector<double>> G(total_unknowns, vector<double>(total_unknowns, 0.0));
//            for (const auto& tr : triplets) {
//                G[tr.Row][tr.Column] += tr.Value;
//            }
//            std::cerr << "[DEBUG] t=" << t
//                      << ", total_unknowns=" << total_unknowns
//                      << ", triplets.size()=" << triplets.size() << endl;
            for (const auto& tr : triplets) {
                if (tr.Row < 0 || tr.Row >= total_unknowns ||
                    tr.Column < 0 || tr.Column >= total_unknowns) {

                    std::cerr << "[FATAL ERROR] Out-of-bounds index detected in stamping!\n"
                              << "Triplet values: Row=" << tr.Row << ", Column=" << tr.Column << "\n"
                              << "Total unknowns = " << total_unknowns << "\n";
                    std::abort(); // Stop the program immediately
                }
                //cout << "G assembelling" << endl;
                G[tr.Row][tr.Column] += tr.Value;
            }

            // calculating residual error
            vector<double> G_times_xk = multiply_matrix_vector(G, x_k);
            vector<double> residual = subtract_vectors(G_times_xk, b_rhs);

            //cout << "calcing residual errors" << endl;
            // checking the convergence
            if (check_convergence(residual)) {
                converged = true;
                break;
            }

            // solving
            for(double& val : residual) { val = -val; }
            vector<double> delta_x = algorithems.solve_LU(G, residual);

            if (delta_x.empty()) {
                break;
            }

            // updating the solution
            //cout << "updating soloution" << endl;
            for(int i = 0; i < x_k.size(); ++i) {
                x_k[i] += delta_x[i];
            }
            //cout << "solution updated" << endl;
        }
        //cout << "NR done" << endl;
        if (!converged) {
            cout << "Error: Newton-Raphson failed to converge at time t = " << t << endl;
            return;
        }

        x_previous = x_k;

        //cout << "saving data" << endl;
        // saving data
        for (auto* n : Active_Nodes) {
            n->set_voltage(x_previous[n->get_index()], t);
        }
        for (auto* e : Elements) {
            if (auto* v_source = dynamic_cast<DC_Source*>(e)) {
                int current_index = v_source->get_aux_index();
                double current = x_previous[current_index];
                v_source->set_current(current, t);
            }
            else if (auto* vcvs = dynamic_cast<VCVS*>(e)) {
                int current_index = vcvs->get_aux_index();
                double current = x_previous[current_index];
                vcvs->set_current(current, t);
            }
            else if (auto* diode = dynamic_cast<Real_Diode*>(e)) {
                double current = diode->calculate_current(x_previous);
                diode->set_current(current, t);
            }
            else if (auto* inductor = dynamic_cast<Inductor*>(e)) {
                int current_index = inductor->get_aux_index();
                double current = x_previous[current_index];
                inductor->set_current(current, t);
            }
            else if (auto* zener = dynamic_cast<Zener_Diode*>(e)) {
                double current = zener->calculate_current(x_previous);
                zener->set_current(current, t);
            }
            //cout << "data saved" << endl;
        }
    }
    cout << "transient worked!" << endl;
}

void Circuit::transient_linear()
{
    vector<double> x_previous(total_unknowns, 0.0);

    for (double t = t_start; t < t_end; t += time_step)
    {

        vector<Triplet> triplets;
        vector<double>  b_rhs(total_unknowns, 0.0);


        for (auto* e : Elements) {
            e->stamp(t, time_step, triplets, b_rhs, x_previous, x_previous);
        }

        vector<vector<double>> G(total_unknowns, vector<double>(total_unknowns, 0.0));
        for (const auto& tr : triplets) {
            if (tr.Row < 0 || tr.Row >= total_unknowns ||
                tr.Column < 0 || tr.Column >= total_unknowns) {

                std::cerr << "[FATAL ERROR] Out-of-bounds index detected in stamping!\n"
                          << "Triplet values: Row=" << tr.Row << ", Column=" << tr.Column << "\n"
                          << "Total unknowns = " << total_unknowns << "\n";
                std::abort();
            }
            G[tr.Row][tr.Column] += tr.Value;
        }
        vector<double> x_current = algorithems.solve_LU(G, b_rhs);
        if (x_current.empty()) {
            cout << "Error: Linear solver failed at time t = " << t << endl;
            return;
        }
        x_previous = x_current;

        for (auto* n : Active_Nodes) {
            n->set_voltage(x_previous[n->get_index()], t);
        }
        for (auto* e : Elements) {
            if (auto* v_source = dynamic_cast<DC_Source*>(e)) {
                int current_index = v_source->get_aux_index();
                double current = x_previous[current_index];
                v_source->set_current(current, t);
            }
            else if (auto* vcvs = dynamic_cast<VCVS*>(e)) {
                int current_index = vcvs->get_aux_index();
                double current = x_previous[current_index];
                vcvs->set_current(current, t);
            }
            else if (auto* inductor = dynamic_cast<Inductor*>(e)) {
                int current_index = inductor->get_aux_index();
                double current = x_previous[current_index];
                inductor->set_current(current, t);
            }
            else if (auto* diode = dynamic_cast<Real_Diode*>(e)) {
                double current = diode->calculate_current(x_previous);
                diode->set_current(current, t);
            }
            else if (auto* zener = dynamic_cast<Zener_Diode*>(e)) {
                double current = zener->calculate_current(x_previous);
                zener->set_current(current, t);
            }
        }
    }
    cout << "transient worked!" << endl;
    // this version is working just fine . if it doesnt work we should see whats added 11.08
}

Circuit::~Circuit() {
    for (Element* elem : Elements) {
        delete elem;
    }
    for (Node* node : Nodes) {
        delete node;
    }
}

void Circuit::transient()
{
    if (is_diode_added)
        transient_NR();
    else
        transient_linear();
}

Node* Circuit::create_new_node(const string& name)
{
    int existing_node_index = node_index_finder_by_name(name);
    if (existing_node_index != -1)
    {
        return Nodes[existing_node_index];
    }

    Node* new_node = new Node(name);

    Nodes.push_back(new_node);

    return new_node;
}

void Circuit::addElement(Element* new_element)
{
    if (new_element) {
        Elements.push_back(new_element);
    }
}

void Circuit::delete_node(Node *node_to_delete)
{
    for (auto it = Nodes.begin(); it != Nodes.end(); )
    {
        if ((*it)->get_name() == node_to_delete->get_name())
        {
            delete *it;
            it = Nodes.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

Node* SubCircuit::getInput()  { return input; }
Node* SubCircuit::getOutput()  { return output; }
void SubCircuit::setInput(Node* newInput) { input = newInput; }
void SubCircuit::setOutput(Node* newOutput) { output = newOutput; }

