#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>

#include "atom/result_recorders.h"
#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/SimulationRepositoryImpl.hpp"
#include "utils/utils.h"

SimulationRepositoryImpl::SimulationRepositoryImpl() {
    db = ServiceLocator::getInstance().get<DataSource>().getDB();
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

size_t SimulationRepositoryImpl::add(const Simulation &simulation) const {
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
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error inserting simulation: " << e.what() << std::endl;
        return 0; // Indicate error
    }

    return db->getLastInsertRowid();
}

void SimulationRepositoryImpl::remove(size_t id) const {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return;
    }

    try {
        SQLite::Statement query(*db, "DELETE FROM Simulations WHERE id = ?;");
        query.bind(1, (int32_t)id);
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error deleting simulation: " << e.what() << std::endl;
    }
}

void SimulationRepositoryImpl::markSimulationComplete(size_t id) const {
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

std::vector<std::unique_ptr<Simulation>>
SimulationRepositoryImpl::getAll() const {
    if (!db)
        throw std::runtime_error("Database not initialized.");

    std::string sql = "SELECT id, name, record_interval, total_duration, "
                      "time_interval, status, r_0, v_0, theta_rv "
                      "FROM Simulations;";

    std::vector<std::unique_ptr<Simulation>> simulations;
    try {
        SQLite::Statement query(*db, sql);
        while (query.executeStep()) {
            auto simulation = std::make_unique<Simulation>();

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

            simulations.push_back(std::move(simulation));
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving simulations: " << e.what() << std::endl;
    }

    return simulations;
}