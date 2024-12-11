from abc import ABC, abstractmethod
from enum import Enum

import numpy as np
import matplotlib.pyplot as plt

from bohr_model import simulate


class Electron(ABC):
    def __init__(self, principle: int, angular: int):
        self.principle = principle
        self.angular = angular

    @abstractmethod
    def get_quantum_numbers(self):
        pass


class Electron2D(Electron):
    def __init__(self, principle: int, angular: int):
        super().__init__(principle, angular)

    def get_quantum_numbers(self):
        return (self.principle, self.angular, -1, 0)

    def __hash__(self) -> int:
        return self.principle | self.angular << 8


class Electron3D(Electron):
    def __init__(self, principle: int, angular: int, magnetic: int):
        super().__init__(principle, angular)
        self.magnetic = magnetic

    def get_quantum_numbers(self):
        return (self.principle, self.angular, self.magnetic, 0)

    def __hash__(self) -> int:
        return self.principle | self.angular << 8 | self.magnetic << 16


class Electron3DSpin(Electron3D):
    def __init__(self, principle: int, angular: int, magnetic: int, spin: int):
        super().__init__(principle, angular, magnetic)
        self.spin = spin

    def get_quantum_numbers(self):
        return (self.principle, self.angular, self.magnetic, self.spin)

    def __hash__(self) -> int:
        return (
            self.principle | self.angular << 8 | self.magnetic << 16 | self.spin << 24
        )


class SimType(Enum):
    POLAR = 0
    POLAR_REL = 1
    SPHERICAL = 2
    SPHERICAL_REL = 3


class SimulationResult:
    def __init__(
        self,
        results: dict,
        revolutions: float,
        time_interval: float,
        sim_type: SimType,
        delta_psi_mode: bool,
        record_interval: float,
    ):
        self._results = results
        self._revolutions = revolutions
        self._time_interval = time_interval
        self._sim_type = sim_type
        self._delta_psi_mode = delta_psi_mode
        self._record_interval = record_interval

    def get_results(self):
        return self.results

    def get_result(self, key: str):
        return self.results[key]

    def get_revolutions(self):
        return self._revolutions

    def get_time_interval(self):
        return self._time_interval

    def get_sim_type(self):
        return self._sim_type

    def get_delta_psi_mode(self):
        return self._delta_psi_mode

    def get_record_interval(self):
        return self._record_interval


class Simulation:
    def __init__(
        self,
        sim_type: SimType,
        delta_psi_mode: bool,
        electrons: list[Electron] = None,
        revolutions: float = 10,
        time_interval: float = 1e-6,
        record_interval: float = 1000,
    ):
        self._revolutions = revolutions
        self._sim_type = sim_type
        self._time_interval = time_interval
        self._delta_psi_mode = delta_psi_mode
        self._record_interval = record_interval
        if electrons is None:
            electrons = []
        self._electrons = electrons
        self._results = None

    def run(self):
        results = simulate(
            self._revolutions,
            self._time_interval,
            self._sim_type.value,
            self._delta_psi_mode,
            self._record_interval,
            [electron.get_quantum_numbers() for electron in self._electrons],
        )

        self._results = SimulationResult(
            results,
            self._revolutions,
            self._time_interval,
            self._sim_type,
            self._delta_psi_mode,
            self._record_interval,
        )

    def add_electron(self, electron: Electron):
        self._electrons.append(electron)

    def get_results(self) -> SimulationResult:
        return self._results


class SaveResults:
    def __init__(self, results: SimulationResult):
        self._results = results

    def save(self, file_name: str):
        np.save(file_name, self._results.get_results())


class ResultsPlotter(ABC):
    def __init__(self, results: SimulationResult):
        self._results = results

    @abstractmethod
    def create_plot(self) -> plt.Figure:
        pass


class ResultsPlotter2D(ResultsPlotter):
    def create_plot(self) -> plt.Figure:
        fig, ax = plt.subplots()
        for key, value in self._results.get_results().items():
            ax.plot(value)
        return fig


class ResultsPlotter3D(ResultsPlotter):
    def plot(self):
        pass


class ResultsPlotterRel(ResultsPlotter):
    def plot(self):
        pass
