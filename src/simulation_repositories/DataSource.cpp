#include <iostream>
#include <memory>

#include "SQLiteCpp/Database.h"
#include "simulation_repositories/DataSource.hpp"

std::unique_ptr<DataSource> DataSource::instance = nullptr;

DataSource::DataSource() {
    try {
        db = std::make_shared<SQLite::Database>(
            filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error opening database: " << e.what() << std::endl;
    }
}

DataSource *DataSource::getInstance() {
    if (!instance) {
        instance.reset(new DataSource());
    }
    return instance.get();
}

std::shared_ptr<SQLite::Database> DataSource::getDB() { return db; }
