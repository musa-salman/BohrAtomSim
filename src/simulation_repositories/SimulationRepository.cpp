#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <string>

#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/SimulationRepository.hpp"

// Constructor: Open SQLite database and create table if it doesn't exist
SimulationRepository::SimulationRepository() {
    db = DataSource::getInstance()->getDB();

    std::string sql = "CREATE TABLE IF NOT EXISTS Simulations ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT NOT NULL, "
                      "type INTEGER NOT NULL, "
                      "record_interval INTEGER NOT NULL, "
                      "principal INTEGER NOT NULL, "
                      "angular INTEGER NOT NULL, "
                      "magnetic INTEGER NOT NULL, "
                      "revolutions REAL NOT NULL, "
                      "time_interval REAL NOT NULL, "
                      "status TEXT NOT NULL, "
                      "timestamp TEXT NOT NULL);";

    char *error_message;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_message);
    if (exit != SQLITE_OK) {
        std::cerr << "Error creating table: " << error_message << std::endl;
        sqlite3_free(error_message);
    }
}

// Destructor: Close the SQLite database connection
SimulationRepository::~SimulationRepository() {
    if (db) {
        sqlite3_close(db);
    }
}

// Create a new simulation entry in the database

// Create a new simulation entry in the database
size_t SimulationRepository::createSimulation(const Simulation &simulation) {
    std::string sql =
        std::format("INSERT INTO Simulations (name, type, record_interval,"
                    "principal INTEGER, angular INTEGER, magnetic INTEGER, "
                    "revolutions, time_interval, status, timestamp) "
                    "VALUES ('{}', {}, {}, {}, {}, {}, {}, {}, 'running', "
                    "datetime('now'));",
                    simulation.name, simulation.type,
                    simulation.record_interval, simulation.orbit.principal,
                    simulation.orbit.angular, simulation.orbit.magnetic,
                    simulation.revolutions, simulation.time_interval);

    char *error_message;
    if (int exit =
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_message);
        exit != SQLITE_OK) {
        std::cerr << "Error inserting simulation: " << error_message
                  << std::endl;
        sqlite3_free(error_message);
        return -1; // Indicate error
    }

    return sqlite3_last_insert_rowid(db);
}

// Remove a simulation from the database
void SimulationRepository::removeSimulation(size_t id) {
    std::string sql = std::format("DELETE FROM Simulations WHERE id={};", id);
    char *error_message;
    if (int exit =
            sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_message);
        exit != SQLITE_OK) {
        std::cerr << "Error deleting simulation: " << error_message
                  << std::endl;
        sqlite3_free(error_message);
    }

    // Remove .bin file
    std::string filename = std::format("{}/simulations/{}.bin", DB_PATH, id);
    std::filesystem::remove(filename);
}

// Mark a simulation as complete in the database
void SimulationRepository::markSimulationComplete(size_t id) {
    std::string sql = std::format(
        "UPDATE Simulations SET status='completed' WHERE id={};", id);
    char *error_message;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &error_message);
    if (exit != SQLITE_OK) {
        std::cerr << "Error updating status: " << error_message << std::endl;
        sqlite3_free(error_message);
    }
}

// Callback function for retrieving simulation data
int SimulationRepository::callback(void *data, [[maybe_unused]] int argc,
                                   char **argv,
                                   [[maybe_unused]] char **colNames) {
    auto simulations =
        static_cast<std::vector<std::shared_ptr<Simulation>> *>(data);
    auto sim = std::make_shared<Simulation>();
    sim->id = argv[0] ? std::stoul(argv[0]) : 0;
    sim->name = argv[1] ? argv[1] : "";
    sim->type = argv[2] ? std::stoi(argv[2]) : 0;
    sim->record_interval =
        argv[3] ? static_cast<unsigned short>(std::stoi(argv[3])) : 0;
    sim->orbit.principal = argv[4] ? std::stoi(argv[4]) : 0;
    sim->orbit.angular = argv[5] ? std::stoi(argv[5]) : 0;
    sim->orbit.magnetic = argv[6] ? std::stoi(argv[6]) : 0;
    sim->revolutions = argv[7] ? std::stof(argv[7]) : 0.0f;
    sim->time_interval = argv[8] ? std::stod(argv[8]) : 0.0;

    simulations->push_back(sim);
    return 0;
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

    std::string sql = "SELECT id, name, type, record_interval, "
                      "principal, angular, magnetic, "
                      "revolutions, time_interval FROM Simulations;";
    char *error_message;

    if (int exit = sqlite3_exec(db, sql.c_str(), callback, &simulations,
                                &error_message);
        exit != SQLITE_OK) {
        std::cerr << "Error retrieving simulations: " << error_message
                  << std::endl;
        sqlite3_free(error_message);
    }

    return simulations;
}
