//
// Created by amirh on 2025-05-23.
//

#include "Voltage_Source.h"

void Voltage_Source::stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous)
{
    int i, j, a;
    i = node1->get_index();
    j = node2->get_index();
    a = aux_index;
    // stamping in G
    G_triplets.emplace_back(i, a, 1.0);
    G_triplets.emplace_back(j, a, -1.0);
    G_triplets.emplace_back(a, i, 1.0);
    G_triplets.emplace_back(a, j, -1.0);
    // stamping in b
    b[a] = value;
}