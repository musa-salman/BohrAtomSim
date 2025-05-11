
#include <fstream>

#include "exporters/CSVExporter.hpp"

void CSVExporter::exportData(const std::string &path, const Dataset &datasets) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return;
    }

    const auto &itNames = datasets.getColumnsNames();
    const auto &endNames = datasets.getColumnsNames().end();
    for (auto it = itNames.begin(); it != endNames; ++it) {
        file << *it;
        if (std::next(it) != endNames)
            file << ",";
    }
    file << "\n";

    for (size_t i = 0; i < datasets.getRowCount(); ++i) {
        for (auto it = itNames.begin(); it != endNames; ++it) {
            file << datasets.get(*it)[i];
            if (std::next(it) != endNames)
                file << ",";
        }
        file << "\n";
    }
    file.close();
}
