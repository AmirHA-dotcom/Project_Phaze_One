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
vector<string> File_Handler::get_file_paths()
{
    vector<string> file_paths;
    for (const auto& f : files)
        file_paths.push_back(f.second);
    return file_paths;
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
    // checking if the file already exists
    for (const auto& f : files)
    {
        if (f.second == file_path)
        {
            std::remove(files.begin(), files.end(),f);
        }
    }
    files.push_back({file_name, file_path});
}

vector<vector<string>> File_Handler::showText (int file_index) {
    vector<vector<string>> file_content;
    string filePath = get_file_paths()[file_index];
    ifstream file_stream(filePath);

    if (!file_stream.is_open()) {
        cerr << "Error: Could not open file at path: " << filePath << endl;
        return file_content;
    }

    string line;
    while (getline(file_stream, line)) {
        vector<string> line_as_vector;
        stringstream line_stream(line);
        string word;

        while (line_stream >> word) {
            line_as_vector.push_back(word);
        }

        file_content.push_back(line_as_vector);
    }

    file_stream.close();
    return file_content;
}

void File_Handler::saveFiles() {
    string outputPath = mainFolderPath + "/files_info.txt";
    ofstream out_file(outputPath);
    if (!out_file.is_open()) {
        cerr << "Error: Could not open file for writing: " << outputPath << endl;
        return;
    }

    // ذخیره اطلاعات همه فایل‌ها
    for (const auto &file : files) {
        out_file << "File Name: " << file.first << "\n";
        out_file << "File Path: " << file.second << "\n";
        out_file << "----------------------------------------\n";
    }

    out_file.close();
    cout << "Files info saved to: " << outputPath << endl;
}

void File_Handler::loadFiles() {
    string inputPath = mainFolderPath + "/files_info.txt";
    ifstream in_file(inputPath);
    if (!in_file.is_open()) {
        cerr << "Error: Could not open file for reading: " << inputPath << endl;
        return;
    }
    files.clear(); // پاک کردن لیست قبلی
    string line, fileName, filePath;
    while (getline(in_file, line)) {
        if (line.find("File Name: ") == 0) {
            fileName = line.substr(11); // طول "File Name: " = 11
        } else if (line.find("File Path: ") == 0) {
            filePath = line.substr(11); // طول "File Path: " = 11
        } else if (line.find("---") == 0) {
            // وقتی جداکننده رسیدیم یعنی یک فایل کامل شده
            if (!fileName.empty() && !filePath.empty()) {
                files.emplace_back(fileName, filePath);
            }
            fileName.clear();
            filePath.clear();
        }
    }
    // در صورتی که آخرین ورودی جداکننده نداشت
    if (!fileName.empty() && !filePath.empty()) {
        files.emplace_back(fileName, filePath);
    }
    in_file.close();
    cout << "Files info loaded from: " << inputPath << endl;
}


