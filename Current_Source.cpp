//
// Created by amirh on 2025-06-07.
//

#include "Current_Source.h"

void Current_Source::stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous)
{
    int i, j;
    i = node1->get_index();
    j = node2->get_index();
    // stamp in b
    b[i] += value;
    b[j] -= value;
}