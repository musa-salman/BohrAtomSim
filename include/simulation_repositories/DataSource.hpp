#ifndef DATA_SOURCE_HPP
#define DATA_SOURCE_HPP

#include "SQLiteCpp/Database.h"
#include <format>
#include <memory>
#include <string>

class DataSource {
  private:
    static std::unique_ptr<DataSource> instance;
    std::shared_ptr<SQLite::Database> db;
    const std::string filename = std::format("{}/simulations.db", DB_PATH);

    DataSource();

  public:
    DataSource(const DataSource &) = delete;
    DataSource &operator=(const DataSource &) = delete;

    static DataSource *getInstance();

    std::shared_ptr<SQLite::Database> getDB();
};

#endif // DATA_SOURCE_HPP
