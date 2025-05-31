#ifndef POTENTIAL_REPOSITORY_HPP
#define POTENTIAL_REPOSITORY_HPP

#include "simulation_repositories/Repository.hpp"
#include "steppers/Potential.hpp"

class PotentialRepository : public Repository<Potential> {};

#endif // POTENTIAL_REPOSITORY_HPP