#include <iostream>
#include <memory>

#include "simulation_repositories/DataSource.hpp"

std::unique_ptr<DataSource> DataSource::instance = nullptr;

DataSource::DataSource() : db(nullptr) {
    int exit = sqlite3_open(filename.c_str(), &db);
    if (exit) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db)
                  << std::endl;
        db = nullptr;
    }
}

DataSource *DataSource::getInstance() {
    if (!instance) {
        instance.reset(new DataSource());
    }
    return instance.get();
}

sqlite3 *DataSource::getDB() { return db; }

DataSource::~DataSource() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}
