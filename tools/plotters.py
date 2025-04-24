import matplotlib.pyplot as plt
import numpy as np
from tools.utils import calculate_delta_psi

def plot3d_electron_points(points, plane_x, plane_y, plane_z):
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection="3d")

    ax.scatter(
        points[:, 0], points[:, 1], points[:, 2], color="r", label="Electron Points"
    )

    ax.plot_surface(
        plane_x, plane_y, plane_z, alpha=0.5, color="blue", label="Fitted Plane"
    )

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title("Plane Fitting Visualization")
    ax.legend()
    plt.show()

def plots_delta_psi(angulars: list, results: list, title: str):
    plt.rcParams['text.usetex'] = False

    plt.figure()
    plt.ylabel(r"$10^4\psi(r=r_{\text{max}})$", fontdict={'color': 'black', 'weight': 'normal'})
    plt.xlabel('Revolutions Number', fontdict={'color': 'black', 'weight': 'normal'})
    plt.xticks(fontsize=12)

    scaled = 10 ** 4

    markers = ['x', 's', 'D', 'o']

    aungular_markers = {}

    for angular, result in zip(angulars, results):
        delta_psi = calculate_delta_psi(angular)
        scaled_delta_psi = scaled * delta_psi
        scaled_expected_delta_psi = np.array([scaled_delta_psi * i for i in range(0, len(result))])
        scaled_result = scaled * result

        if angular not in aungular_markers:
            aungular_markers[angular] = markers.pop()
            plt.plot(np.arange(len(result)), scaled_expected_delta_psi, label=f'Theoretical (k={angular})')
            plt.scatter(np.arange(len(result)), scaled_result, label=f'Simulated (k={angular})', marker=aungular_markers[angular])

            continue

        plt.scatter(np.arange(len(result)), scaled_result, label=f'Simulated (k={angular})', marker=aungular_markers[angular])

    plt.show()

def plot_delta_psi(angular: int, result: np.array, title: str):
    plt.rcParams['text.usetex'] = False

    plt.figure()
    plt.ylabel(r"$10\psi(r=r_{\text{max}})$", fontdict={'color': 'black', 'weight': 'normal'})
    plt.xlabel('revolutions number', fontdict={'color': 'black', 'weight': 'normal'})
    plt.xticks(fontsize=12)

    scaled = 10

    delta_psi = calculate_delta_psi(angular)
    scaled_delta_psi = scaled * delta_psi
    scaled_expected_delta_psi = np.array([scaled_delta_psi * i for i in range(1, len(result) + 1)])
    scaled_result = scaled * result

    plt.scatter(np.arange(len(result)), scaled_result, label=f'Simulated', color='red')
    plt.plot(np.arange(len(result)), scaled_expected_delta_psi, label='Theoretical', color='black')
    plt.legend()
    plt.title(title)
    plt.show()
