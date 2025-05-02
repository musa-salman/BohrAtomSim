
#include <fstream>

#include "exporters/CSVExporter.hpp"

void CSVExporter::exportData(
    const std::string &path,
    const std::unordered_map<std::string, std::vector<double>> &datasets) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return;
    }

    for (auto it = datasets.begin(); it != datasets.end(); ++it) {
        file << it->first;
        if (std::next(it) != datasets.end()) {
            file << ",";
        }
    }
    file << "\n";

    size_t num_rows = datasets.begin()->second.size();
    for (size_t i = 0; i < num_rows; ++i) {
        for (auto it = datasets.begin(); it != datasets.end(); ++it) {
            file << it->second[i];
            if (std::next(it) != datasets.end()) {
                file << ",";
            }
        }
        file << "\n";
    }
    file.close();
}
