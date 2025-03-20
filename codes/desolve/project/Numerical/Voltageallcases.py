import linear_de
import numpy as np
import matplotlib.pyplot as plt

# Square wave generator
def SquareWave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

# Numerical derivative function
def numerical_derivative(y, stepsize):
    dy = [(y[i + 1] - y[i]) / stepsize for i in range(len(y) - 1)]  # Forward difference
    dy = np.append(dy, dy[-1])  # Repeat the last value to match the length
    return np.array(dy)

# ODE functions for different cases
def f_case1(t, y, alpha, T, Amplitude):  # L = R (τ = T)
    R, L = 10, 10
    return SquareWave(t, Amplitude, T, alpha) / L - (R * y) / L

def f_case2(t, y, alpha, T, Amplitude):  # R >> L (τ < T)
    R, L = 300, 10
    return SquareWave(t, Amplitude, T, alpha) / L - (R * y) / L

def f_case3(t, y, alpha, T, Amplitude):  # L >> R (τ >> T)
    R, L = 10, 500
    return SquareWave(t, Amplitude, T, alpha) / L - (R * y) / L

def f_case4(t, y, alpha, T, Amplitude):  # L > R (τ > T)
    R, L = 10, 50
    return SquareWave(t, Amplitude, T, alpha) / L - (R * y) / L

# Parameters
t_init = 0
y0 = 0
t_max = 0.1
stepsize = 5e-7
T = 0.01
Amplitude = 10
alphas = [0.1, 0.5, 0.9]

# Solver methods
methods = {
    "Forward Euler": "EulerForward",
    "Backward Euler": "EulerBackward",
    "RK2": "RK2",
    "RK4": "RK4",
    "Trapezoidal": "Trapezoidal"
}

# Iterate over methods and alpha values
for method_name, solver_method in methods.items():
    for alpha in alphas:
        fig, axes = plt.subplots(2, 2, figsize=(12, 8))
        fig.suptitle(f"{method_name} - Voltage Response (α = {alpha})", fontsize=16)

        cases = [
            (f_case1, "Case 1: L = R (τ = T)"),
            (f_case2, "Case 2: R >> L (τ < T)"),
            (f_case3, "Case 3: L >> R (τ >> T)"),
            (f_case4, "Case 4: L > R (τ > T)")
        ]

        for i, (case_func, case_title) in enumerate(cases):
            # Create solver instance
            solver = linear_de.LinearDE(
                lambda t, y: case_func(t, y, alpha, T, Amplitude),
                t_init, y0, t_max, stepsize
            )

            # Solve for current
            I_t = np.array(getattr(solver, solver_method)())
            t_values = np.arange(t_init, t_max, stepsize)

            # Compute voltage as L * dI/dt
            L = [10, 10, 500, 50][i]  # L values corresponding to each case
            V_t = L * numerical_derivative(I_t, stepsize)

            # Ensure x and y have the same length
            min_len = min(len(t_values), len(V_t))
            t_values = t_values[:min_len]
            V_t = V_t[:min_len]

            # Plot customization
            ax = axes[i // 2, i % 2]
            ax.plot(t_values, V_t, label=f'$V(t)$', color='black', linewidth=1)
            ax.set_title(case_title, fontsize=12)
            ax.set_xlabel('Time (t)', fontsize=10)
            ax.set_ylabel('$V(t)$ (V)', fontsize=10)
            ax.legend()
            ax.grid(True)

        plt.tight_layout(rect=[0, 0, 1, 0.96])
        plt.show()
