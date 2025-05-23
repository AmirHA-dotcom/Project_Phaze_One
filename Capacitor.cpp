//
// Created by amirh on 2025-05-23.
//

#include "Capacitor.h"

double Capacitor::get_current()
{
    cout << "Still Not Coded!!!" << endl;
    return 0;
}

void Capacitor::display_info()
{
    cout << "Capacitor Name : " << name << endl;
    cout << "C = " << value << endl;
    cout << "From Node " << node1->get_name() << " to Node " << node2->get_name() << endl;
}