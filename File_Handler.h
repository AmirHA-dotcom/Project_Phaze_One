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
    string mainFolderPath = "/Users/arian/Desktop/OOP/PNGs & FONTs";
    //string mainFolderPath = "D://Project_Files";
public:
    vector<string> get_file_names();
    vector<string> get_file_paths();
    void set_file_index(int file_index);
    void show_current_file();
    void add_file(const string& file_path);
    vector<vector<string>> showText (int file_index);
    void saveFiles();
    void loadFiles();
    string getMainFolderPath () const { return mainFolderPath; }
    vector<string> getFilesInDirectory (const string& directory) {
        vector<string> file_names;
        for (const auto& entry : filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                file_names.push_back(entry.path().filename().string());
            }
        }
        return file_names;
    }
};

#endif //PROJECT_PHAZE_ONE_FILE_HANDLER_H
