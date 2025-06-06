//
// Created by amirh on 2025-06-07.
//

#include "Algorithems.h"

float Algorithems::determinant(vector<vector<double>> matrix)
{
    int n = matrix.size();
    double det = 1;
    for (int i = 0; i < n; i++)
    {
        int max_row = i;
        for (int k = i + 1; k < n; k++)
        {
            if (abs(matrix[k][i]) > abs(matrix[max_row][i]))
            {
                max_row = k;
            }
        }
        if (max_row != i)
        {
            swap(matrix[i], matrix[max_row]);
            det *= -1;
        }
        for (int j = i + 1; j < n; j++)
        {
            double factor = matrix[j][i] / matrix[i][i];
            for (int k = i; k < n; k++)
            {
                matrix[j][k] -= factor * matrix[i][k];
            }
        }
        det *= matrix[i][i];
    }
    return det;
}

vector<double> Algorithems::solve_cramer(vector<vector<double>> G, vector<double> I)
{
    int n = G.size();
    vector<double> solution(n);
    double det_G = determinant(G);

    for (int i = 0; i < n; i++) {
        auto G_temp = G;
        for (int j = 0; j < n; j++) {
            G_temp[j][i] = I[j];
        }
        float det_temp = determinant(G_temp);
        solution[i] = det_temp / det_G;
    }
    return solution;
}
