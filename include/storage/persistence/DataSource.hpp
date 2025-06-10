#pragma once

#include <SQLiteCpp/Database.h>
#include <format>
#include <memory>
#include <string>

namespace storage::persistence {

class DataSource {
  private:
    std::shared_ptr<SQLite::Database> db;
    const std::string filename = std::format("{}/simulations.db", DB_PATH);

  public:
    DataSource();

    std::shared_ptr<SQLite::Database> getDB();
};
} // namespace storage::persistence
