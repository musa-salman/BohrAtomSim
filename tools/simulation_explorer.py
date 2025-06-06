import sqlite3
import pandas as pd
import numpy as np
import os
from tools.utils import load_simulation_data
from enum import IntEnum

class SimulationType(IntEnum):
    NR2D = 0
    R2D = 1
    NR3D = 2
    R3D = 3
    RS3D = 4

class SimulationDB:
    def __init__(self, path='..'):
        self.path = os.path.abspath(path)
        self.simulations_folder = os.path.join(self.path, 'simulations')
        self.db_path = os.path.join(self.path, 'simulations.db')
        self.conn = sqlite3.connect(self.db_path)
        self.df = self._load_db()

    def _load_db(self):
        query = "SELECT * FROM Simulations"
        return pd.read_sql_query(query, self.conn)

    def filter_recent(self, limit=5):
        return self.df.sort_values(by='timestamp', ascending=False).head(limit)
    
    def contains_simulation_id(self, sim_id: int):
        return sim_id in self.df['id'].values

    def get_by_id(self, sim_id: int):
        return self.df[self.df['id'] == sim_id].squeeze()

    def get_simulation_path(self, sim_id: int):
        return os.path.join(self.simulations_folder, f"{sim_id}.bin")

    def load_simulation(self, sim_id: int):
        path = self.get_simulation_path(sim_id)
        if os.path.exists(path):
            return load_simulation_data(path)
        else:
            raise FileNotFoundError(f"No simulation data found at {path}")

    def show_all(self):
        return self.df

    def __del__(self):
        if hasattr(self, 'conn') and self.conn:
            self.conn.close()