#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>

#include "simulation_repositories/SimulationResultLoader.hpp"

void SimulationResultLoader::loadSimulation(
    const std::string &filepath, const std::vector<std::string> &column_names,
    std::unordered_map<std::string, std::vector<double>> &datasets) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filepath << "\n";
        std::cerr << "Reason: " << strerror(errno) << "\n";
        return;
    }

    // Determine the number of columns
    size_t num_columns = column_names.size();
    if (num_columns == 0) {
        std::cerr << "Error: Invalid column names provided.\n";
        return;
    }

    // Read entire binary file into a buffer
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (file_size % sizeof(double) != 0) {
        std::cerr
            << "Error: Binary file size is not a multiple of sizeof(double).\n";
        return;
    }

    size_t num_elements = file_size / sizeof(double);
    file.close();

    if (num_elements == 0) {
        std::cerr << "Error: No data in binary file.\n";
        return;
    }

    // Read data into buffer
    std::vector<double> double_buffer(num_elements);
    std::ifstream in_file(filepath, std::ios::binary);
    in_file.read(reinterpret_cast<char *>(double_buffer.data()), file_size);
    in_file.close();

    // Compute total rows in the file
    size_t total_rows = num_elements / num_columns;

    // Check existing rows
    size_t existing_rows =
        datasets.empty() ? 0 : datasets.begin()->second.size();

    if (existing_rows >= total_rows) {
        return; // No new rows to add
    }

    size_t new_rows = total_rows - existing_rows;

    // Append only new rows
    for (size_t col = 0; col < num_columns; ++col) {
        std::vector<double> new_data(new_rows);
        for (size_t row = 0; row < new_rows; ++row) {
            new_data[row] =
                double_buffer[(existing_rows + row) * num_columns + col];
        }

        // Append new data to the dataset
        datasets[column_names[col]].insert(datasets[column_names[col]].end(),
                                           new_data.begin(), new_data.end());
    }
}
