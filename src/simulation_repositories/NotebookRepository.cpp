#include <filesystem>
#include <format>
#include <iostream>

#include "simulation_repositories/NotebookRepository.hpp"

// Constructor: Open SQLite database and create tables
NotebookRepository::NotebookRepository() {
    // db = DataSource::getInstance()->getDB();

    // Create Notebooks table
    std::string sql_notebook = "CREATE TABLE IF NOT EXISTS Notebooks ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               "name TEXT NOT NULL, "
                               "description TEXT NOT NULL);";

    // Create Plots table
    std::string sql_plot =
        "CREATE TABLE IF NOT EXISTS Plots ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "notebook_id INTEGER, "
        "name TEXT NOT NULL, "
        "description TEXT NOT NULL, "
        "FOREIGN KEY (notebook_id) REFERENCES Notebooks(id));";

    char *error_message;
    if (sqlite3_exec(db, sql_notebook.c_str(), nullptr, nullptr,
                     &error_message) != SQLITE_OK ||
        sqlite3_exec(db, sql_plot.c_str(), nullptr, nullptr, &error_message) !=
            SQLITE_OK) {
        std::cerr << "Error creating tables: " << error_message << std::endl;
        sqlite3_free(error_message);
    }
}

// Destructor: Close database connection
NotebookRepository::~NotebookRepository() {
    if (db) {
        sqlite3_close(db);
    }
}

// Create a new notebook
size_t NotebookRepository::createNotebook(const Notebook &notebook) {
    std::string sql = std::format(
        "INSERT INTO Notebooks (name, description) VALUES ('{}', '{}');",
        notebook.name, notebook.description);

    if (char *error_message; sqlite3_exec(db, sql.c_str(), nullptr, nullptr,
                                          &error_message) != SQLITE_OK) {
        std::cerr << "Error inserting notebook: " << error_message << std::endl;
        sqlite3_free(error_message);
        return -1;
    }

    return sqlite3_last_insert_rowid(db);
}

// Add a plot to an existing notebook
size_t NotebookRepository::addPlotToNotebook(const Plot &plot) {
    std::string sql =
        std::format("INSERT INTO Plots (notebook_id, name, description) "
                    "VALUES ({}, '{}', '{}');",
                    plot.notebook_id, plot.name, plot.description);

    if (char *error_message; sqlite3_exec(db, sql.c_str(), nullptr, nullptr,
                                          &error_message) != SQLITE_OK) {
        std::cerr << "Error inserting plot: " << error_message << std::endl;
        sqlite3_free(error_message);
        return -1;
    }

    return sqlite3_last_insert_rowid(db);
}

// Callback function for retrieving notebooks
int NotebookRepository::notebookCallback(void *data, [[maybe_unused]] int argc,
                                         char **argv, char **) {
    auto *notebooks = static_cast<std::vector<Notebook> *>(data);
    Notebook nb;
    nb.id = argv[0] ? std::stoul(argv[0]) : 0;
    nb.name = argv[1] ? argv[1] : "";
    nb.description = argv[2] ? argv[2] : "";
    notebooks->push_back(nb);
    return 0;
}

// Retrieve all notebooks
std::vector<Notebook> NotebookRepository::getNotebooks() {
    std::vector<Notebook> notebooks;
    std::string sql = "SELECT id, name, description FROM Notebooks;";

    if (char *error_message;
        sqlite3_exec(db, sql.c_str(), notebookCallback, &notebooks,
                     &error_message) != SQLITE_OK) {
        std::cerr << "Error retrieving notebooks: " << error_message
                  << std::endl;
        sqlite3_free(error_message);
    }

    return notebooks;
}

// Callback function for retrieving plots
int NotebookRepository::plotCallback(void *data, [[maybe_unused]] int argc,
                                     char **argv, char **) {
    auto *plots = static_cast<std::vector<std::shared_ptr<Plot>> *>(data);
    auto p = std::make_shared<Plot>();
    p->id = argv[0] ? std::stoul(argv[0]) : 0;
    p->notebook_id = argv[1] ? std::stoul(argv[1]) : 0;
    p->name = argv[2] ? argv[2] : "";
    p->description = argv[3] ? argv[3] : "";
    plots->push_back(p);
    return 0;
}

void NotebookRepository::removeNotebook(size_t notebook_id) {
    std::string sql =
        std::format("DELETE FROM Notebooks WHERE id={}; DELETE FROM Plots "
                    "WHERE notebook_id={};",
                    notebook_id, notebook_id);

    std::string folder = std::format("{}/plots/{}", DB_PATH, notebook_id);
    std::filesystem::remove_all(folder);

    if (char *error_message; sqlite3_exec(db, sql.c_str(), nullptr, nullptr,
                                          &error_message) != SQLITE_OK) {
        std::cerr << "Error deleting notebook: " << error_message << std::endl;
        sqlite3_free(error_message);
    }
}

void NotebookRepository::removePlot(size_t plot_id) {
    std::string sql = std::format("DELETE FROM Plots WHERE id={};", plot_id);

    if (char *error_message; sqlite3_exec(db, sql.c_str(), nullptr, nullptr,
                                          &error_message) != SQLITE_OK) {
        std::cerr << "Error deleting plot: " << error_message << std::endl;
        sqlite3_free(error_message);
    }
}

// Retrieve plots by notebook ID
std::vector<std::shared_ptr<Plot>>
NotebookRepository::getPlotsByNotebook(size_t notebook_id, bool cached) {
    if (cached && notebook_plots.contains(notebook_id)) {
        return notebook_plots[notebook_id];
    }

    std::vector<std::shared_ptr<Plot>> plots;
    std::string sql = std::format("SELECT id, notebook_id, name, description "
                                  " FROM Plots WHERE notebook_id={};",
                                  notebook_id);

    if (char *error_message; sqlite3_exec(db, sql.c_str(), plotCallback, &plots,
                                          &error_message) != SQLITE_OK) {
        std::cerr << "Error retrieving plots: " << error_message << std::endl;
        sqlite3_free(error_message);
    }

    return plots;
}
