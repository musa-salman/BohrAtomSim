#pragma once

#include <SQLiteCpp/Database.h>

#include "PotentialRepository.hpp"
#include "simulation/model/Potential.hpp"

namespace storage::persistence {

class PotentialRepositoryImpl : public PotentialRepository {
    std::shared_ptr<SQLite::Database> db;
    void insertInitialPotentials() const;

  public:
    explicit PotentialRepositoryImpl();

    size_t add(const simulation::model::Potential &item) const override;
    void remove(const size_t id) const override;
    std::vector<std::unique_ptr<simulation::model::Potential>>
    getAll() const override;
};
} // namespace storage::persistence
