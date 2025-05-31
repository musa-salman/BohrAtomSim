#ifndef POTENTIAL_REPOSITORY_IMPL_HPP
#define POTENTIAL_REPOSITORY_IMPL_HPP

#include "SQLiteCpp/Database.h"
#include "simulation_repositories/PotentialRepository.hpp"

class PotentialRepositoryImpl : public PotentialRepository {
    std::shared_ptr<SQLite::Database> db;
    void insertInitialPotentials() const;

  public:
    explicit PotentialRepositoryImpl();

    size_t add(const Potential &item) const override;
    void remove(const size_t id) const override;
    std::vector<std::unique_ptr<Potential>> getAll() const override;
};

#endif // POTENTIAL_REPOSITORY_IMPL_HPP