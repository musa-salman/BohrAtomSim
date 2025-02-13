#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <boost/asio.hpp>
#include <thread>

#include "atom/atom_bohr_sim.h"

class Simulator {
  public:
    Simulator();
    explicit Simulator(int numThreads);

    ~Simulator();

    void simulateOrbit(const sim_ctx ctx, int type,
                       std::function<void()> onCompletion);

  private:
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        workGuard;
    std::vector<std::jthread> workers;
};

#endif // SIMULATOR_HPP