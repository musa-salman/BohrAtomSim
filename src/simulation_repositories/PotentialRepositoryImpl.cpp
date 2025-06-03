#include <array>
#include <iostream>

#include "service_locator/ServiceLocator.hpp"
#include "simulation_repositories/DataSource.hpp"
#include "simulation_repositories/PotentialRepositoryImpl.hpp"

PotentialRepositoryImpl::PotentialRepositoryImpl() {
    db = ServiceLocator::getInstance().get<DataSource>().getDB();

    if (!db) {
        std::cerr << "Failed to open database." << std::endl;
        return;
    }

    constexpr std::string_view initializePotentialsTableSQL =
        "CREATE TABLE IF NOT EXISTS Potentials ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "type INTEGER NOT NULL, "
        "expression TEXT NOT NULL, "
        "constants TEXT NOT NULL, "
        "timestamp TEXT NOT NULL);";
    try {
        db->exec(initializePotentialsTableSQL.data());

        // Check if Potentials table is empty
        SQLite::Statement checkStmt(*db, "SELECT COUNT(*) FROM Potentials;");
        checkStmt.executeStep();
        if (checkStmt.getColumn(0).getInt() == 0) {
            insertInitialPotentials();
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "[PotentialRepository] Error creating table: " << e.what()
                  << std::endl;
    }
}

size_t PotentialRepositoryImpl::add(const Potential &item) const {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return 0; // Indicate error
    }

    try {
        SQLite::Statement query(
            *db, "INSERT INTO Potentials (name, type, expression, "
                 "constants, "
                 "timestamp) VALUES (?, ?, ?, ?, datetime('now'));");

        query.bind(1, item.getName());
        query.bind(2, static_cast<int32_t>(item.getType()));
        query.bind(3, item.getExpression());
        query.bind(4, item.serializeConstants());
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error inserting potential: " << e.what() << std::endl;
        return 0; // Indicate error
    }

    return db->getLastInsertRowid();
}

void PotentialRepositoryImpl::remove(const size_t id) const {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return;
    }

    try {
        SQLite::Statement query(*db, "DELETE FROM Potentials WHERE id = ?;");
        query.bind(1, static_cast<int>(id));
        query.exec();
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error deleting potential: " << e.what() << std::endl;
    }
}

void PotentialRepositoryImpl::insertInitialPotentials() const {
    std::array<Potential, 1> defaults = {Potential(
        0, "Coulomb Potential", PotentialType::Coulomb, "-1/r^2", {})};

    for (const auto &pot : defaults) {
        add(pot);
    }
}

std::vector<std::unique_ptr<Potential>>
PotentialRepositoryImpl::getAll() const {
    if (!db) {
        std::cerr << "Database not initialized." << std::endl;
        return {};
    }

    std::vector<std::unique_ptr<Potential>> potentials;

    try {
        SQLite::Statement query(
            *db,
            "SELECT id, name, expression, type, constants FROM Potentials;");

        while (query.executeStep()) {
            auto potential = std::make_unique<Potential>();
            potential->setId(query.getColumn(0).getInt());
            potential->setName(query.getColumn(1).getText());
            potential->setExpression(query.getColumn(2).getText());
            potential->setType(
                static_cast<PotentialType>(query.getColumn(3).getInt()));
            auto potentialConstants =
                nlohmann::json::parse(query.getColumn(4).getText());
            potential->setConstants(potentialConstants);
            potentials.push_back(std::move(potential));
        }
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error retrieving potentials: " << e.what() << std::endl;
    }

    return potentials;
}
