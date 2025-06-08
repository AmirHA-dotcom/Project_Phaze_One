//
// Created by amirh on 2025-06-07.
//

#include "CCVS.h"

void CCVS::set_aux_index(int i)
{
    aux_index = i;
}

void CCVS::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i, j, ictrl, jctrl, a;
    i = node1->get_index();
    j = node2->get_index();
    a = aux_index;


    // tieing to the ctrl nodes
}
//int i = node_plus->get_index();
//    int j = node_minus->get_index();
//    int a_out = aux_index;
//    int a_ctrl = ctrl_vs->get_aux_index();  // current through controlling VS
//
//    // 1) Tie I_{a_out} into KCL at n+ and n-:
//    G_triplets.emplace_back(i, a_out,  +1.0);
//    G_triplets.emplace_back(j, a_out,  -1.0);
//
//    // 2) In the aux‐equation (row = a_out), enforce:
//    //    V_{n+} - V_{n-} - r0 * I_ctrl = 0
//    G_triplets.emplace_back(a_out, i,     +1.0);
//    G_triplets.emplace_back(a_out, j,     -1.0);
//    G_triplets.emplace_back(a_out, a_ctrl, -r0);