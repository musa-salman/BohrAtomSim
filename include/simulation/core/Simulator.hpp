#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "ISimulator.hpp"

#include "physics/state/State2D.hpp"
#include "physics/state/State3D.hpp"

#include "physics/math/Vector2.hpp"
#include "physics/potential/PotentialTraits.hpp"
#include "physics/stepper/SimulationStepper.hpp"
#include "physics/stepper/StepperFactory.hpp"
#include "simulation/model/Potential.hpp"
#include "simulation/model/Simulation.hpp"
#include "utils/ServiceLocator.hpp"

namespace simulation::core {

using namespace physics::potential;
using namespace physics::state;
using namespace physics::stepper;
using namespace simulation::model;
using namespace utils;

class Simulator : public ISimulator {
  public:
    explicit Simulator(int numThreads);

    ~Simulator();

    void simulateOrbit(Simulation &simulation,
                       std::function<void()> &&onCompletion) override;

    void pauseSimulation(size_t id) override;

    void resumeSimulation(Simulation &simulation) override;

    void stopSimulation(size_t id) override;

    template <bool Is3D, bool IsRel, bool IsQuant, typename... Tags>
    void dispatch_by_potential_type(
        const Simulation &sim,
        const physics::stepper::StepperCommonConfig &commonConfig,
        std::tuple<Tags...>) {
        const Potential &potential = sim.getPotential();
        const auto &constants = sim.getConstantValues();

        auto try_dispatch = [&](auto tag) {
            using Traits = PotentialTraits<std::decay_t<decltype(tag)>>;
            if (potential.getType() == Traits::type) {
                auto factory = Traits::make_factory(potential, constants);
                using Pot =
                    typename Traits::template PotType<decltype(factory)>;

                if constexpr (Is3D) {
                    steppers[sim.getId()] =
                        utils::ServiceLocator::getInstance()
                            .get<StepperFactory>()
                            .create(State3D<IsRel, IsQuant, Pot>(
                                        sim.getR0(), sim.getV0(),
                                        std::move(factory)),
                                    commonConfig);
                } else {
                    physics::math::Vector2 r0{sim.getR0().x, sim.getR0().y};
                    physics::math::Vector2 v0{sim.getV0().x, sim.getV0().y};
                    steppers[sim.getId()] =
                        utils::ServiceLocator::getInstance()
                            .get<StepperFactory>()
                            .create(State2D<IsRel, IsQuant, Pot>(
                                        r0, v0, std::move(factory)),
                                    commonConfig);
                }
                return true;
            }
            return false;
        };

        (void(std::initializer_list<int>{(try_dispatch(Tags{}), 0)...}));
    }

    template <bool Is3D, bool IsRel, bool IsQuant>
    void make_state(const Simulation &sim,
                    const physics::stepper::StepperCommonConfig &commonConfig) {
        dispatch_by_potential_type<Is3D, IsRel, IsQuant>(
            sim, commonConfig, std::tuple<CoulombTag, GeneralExpressionTag>{});
    }

  private:
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        workGuard;
    std::vector<std::jthread> workers;

    std::unordered_map<size_t,
                       std::unique_ptr<physics::stepper::SimulationStepper>>
        steppers;
};
} // namespace simulation::core

#endif // SIMULATOR_HPP