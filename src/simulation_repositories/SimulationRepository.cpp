#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

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
        "status INTEGER NOT NULL DEFAULT 0, "
        "stepper_type INTEGER NOT NULL, "
        "params TEXT NOT NULL, "
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
            *db, "INSERT INTO Simulations (name, stepper_type, params, "
                 "timestamp) "
                 "VALUES (?, ?, ?, datetime('now'));");

        query.bind(1, simulation.getName());
        query.bind(2, static_cast<int32_t>(simulation.getStepperType()));
        query.bind(3, simulation.serializeParams());
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

    std::string sql = "SELECT id, name, status, stepper_type, params "
                      "FROM Simulations;";

    std::vector<std::unique_ptr<Simulation>> simulations;
    try {
        SQLite::Statement query(*db, sql);
        while (query.executeStep()) {
            auto simulation = std::make_unique<Simulation>();

            simulation->setId(query.getColumn(0).getInt());
            simulation->setName(query.getColumn(1).getText());

            simulation->setStatus(static_cast<Simulation::SimulationStatus>(
                query.getColumn(2).getInt()));
            simulation->setStepperType(
                static_cast<StepperType>(query.getColumn(3).getInt()));

            auto paras = nlohmann::json::parse(query.getColumn(4).getText());
            simulation->setParams(paras);

            simulations.push_back(std::move(simulation));
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving simulations: " << e.what() << std::endl;
    }

    return simulations;
}