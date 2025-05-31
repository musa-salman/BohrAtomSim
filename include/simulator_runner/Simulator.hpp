#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <boost/asio.hpp>
#include <memory>
#include <thread>

#include "service_locator/ServiceLocator.hpp"
#include "simulator_runner/ISimulator.hpp"
#include "simulator_runner/Simulation.hpp"
#include "steppers/SimulationStepper.hpp"
#include "steppers/State2D.hpp"
#include "steppers/State3D.hpp"
#include "steppers/StateFactory.hpp"
#include "steppers/StepperFactory.hpp"

class Simulator : public ISimulator {
  public:
    explicit Simulator(int numThreads);

    ~Simulator();

    void simulateOrbit(Simulation &simulation,
                       std::function<void()> &&onCompletion);

    void pauseSimulation(size_t id);

    void resumeSimulation(size_t id);

    void stopSimulation(size_t id);

    template <bool Is3D, bool IsRel, bool IsQuant, typename... Tags>
    void dispatch_by_potential_type(const Simulation &sim,
                                    const StepperCommonConfig &commonConfig,
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
                        ServiceLocator::getInstance()
                            .get<StepperFactory>()
                            .create(State3D<IsRel, IsQuant, Pot>(
                                        sim.getR0(), sim.getV0(),
                                        std::move(factory)),
                                    commonConfig);
                } else {
                    eom::Vector2 r0{sim.getR0().x, sim.getR0().y};
                    eom::Vector2 v0{sim.getV0().x, sim.getV0().y};
                    steppers[sim.getId()] =
                        ServiceLocator::getInstance()
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
                    const StepperCommonConfig &commonConfig) {
        dispatch_by_potential_type<Is3D, IsRel, IsQuant>(
            sim, commonConfig, std::tuple<CoulombTag, GeneralExpressionTag>{});
    }

  private:
    boost::asio::io_context ioContext;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
        workGuard;
    std::vector<std::jthread> workers;

    std::unordered_map<size_t, std::unique_ptr<SimulationStepper>> steppers;
};

#endif // SIMULATOR_HPP