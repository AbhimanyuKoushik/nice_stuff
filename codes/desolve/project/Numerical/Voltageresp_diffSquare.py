import linear_de
import numpy as np
import matplotlib.pyplot as plt

# Define the square wave function
def squarewave(t, Amplitude, timePeriod, dutyratio):
    """
    Generate a square wave with a given amplitude, period, and duty ratio.
    """
    t = np.asarray(t)  # Ensure t is an array
    fractional = np.mod(t, timePeriod) / timePeriod
    return np.where(fractional < dutyratio, Amplitude, 0)

# Compute numerical derivative of the square wave
def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    """
    Compute a numerical derivative of the square wave.
    """
    t = np.asarray(t)  # Ensure t is an array
    y = squarewave(t, Amplitude, timePeriod, dutyratio)
    
    if len(y) < 2:
        raise ValueError("Time array must have at least two elements for differentiation.")
    
    return numerical_derivative(y, stepsize)

# Compute numerical derivative
def numerical_derivative(y, stepsize):
    if len(y) < 2:
        return np.array([0])  # Return zero if differentiation is not possible
    
    dy = np.diff(y) / stepsize  # First-order difference
    dy = np.append(dy, dy[-1])  # Maintain the same length as input
    return dy

# Define system parameters
t_init = 0
y0 = 10
R = 10
L = 10
t_max = 5
stepsize = 5e-4
alpha = 0.5
T = 1.0
Amplitude = 10

# Define function for the differential equation
def g(x, y):
    # Generate an array of two points for differentiation
    t_array = np.array([x, x + stepsize])  
    
    diff_values = diff_squarewave(t_array, Amplitude, T, alpha, stepsize)
    
    return diff_values[0] - (R * y) / L  # Take the first value

# Create ODE solver instance
solver = linear_de.LinearDE(g, t_init, y0, t_max, stepsize)

# Compute solutions
yEulerfwd = np.array(solver.EulerForward())  # Convert to NumPy array
yRK2 = np.array(solver.RK2())  # Convert to NumPy array
yRK4 = np.array(solver.RK4())
yEulerbkd = np.array(solver.EulerBackward())
yTrapezoidal = np.array(solver.Trapezoidal())

# Generate time values ensuring correct length
t_values = np.arange(t_init, t_max + stepsize, stepsize)

# Plot results
plt.plot(t_values, yEulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
plt.plot(t_values, yEulerbkd, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(t_values, yRK4, label="RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, yRK2, label="RK2 Method", linestyle = "dashed", color = "black")
plt.plot(t_values, yTrapezoidal, label="Trapezoidal", linestyle = "dashed", color = "purple")

plt.xlabel("Time (s)")
plt.ylabel("Current (A)")
plt.title("Voltage response of an Inductor to a square wave input")
plt.legend()
plt.grid()
plt.show()
