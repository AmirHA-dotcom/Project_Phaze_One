//
// Created by amirh on 2025-06-07.
//

#include "CCCS.h"

void CCCS::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{

}
//int i = node_plus->get_index();
//    int j = node_minus->get_index();
//    int a_ctrl = ctrl_vs->get_aux_index();
//    // The controlling voltage source’s aux_index equals the current through it.
//
//    // G[n+][a_ctrl] += +β
//    G_triplets.emplace_back(i, a_ctrl, +beta);
//    // G[n-][a_ctrl] += -β
//    G_triplets.emplace_back(j, a_ctrl, -beta);