#ifndef DATA_SOURCE_HPP
#define DATA_SOURCE_HPP

#include <format>
#include <memory>
#include <sqlite3.h>
#include <string>

class DataSource {
  private:
    static std::unique_ptr<DataSource> instance;
    sqlite3 *db;
    const std::string filename = std::format("{}/simulations.db", DB_PATH);

    DataSource();

  public:
    DataSource(const DataSource &) = delete;
    DataSource &operator=(const DataSource &) = delete;

    static DataSource *getInstance();

    sqlite3 *getDB();

    ~DataSource();
};

#endif // DATA_SOURCE_HPP
