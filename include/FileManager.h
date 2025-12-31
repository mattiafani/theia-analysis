#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <TFile.h>
#include <TTree.h>

#include <string>

class FileManager {
   public:
    FileManager();
    ~FileManager();

    void SetupPaths(const std::string& chunk);
    std::string GetInputPath() const { return input_file_path; }
    std::string GetOutputPath() const { return output_file_path; }

    bool OpenFiles(int file_nr);
    void CloseFiles();

    TFile* GetInputFile() { return input_file; }
    TFile* GetOutputFile() { return output_file; }
    TTree* GetInputTree() { return input_tree; }
    TTree* GetOutputTree() { return output_tree; }

    bool IsValid() const { return files_valid; }

    static bool FileExists(const std::string& fname);
    static std::string FindHostname(const std::string& chunk = "");

   private:
    std::string input_file_path;
    std::string output_file_path;
    std::string input_file_name;
    std::string output_file_name;

    TFile* input_file;
    TFile* output_file;
    TTree* input_tree;
    TTree* output_tree;

    bool files_valid;

    void FindPathsForHostname(const std::string& hostname, const std::string& chunk);
};

#endif
