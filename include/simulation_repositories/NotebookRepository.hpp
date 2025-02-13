#ifndef NOTEBOOK_REPOSITORY_HPP
#define NOTEBOOK_REPOSITORY_HPP

#include <memory>
#include <sqlite3.h>
#include <string>
#include <unordered_map>
#include <vector>

struct Notebook {
    size_t id;
    std::string name;
    std::string description;
};

struct Plot {
    size_t id;
    size_t notebook_id;
    std::string name;
    std::string description;
};

class NotebookRepository {
  private:
    std::unordered_map<size_t, std::vector<std::shared_ptr<Plot>>>
        notebook_plots;

    sqlite3 *db;

    static int notebookCallback(void *data, int argc, char **argv,
                                char **colNames);
    static int plotCallback(void *data, int argc, char **argv, char **colNames);

  public:
    explicit NotebookRepository();
    ~NotebookRepository();

    size_t createNotebook(const Notebook &notebook);
    size_t addPlotToNotebook(const Plot &plot);

    void removeNotebook(size_t notebook_id);
    void removePlot(size_t plot_id);

    std::vector<Notebook> getNotebooks();
    std::vector<std::shared_ptr<Plot>> getPlotsByNotebook(size_t notebook_id,
                                                          bool cached = true);
};

#endif // NOTEBOOK_REPOSITORY_HPP
