#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include "atom/result_recorders.h"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/SimulationRepository.hpp"
#include "utils/utils.h"

SimulationRepository::SimulationRepository() {
    db = ServiceLocator::getInstance().get<DataSource>()->getDB();
    if (!db) {
        std::cerr << "Failed to open database." << std::endl;
        return;
    }

    const std::string initializeSimulationsTableSQL =
        "CREATE TABLE IF NOT EXISTS Simulations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "record_interval INTEGER NOT NULL, "
        "total_duration REAL NOT NULL, "
        "time_interval REAL NOT NULL, "
        "status INTEGER NOT NULL, "
        "r_0 REAL NOT NULL, "
        "v_0 REAL NOT NULL, "
        "theta_rv REAL NOT NULL, "
        "timestamp TEXT NOT NULL);";

    try {
        db->exec(initializeSimulationsTableSQL);
    } catch (const SQLite::Exception &e) {
        std::cerr << "[SimulationRepository] Error creating table: " << e.what()
                  << std::endl;
    }
}

size_t SimulationRepository::add(const Simulation &simulation) {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return 0; // Indicate error
    }

    try {
        SQLite::Statement query(
            *db, "INSERT INTO Simulations (name, record_interval, "
                 "total_duration, time_interval, status, r_0, v_0, theta_rv, "
                 "timestamp) "
                 "VALUES (?, ?, ?, ?, ?, ?, ?, ?, datetime('now'));");

        query.bind(1, simulation.getName());
        query.bind(2, simulation.getRecordInterval());
        query.bind(3, simulation.getTotalDuration());
        query.bind(4, simulation.getDeltaTime());
        query.bind(5, (int)simulation.status);
        query.bind(6, simulation.getR0());
        query.bind(7, simulation.getV0());
        query.bind(8, simulation.getThetaRV());

        query.exec();

        {
            char file_name[FILE_PATH_SIZE];
            format_output_filename(db->getLastInsertRowid(), file_name);
            FILE *file_bin = fopen(file_name, "wb");
            if (!file_bin) {
                std::cerr << "Error opening file for writing: " << file_name
                          << std::endl;
                return 0; // Indicate error
            }
            const uint8_t field_names_2DR[][MAX_FIELD_NAME] = {
                "t", "r", "r_dot", "r_ddot", "phi", "phi_dot", "gamma"};

            init_file_header(file_bin, field_names_2DR,
                             sizeof(field_names_2DR) /
                                 sizeof(field_names_2DR[0]));
            fclose(file_bin);
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error inserting simulation: " << e.what() << std::endl;
        return 0; // Indicate error
    }

    return db->getLastInsertRowid();
}

void SimulationRepository::remove(size_t id) {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return;
    }

    try {
        SQLite::Statement query(*db, "DELETE FROM Simulations WHERE id = ?;");
        query.bind(1, (int32_t)id);
        query.exec();

        std::string filename =
            std::format("{}/simulations/{}.bin", DB_PATH, id);
        std::filesystem::remove(filename);
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error deleting simulation: " << e.what() << std::endl;
    }
}

void SimulationRepository::markSimulationComplete(size_t id) {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return;
    }

    try {
        SQLite::Statement query(
            *db, "UPDATE Simulations SET status = 3 WHERE id = ?;");
        query.bind(1, (int32_t)id);
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error updating simulation status: " << e.what()
                  << std::endl;
    }
}

std::vector<std::shared_ptr<Simulation>> SimulationRepository::getAll() {
    if (!db) {
        return {};
    }

    simulations.clear();

    std::string sql = "SELECT id, name, record_interval, total_duration, "
                      "time_interval, status, r_0, v_0, theta_rv "
                      "FROM Simulations;";

    try {
        SQLite::Statement query(*db, sql);
        while (query.executeStep()) {
            auto simulation = std::make_shared<Simulation>();

            simulation->setId(query.getColumn(0).getInt());
            simulation->setName(query.getColumn(1).getText());
            simulation->setRecordInterval(query.getColumn(2).getInt());
            simulation->setTotalDuration(query.getColumn(3).getDouble());
            simulation->setDeltaTime(query.getColumn(4).getDouble());
            simulation->setStatus(static_cast<Simulation::SimulationStatus>(
                query.getColumn(5).getInt()));
            simulation->setR0(query.getColumn(6).getDouble());
            simulation->setV0(query.getColumn(7).getDouble());
            simulation->setThetaRV(query.getColumn(8).getDouble());

            simulations.push_back(simulation);
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving simulations: " << e.what() << std::endl;
    }

    return simulations;
}
