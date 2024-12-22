#include <boost/asio.hpp>
#include <thread>

#include "atom/atom_bohr_sim.h"
#include "atom/result_recorders.h"
#include "spherical/spherical_sim.h"

class Simulator {
  public:
    Simulator();
    explicit Simulator(int numThreads);

    ~Simulator();

    void
    simulateOrbit(sim_ctx *ctx, const electron_orbit &orbit,
                  const std::function<void(sim_ctx *, const electron_orbit &)>
                      &simulateFunction);

  private:
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        workGuard;
    std::vector<std::jthread> workers;
};
