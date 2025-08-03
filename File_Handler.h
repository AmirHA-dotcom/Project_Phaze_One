//
// Created by amirh on 2025-06-08.
//

#ifndef PROJECT_PHAZE_ONE_FILE_HANDLER_H
#define PROJECT_PHAZE_ONE_FILE_HANDLER_H

#include "Libraries.h"

class File_Handler
{
private:
    vector<pair<string, string>> files;     // pair of file name and file path
    int current_file_index = 0;
    string mainFolderPath;
public:
    vector<string> get_file_names();
    vector<string> get_file_paths();
    void set_file_index(int file_index);
    void show_current_file();
    void add_file(const string& file_path);
    vector<vector<string>> showText (int file_index);
    void saveFiles();
    void loadFiles();

};

#endif //PROJECT_PHAZE_ONE_FILE_HANDLER_H
