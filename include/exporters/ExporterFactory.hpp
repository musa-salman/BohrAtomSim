#ifndef EXPORTER_FACTORY_HPP
#define EXPORTER_FACTORY_HPP

#include <memory>
#include <stdexcept>

#include "ExportFormat.hpp"
#include "HDF5Exporter.hpp"
#include "SimulationExporter.hpp"

inline std::unique_ptr<SimulationExporter> createExporter(ExportFormat format) {
    switch (format) {
    case ExportFormat::HDF5:
        return std::make_unique<HDF5Exporter>();
    default:
        throw std::invalid_argument("Unsupported export format.");
    }
}

#endif // EXPORTER_FACTORY_HPP
