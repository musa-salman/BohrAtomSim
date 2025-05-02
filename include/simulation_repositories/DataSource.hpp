#ifndef DATA_SOURCE_HPP
#define DATA_SOURCE_HPP

#include "SQLiteCpp/Database.h"
#include <format>
#include <memory>
#include <string>

class DataSource {
  private:
    std::shared_ptr<SQLite::Database> db;
    const std::string filename = std::format("{}/simulations.db", DB_PATH);

  public:
    DataSource();

    std::shared_ptr<SQLite::Database> getDB();
};

#endif // DATA_SOURCE_HPP
