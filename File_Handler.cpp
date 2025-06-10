//
// Created by amirh on 2025-06-08.
//

#include "File_Handler.h"

vector<string> File_Handler::get_file_names()
{
    vector<string> file_names;
    for (const auto& f : files)
        file_names.push_back(f.first);
    return file_names;
}

void File_Handler::set_file_index(int file_index)
{
    if (file_index >= 0 && file_index < files.size())
    {
        current_file_index = file_index;
    }
    else
    {
        cerr << "Error: Inappropriate input. Index out of range." << endl;
        current_file_index = -1;
    }
}

void File_Handler::show_current_file()
{
    if (current_file_index < 0 || current_file_index >= files.size())
    {
        cerr << "Error: No file selected or invalid index." << endl;
        return;
    }

    const string& file_name = files[current_file_index].first;
    const string& file_path = files[current_file_index].second;

    ifstream input_file(file_path);
    if (!input_file.is_open())
    {
        cerr << "Error: Could not open the file " << file_path << endl;
        return;
    }
    cout << file_name << " :" << endl;
    cout << "-------------------------------" << endl;
    // reading and showing the file
    string line;
    while (getline(input_file, line))
    {
        cout << line << endl;
    }
    input_file.close();
    cout << "-------------------------------" << endl;
}

void File_Handler::add_file(const string& file_path)
{
    size_t last_separator_pos = file_path.find_last_of("/\\");
    string file_name;

    if (string::npos != last_separator_pos)
    {
        // extracting the filename from the path
        file_name = file_path.substr(last_separator_pos + 1);
    }
    else
    {
        file_name = file_path;
    }
    // removing .txt
    size_t dot_pos = file_name.rfind('.');
    if (string::npos != dot_pos)
    {
        file_name = file_name.substr(0, dot_pos);
    }
    files.push_back({file_name, file_path});
}
