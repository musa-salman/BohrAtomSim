#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "simulator_runner/Simulator.hpp"

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
    std::shared_ptr<sim_ctx> ctx, const electron_orbit &orbit,
    const std::function<void(std::shared_ptr<sim_ctx>, const electron_orbit &)>
        &simulateFunction) {
    boost::asio::post(ioContext, [ctx, orbit, simulateFunction]() {
        simulateFunction(ctx, orbit);
    });
}
