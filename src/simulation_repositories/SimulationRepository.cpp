#include <cstdint>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/SimulationRepository.hpp"

// Constructor: Open SQLite database and create table if it doesn't exist
SimulationRepository::SimulationRepository() {
    db = DataSource::getInstance()->getDB();
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
        "status TEXT NOT NULL, "
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

size_t SimulationRepository::createSimulation(const Simulation &simulation) {
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

        query.bind(1, simulation.name);
        query.bind(2, simulation.record_interval);
        query.bind(3, simulation.total_duration);
        query.bind(4, simulation.time_interval);
        query.bind(5, "in progress");
        query.bind(6, simulation.r_0);
        query.bind(7, simulation.v_0);
        query.bind(8, simulation.theta_rv);

        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error inserting simulation: " << e.what() << std::endl;
        return 0; // Indicate error
    }

    return db->getLastInsertRowid();
}

void SimulationRepository::removeSimulation(size_t id) {
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
            *db, "UPDATE Simulations SET status = 'completed' WHERE id = ?;");
        query.bind(1, (int32_t)id);
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error updating simulation status: " << e.what()
                  << std::endl;
    }
}

std::vector<std::shared_ptr<Simulation>>
SimulationRepository::getSimulations(bool cached) {
    if (!db) {
        return {};
    }

    if (cached && !simulations.empty()) {
        return simulations;
    }

    simulations.clear();

    std::string sql = "SELECT id, name, record_interval, total_duration, "
                      "time_interval, status, r_0, v_0, theta_rv "
                      "FROM Simulations;";

    try {
        SQLite::Statement query(*db, sql);
        while (query.executeStep()) {
            auto simulation = std::make_shared<Simulation>();
            simulation->id = query.getColumn(0).getInt();
            simulation->name = query.getColumn(1).getText();
            simulation->record_interval = query.getColumn(2).getInt();
            simulation->total_duration = query.getColumn(3).getDouble();
            simulation->time_interval = query.getColumn(4).getDouble();
            simulation->r_0 = query.getColumn(5).getDouble();
            simulation->v_0 = query.getColumn(6).getDouble();
            simulation->theta_rv = query.getColumn(7).getDouble();

            simulations.push_back(simulation);
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving simulations: " << e.what() << std::endl;
    }

    return simulations;
}
