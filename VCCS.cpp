//
// Created by amirh on 2025-06-07.
//

#include "VCCS.h"

void VCCS::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i, j, ictrl, jctrl, a;
    i = node1->get_index();
    j = node2->get_index();
    ictrl = ctrl_node1->get_index();
    jctrl = ctrl_node1->get_index();
    // stamping
    G_triplets.emplace_back(i, ictrl, value);
    G_triplets.emplace_back(i, jctrl, -value);
    G_triplets.emplace_back(j, ictrl, -value);
    G_triplets.emplace_back(j, jctrl, value);
}