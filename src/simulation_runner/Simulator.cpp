#include <boost/asio.hpp>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulator_runner/Simulator.hpp"
#include "utils/iterator.h"

#include "polar/polar_sim.h"
#include "polar/polar_sim_rel.h"
#include "spherical/spherical_sim.h"
#include "spherical/spherical_sim_rel.h"
#include "spin/spin_sim.h"

Simulator::Simulator()
    : ioContext(1), workGuard(boost::asio::make_work_guard(ioContext)) {
    workers.emplace_back([this]() { ioContext.run(); });
}

Simulator::Simulator(int numThreads)
    : ioContext(numThreads),
      workGuard(boost::asio::make_work_guard(ioContext)) {
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this]() { ioContext.run(); });
    }
}

Simulator::~Simulator() {
    ioContext.stop();
    for (auto &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void Simulator::simulateOrbit(
    const sim_ctx ctx, int type,                // NOSONAR
    const std::function<void()> onCompletion) { // NOSONAR
    boost::asio::post(ioContext, [ctx, type, onCompletion]() {
        if (type == POLAR) {
            simulate_polar_orbit(ctx);
        } else if (type == REL_POLAR) {
            simulate_polar_orbit_rel(ctx);
        } else if (type == SPHERICAL) {
            simulate_spherical_orbit(ctx);
        } else if (type == REL_SPHERICAL) {
            simulate_spherical_rel_orbit(ctx);
        } else if (type == SPIN) {
            simulate_spin_orbit(ctx);
        }
        onCompletion();
    });
}
