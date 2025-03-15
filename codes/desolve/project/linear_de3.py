import linear_de
import numpy as np
import matplotlib.pyplot as plt

# Define the square wave function
def squarewave(t, Amplitude, timePeriod, dutyratio):
    """
    Generate a square wave with a given amplitude, period, and duty ratio.

    Parameters:
    t: Time array
    Amplitude: Amplitude of the square wave
    timePeriod: Period of the square wave
    dutyratio: Fraction of the period where the wave is high

    Returns:
    Array representing the square wave signal
    """
    fractional = np.mod(t, timePeriod) / timePeriod
    return np.where(fractional < dutyratio, Amplitude, 0)

# Define the derivative of the square wave
def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    """
    Compute a numerical derivative of the square wave.

    Parameters:
    t: Time array
    Amplitude: Amplitude of the square wave
    timePeriod: Period of the square wave
    dutyratio: Duty cycle of the square wave (0 to 1)
    stepsize: Small step size for differentiation

    Returns:
    Array representing the derivative of the square wave
    """
    return (squarewave(t, Amplitude, timePeriod, dutyratio) - 
            squarewave(t-stepsize, Amplitude, timePeriod, dutyratio)) / stepsize

# Parameters
t_init = 0
y0 = 0
R = 10
L = 10
t_max = 5
stepsize = 5e-4
alpha = 0.5
T = 1.0
Amplitude = 10

# Define the ODE functions
def f(x, y):
    return squarewave(x, Amplitude, T, alpha) / L - (R * y) / L

def g(x, y):
    return diff_squarewave(x, Amplitude, T, alpha, stepsize) - (R * y) / L 

# Compute numerical derivative
def numerical_derivative(y, stepsize):
    dy = np.diff(y) / stepsize  # Compute first-order difference
    dy = np.append(dy, dy[-1])  # Maintain same length
    return dy

# Create ODE solver instances
solver1 = linear_de.LinearDE(f, t_init, y0, t_max, stepsize)
solver2 = linear_de.LinearDE(g, t_init, y0, t_max, stepsize)

# Compute solutions
y_rk4 = np.array(solver1.RK4())
y_rk4derr = np.array(solver2.RK4())

# Generate time values
t_values = np.arange(t_init, t_max + stepsize, stepsize)

# Compute numerical derivative
y_derrk4 = numerical_derivative(y_rk4, stepsize)

# Plot results
plt.plot(t_values, y_rk4derr, label="Diff then RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, L * y_derrk4, label="RK4 Method then Diff", linestyle="dashed", color="black")

plt.xlabel("Time (s)")
plt.ylabel("Current (A)")
plt.title("Comparison of Differentiated Square Wave Responses")
plt.legend()
plt.grid()
plt.show()
