#include <boost/asio.hpp>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulator_runner/Simulator.hpp"

#include "simulation_2d/simulation_2d.h"

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
    const sim2d_ctx ctx,                        // NOSONAR
    const std::function<void()> onCompletion) { // NOSONAR
    boost::asio::post(ioContext, [ctx, onCompletion]() {
        simulate_2d_electron_motion(ctx);

        onCompletion();
    });
}
