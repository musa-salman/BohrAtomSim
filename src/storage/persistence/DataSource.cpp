#include <SQLiteCpp/Database.h>
#include <iostream>
#include <memory>

#include "storage/persistence/DataSource.hpp"

namespace storage::persistence {

DataSource::DataSource() {
    try {
        db = std::make_shared<SQLite::Database>(
            filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    } catch (const SQLite::Exception &e) {
        std::cerr << "Error opening database: " << e.what() << std::endl;
    }
}

std::shared_ptr<SQLite::Database> DataSource::getDB() { return db; }
} // namespace storage::persistence