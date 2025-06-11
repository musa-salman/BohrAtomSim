#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <SQLiteCpp/SQLiteCpp.h>
#include <gsl/assert>
#include <nlohmann/json.hpp>

#include "simulation/model/Potential.hpp"
#include "simulation/model/Simulation.hpp"
#include "storage/persistence/DataSource.hpp"
#include "storage/persistence/SimulationRepositoryImpl.hpp"
#include "utils/ServiceLocator.hpp"

namespace storage::persistence {
using namespace simulation::model;
using namespace utils;

SimulationRepositoryImpl::SimulationRepositoryImpl() {
    db = ServiceLocator::getInstance().get<DataSource>().getDB();
    Expects(db);

    constexpr std::string_view initializeSimulationsTableSQL =
        "CREATE TABLE IF NOT EXISTS Simulations ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "status INTEGER NOT NULL DEFAULT 0,"
        "is_relativistic INTEGER NOT NULL DEFAULT 1,"
        "is_quantized INTEGER NOT NULL DEFAULT 1,"
        "is_3d INTEGER NOT NULL DEFAULT 0,"
        "r0_x REAL NOT NULL,"
        "r0_y REAL NOT NULL,"
        "r0_z REAL NOT NULL,"
        "v0_x REAL NOT NULL,"
        "v0_y REAL NOT NULL,"
        "v0_z REAL NOT NULL,"
        "delta_time REAL NOT NULL,"
        "total_duration REAL NOT NULL,"
        "record_interval INTEGER NOT NULL,"
        "r_local_max_limit,"
        "constants TEXT NOT NULL,"
        "potential_id INTEGER NOT NULL,"
        "timestamp TEXT NOT NULL DEFAULT (datetime('now')),"
        "FOREIGN KEY(potential_id) REFERENCES Potentials(id) ON DELETE "
        "RESTRICT);";

    try {
        db->exec(initializeSimulationsTableSQL.data());
    } catch (const SQLite::Exception &e) {
        std::cerr << "[SimulationRepository] Error creating table: " << e.what()
                  << std::endl;
    }
}

size_t SimulationRepositoryImpl::add(const Simulation &simulation) const {
    try {
        SQLite::Statement query(
            *db,
            R"(INSERT INTO Simulations (name, status, is_relativistic,
                                is_quantized, is_3d, 
                                r0_x, r0_y, r0_z, v0_x, v0_y, v0_z, 
                                delta_time, total_duration, record_interval, 
                                constants, potential_id, r_local_max_limit) 
                                VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,
                                ?, ?);)");

        query.bind(1, simulation.getName());
        query.bind(2, static_cast<int>(simulation.getStatus()));
        query.bind(3, static_cast<int>(simulation.isRelativistic()));
        query.bind(4, static_cast<int>(simulation.isQuantized()));
        query.bind(5, static_cast<int>(simulation.is3D()));

        const auto &r0 = simulation.getR0();
        const auto &v0 = simulation.getV0();

        query.bind(6, r0.x);
        query.bind(7, r0.y);
        query.bind(8, r0.z);
        query.bind(9, v0.x);
        query.bind(10, v0.y);
        query.bind(11, v0.z);
        query.bind(12, simulation.getDeltaTime());
        query.bind(13, simulation.getTotalDuration());
        query.bind(14, simulation.getRecordInterval());
        query.bind(15, simulation.serializeConstants());
        query.bind(16, static_cast<int>(simulation.getPotential().getId()));
        query.bind(17, simulation.getRLocalMaxCountLimit());

        query.exec();

    } catch (const SQLite::Exception &e) {
        std::cerr << "Error inserting simulation: " << e.what() << std::endl;
        return 0; // Indicate error
    }

    return db->getLastInsertRowid();
}

void SimulationRepositoryImpl::remove(size_t id) const {
    try {
        SQLite::Statement query(*db, "DELETE FROM Simulations WHERE id = ?;");
        query.bind(1, static_cast<int>(id));
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error deleting simulation: " << e.what() << std::endl;
    }
}

void SimulationRepositoryImpl::completeSimulation(size_t id) const {
    try {
        SQLite::Statement query(
            *db, "UPDATE Simulations SET status = 3 WHERE id = ?;");
        query.bind(1, static_cast<int>(id));
        query.exec();

    } catch (const SQLite::Exception &e) {
        std::cerr << "Error updating simulation status: " << e.what()
                  << std::endl;
    }
}

void SimulationRepositoryImpl::update(const Simulation &simulation) {
    try {
        SQLite::Statement query(
            *db,
            R"(UPDATE Simulations SET name = ?, status = ?, is_relativistic = ?,
                                is_quantized = ?, is_3d = ?, 
                                r0_x = ?, r0_y = ?, r0_z = ?, v0_x = ?, v0_y = ?,
                                v0_z = ?, delta_time = ?, total_duration = ?,
                                record_interval = ?, constants = ?,
                                potential_id = ?, r_local_max_limit = ?
                                WHERE id = ?;)");
        query.bind(1, simulation.getName());
        query.bind(2, static_cast<int>(simulation.getStatus()));
        query.bind(3, static_cast<int>(simulation.isRelativistic()));
        query.bind(4, static_cast<int>(simulation.isQuantized()));
        query.bind(5, static_cast<int>(simulation.is3D()));

        const auto &r0 = simulation.getR0();
        const auto &v0 = simulation.getV0();

        query.bind(6, r0.x);
        query.bind(7, r0.y);
        query.bind(8, r0.z);
        query.bind(9, v0.x);
        query.bind(10, v0.y);
        query.bind(11, v0.z);
        query.bind(12, simulation.getDeltaTime());
        query.bind(13, simulation.getTotalDuration());
        query.bind(14, simulation.getRecordInterval());
        query.bind(15, simulation.serializeConstants());
        query.bind(16, static_cast<int>(simulation.getPotential().getId()));
        query.bind(17, simulation.getRLocalMaxCountLimit());
        query.bind(18, static_cast<int>(simulation.getId()));

        query.exec();

    } catch (const SQLite::Exception &e) {
        std::cerr << "Error updating simulation: " << e.what() << std::endl;
    }
}

std::vector<std::unique_ptr<Simulation>>
SimulationRepositoryImpl::getAll() const {
    constexpr std::string_view sql =
        R"(SELECT s.id, s.name, s.status, s.is_relativistic, s.is_quantized,
                  s.is_3d,
                  s.r0_x, s.r0_y, s.r0_z,
                  s.v0_x, s.v0_y, s.v0_z,
                  s.delta_time, s.total_duration, s.record_interval,
                  s.r_local_max_limit,
                  s.constants,
                  p.id AS potential_id, p.name AS potential_name,
                  p.type AS potential_type, p.expression AS potential_expression,
                  p.constants AS potential_constants
           FROM Simulations s
           JOIN Potentials p ON s.potential_id = p.id
           ORDER BY s.id DESC;
    )";

    std::vector<std::unique_ptr<Simulation>> simulations;
    try {
        SQLite::Statement query(*db, sql.data());
        while (query.executeStep()) {
            auto simulation = std::make_unique<Simulation>();

            // Simulation fields
            simulation->setId(query.getColumn(0).getInt());
            simulation->setName(query.getColumn(1).getText());
            simulation->setStatus(static_cast<Simulation::SimulationStatus>(
                query.getColumn(2).getInt()));
            simulation->setRelativistic(query.getColumn(3).getInt() != 0);
            simulation->setQuantized(query.getColumn(4).getInt() != 0);
            simulation->set3D(query.getColumn(5).getInt() != 0);

            physics::math::Vector3 r0(query.getColumn(6).getDouble(),
                                      query.getColumn(7).getDouble(),
                                      query.getColumn(8).getDouble());
            simulation->setR0(r0);

            physics::math::Vector3 v0(query.getColumn(9).getDouble(),
                                      query.getColumn(10).getDouble(),
                                      query.getColumn(11).getDouble());
            simulation->setV0(v0);

            simulation->setDeltaTime(query.getColumn(12).getDouble());
            simulation->setTotalDuration(query.getColumn(13).getDouble());
            simulation->setRecordInterval(query.getColumn(14).getInt());

            simulation->setRLocalMaxCountLimit(query.getColumn(15).getInt());

            simulation->setConstantValues(query.getColumn(16).getText());

            // Potential fields
            Potential potential;
            potential.setId(query.getColumn(17).getInt());
            potential.setName(query.getColumn(18).getText());
            potential.setType(
                static_cast<PotentialType>(query.getColumn(19).getInt()));
            potential.setExpression(query.getColumn(20).getText());

            auto constants = nlohmann::json::parse(
                query.getColumn(21).getText(), nullptr, false);
            potential.setConstants(constants);

            simulation->setPotential(potential);

            simulations.push_back(std::move(simulation));
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving joined simulations and potentials: "
                  << e.what() << std::endl;
    }

    return simulations;
}
} // namespace storage::persistence
