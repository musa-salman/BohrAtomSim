#pragma once

#include "Repository.hpp"
#include "simulation/model/Potential.hpp"

namespace storage::persistence {

class PotentialRepository : public Repository<simulation::model::Potential> {};
} // namespace storage::persistence
