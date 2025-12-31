#include <TSystem.h>
#include <unistd.h>

#include <iostream>

#include "../include/Config.h"
#include "../include/FileManager.h"
#include "../include/Statistics.h"

FileManager::FileManager()
    : input_file(nullptr), output_file(nullptr), input_tree(nullptr), output_tree(nullptr), files_valid(false) {
}

FileManager::~FileManager() {
    CloseFiles();
}

bool FileManager::FileExists(const std::string& fname) {
    return !gSystem->AccessPathName(fname.c_str());
}

std::string FileManager::FindHostname(const std::string& dataset) {
    char hostname[256];
    std::string hostname_str;
    std::string hostname_return;

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        hostname_str = hostname;
        std::cout << "Hostname: " << hostname << std::endl;

        if (hostname_str.substr(0, 8) == "dunegpvm") {
            hostname_return = hostname_str.substr(0, 8);
        } else if (hostname_str.substr(0, 3) == "ahl") {
            hostname_return = hostname_str.substr(0, 3);
        } else if (hostname_str.substr(0, 4) == "mPro") {
            hostname_return = hostname_str.substr(0, 4);
        } else {
            hostname_return = hostname;
        }
    } else {
        std::cerr << " ERROR: Failed to get hostname" << std::endl;
        return "";
    }

    if (!dataset.empty()) {
        std::cout << " : File type: " << dataset << std::endl;
    }

    return hostname_return;
}

void FileManager::FindPathsForHostname(const std::string& hostname, const std::string& dataset) {
    const char* data_root = std::getenv("THEIA_DATA_PATH");
    if (!data_root) {
        throw std::runtime_error(
            "THEIA_DATA_PATH environment variable is not set");
    }
    std::string base(data_root);

    const char* tag = std::getenv("THEIA_PRODUCTION_TAG");
    if (!tag) {
        throw std::runtime_error(
            "THEIA_PRODUCTION_TAG environment variable is not set");
    }
    std::string prod(tag);

    if (hostname == "dunegpvm") {
        std::cout << " : Matched dunegpvm" << std::endl;
        output_file_path =
            base + "/Ratpac_production/" + tag + "/OUTPUT/" + dataset + "/Theia_25kt_genie_";

        input_file_path =
            base + "/Ratpac_production/" + tag + "/INPUT/" + dataset + "/INPUTFILEDIR/genie_root_file_";
    }

    // UMN MSI

    // Local
}

void FileManager::SetupPaths(const std::string& dataset) {
    std::string hostname = FindHostname(dataset);
    FindPathsForHostname(hostname, dataset);
}

bool FileManager::OpenFiles(int file_nr) {
    CloseFiles();
    files_valid = false;

    bool flag_output_file_not_readable = false;
    bool flag_input_file_not_readable = false;
    bool debug = Config::Instance().GetDebugMode();

    // Load Ratpac output file
    output_file_name = output_file_path + std::to_string(file_nr) + ".root";
    if (debug) {
        std::cout << " - debug - Output file name: " << output_file_name << std::endl;
    }

    if (!FileExists(output_file_name)) {
        flag_output_file_not_readable = true;
        if (debug)
            std::cout << "Missing output file " << file_nr << ": " << output_file_name << std::endl;
    }

    if (!flag_output_file_not_readable) {
        output_file = TFile::Open(output_file_name.c_str());
    }

    if (!output_file || flag_output_file_not_readable || output_file->IsZombie()) {
        if (debug) {
            std::cout << " ! ERROR opening output file nr: " << file_nr
                      << " - File not readable :-/ " << std::endl;
        }
        return false;
    }

    // Load Genie input file
    input_file_name = input_file_path + std::to_string(file_nr) + ".root";
    if (debug) {
        std::cout << " - debug - Input file name: " << input_file_name << std::endl;
    }

    if (!FileExists(input_file_name)) {
        flag_input_file_not_readable = true;
    }

    if (!flag_input_file_not_readable) {
        input_file = TFile::Open(input_file_name.c_str());
    }

    if (!input_file || flag_input_file_not_readable || input_file->IsZombie()) {
        if (debug) {
            std::cout << " ! ERROR opening input file nr: " << file_nr
                      << " - File not readable :-/ " << std::endl;
        }
        return false;
    }

    // Get trees
    input_tree = (TTree*)input_file->Get("T");
    if (!input_tree) {
        std::cerr << "ERROR: input tree 'T' not found\n";
        return false;
    }

    output_tree = (TTree*)output_file->Get("output");
    if (!output_tree) {
        std::cerr << "ERROR: output tree 'output' not found\n";
        return false;
    }

    files_valid = true;
    return true;
}

void FileManager::CloseFiles() {
    if (input_file) {
        delete input_file;
        input_file = nullptr;
    }

    if (output_file) {
        delete output_file;
        output_file = nullptr;
    }

    input_tree = nullptr;
    output_tree = nullptr;
    files_valid = false;
}
