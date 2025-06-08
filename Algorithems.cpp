//
// Created by amirh on 2025-06-07.
//

#include "Algorithems.h"

vector<double> Algorithems::solve_LU(vector<vector<double>> G, vector<double> I)
{
    int n = G.size();

    for (int i = 0; i < n; i++)
    {
        // the row with the largest element in the current column
        int max_row = i;
        for (int k = i + 1; k < n; k++)
        {
            if (abs(G[k][i]) > abs(G[max_row][i]))
            {
                max_row = k;
            }
        }

        // swaping the pivot row with the current row in both G and I
        if (max_row != i)
        {
            swap(G[i], G[max_row]);
            swap(I[i], I[max_row]);
        }

        // singular matrix
        if (abs(G[i][i]) < 1e-12)
        {
            cerr << "Error: Matrix is singular. Cannot solve." << endl;
            return {};
        }

        // performing the elimination for the rows below the pivot
        for (int j = i + 1; j < n; j++)
        {
            double factor = G[j][i] / G[i][i];
            G[j][i] = factor;
            for (int k = i + 1; k < n; k++)
            {
                G[j][k] -= factor * G[i][k];
            }
        }
    }

    // solve for an intermediate solution y and store it back into I.
    vector<double> y(n);
    for (int i = 0; i < n; i++)
    {
        double sum = 0.0;
        for (int j = 0; j < i; j++)
        {
            sum += G[i][j] * y[j];
        }
        y[i] = I[i] - sum;
    }

    // solve for the final solution x using the upper-triangular matrix U.
    vector<double> x(n);
    for (int i = n - 1; i >= 0; i--)
    {
        double sum = 0.0;
        for (int j = i + 1; j < n; j++)
        {
            sum += G[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / G[i][i];
    }

    return x;
}