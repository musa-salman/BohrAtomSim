# Bohr Atom Simulation

This project implements a **Bohr atom model simulation** in **2D and 3D**, supporting **relativistic and non-relativistic** scenarios.

## Features

- **Supports 2D and 3D simulations** for relativistic and non-relativistic electron motion.
- **Highly optimized C-based simulation core** for performance and efficiency.
- **C++ user interface (UI)** for managing simulations and visualizing results in real-time.
- **Multi-threaded execution** using Boost/ASIO.
- **SQLite-based metadata storage** for efficient simulation management.
- **Binary storage of simulation results** for performance optimization.

## Technologies Used

### Core Simulation (C)
- **Optimized for performance** using vectorized elementary functions with **Sleef**.

### User Interface (C++)
- **ImGui** for a modern, intuitive user interface.
- **ImPlot** for **real-time plotting** of simulation results.
- **Matplot++** for generating scientific plots.
- **Boost/ASIO** for multi-threading and handling multiple simulations simultaneously.
- **SQLite3** for storing metadata related to simulation configurations.

## Installation

### Prerequisites
- **CMake (≥3.10)**
- **LLVM Clang compiler**
- **Boost (≥1.83.0)**
- **GLFW and OpenGL**
- **Sleef for mathematical functions**
- **SQLite3 for metadata storage**
- **Matplot++ for scientific plots**
- **stb** – A collection of single-file public domain libraries for image loading and texture handling.  
  - Used for **loading and rendering images** in the simulation.
  - **GitHub Repository**: [stb](https://github.com/nothings/stb)

### Building the Project

```sh
git clone https://github.com/musa-salman/BohrAtomSim.git
cd BohrAtomSim
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running the Simulation

To run the main application:

```sh
./bin/bohr_main
```

## Project Structure
```
BohrAtomSim/
├── src/          # C and C++ source files for simulation and UI
├── include/      # Header files
├── external/     # Third-party libraries (ImGui, ImPlot, Sleef, Boost, etc.)
├── config/       # Configuration files (e.g., ImGui settings)
├── bin/          # Compiled executable output
├── db/           # SQLite database for storing metadata, and simulation datasets results
└── CMakeLists.txt # CMake configuration
```

## Photos
![image](https://github.com/user-attachments/assets/d2fcb1ea-901e-4ee4-b072-d6bfea78d299)


![image](https://github.com/user-attachments/assets/347b914b-1c31-4ae5-af84-214c27c0e18a)

![image](https://github.com/user-attachments/assets/9dffd0bb-8c60-4154-90b3-f8b20fec8d72)

