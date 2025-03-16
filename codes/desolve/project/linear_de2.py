import linear_de
import numpy as np
import matplotlib.pyplot as plt
import math

'''
def squarewave(t, Amplitude, timePeriod, Offset, timeShift = 0):
    w = 2*(np.pi/timePeriod)
    phase = w*timeShift
    return (Amplitude * np.sign(np.sin(w*t + phase)))*0.5 + Offset
'''

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
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

# Define the Gaussian Dirac delta function approximation
def gaussian_delta(t, center, sigma, Amplitude):
    """
    Approximate the Dirac delta function using a Gaussian.

    Parameters:
    t: Time array
    center: Center of the Gaussian peak
    sigma: Width of the Gaussian
    Amplitude: Scaling factor

    Returns:
    Array representing the Gaussian function
    """
    return (Amplitude / (sigma * np.sqrt(2 * np.pi))) * np.exp(-((t - center) ** 2) / (2 * sigma ** 2))

# Define the differentiation of a square wave using Gaussian pulses
def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    """
    Compute the approximate derivative of a square wave using Gaussian pulses.

    Parameters:
    t: Time array
    Amplitude: Amplitude of the square wave
    timePeriod: Period of the square wave
    dutyratio: Duty cycle of the square wave (0 to 1)

    Returns:
    Array representing the derivative of the square wave
    """
    sigma = (timePeriod / 100) * stepsize  # Set Gaussian width based on period (1% of the period)

    # Initialize result array
    result = np.zeros_like(t)

    # Calculate range of periods to consider
    t_min, t_max = np.min(t), np.max(t)
    start_period = int(np.floor(t_min / timePeriod)) - 1
    end_period = int(np.ceil(t_max / timePeriod)) + 1

    # Add Gaussian pulses at rising and falling edges
    for i in range(start_period, end_period + 1):
        rising_time = i * timePeriod
        falling_time = rising_time + dutyratio * timePeriod

        result += gaussian_delta(t, rising_time, sigma, Amplitude)   # Positive peak at rising edge
        result -= gaussian_delta(t, falling_time, sigma, Amplitude)  # Negative peak at falling edge

    return result

t_init = 0
y0 = 0
R = 10
L = 1e1
t_max = 10
stepsize = 5e-4
alpha = 0.1
T = 1.0
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return squarewave(x, Amplitude, T, alpha) / L - (R * y)/ L

def g(x, y):
    return diff_squarewave(x, Amplitude, T, alpha, stepsize) - (R * y) / L 

def numerical_derivative(y, stepsize):
    dy = [(y[i+1] - y[i]) / stepsize for i in range(len(y) - 1)]  # Forward difference method
    dy = np.append(dy, dy[-1])  # Repeat the last derivative value to match length
    x = np.linspace(t_init, t_max, len(dy))  # Ensure x matches y length
    return np.array(x), np.array(dy)

t_values = np.arange(t_init, t_max, stepsize)
y_squarewave = squarewave(t_values, Amplitude, T, alpha)
t_values, y_diff = numerical_derivative(y_squarewave, stepsize)
'''
# Create ODE solver1 instance
solver1 = linear_de.LinearDE(f, t_init, y0, t_max, stepsize)
solver2 = linear_de.LinearDE(g, t_init, y0, t_max, stepsize)

# Compute solutions
y_eulerfwd = np.array(solver1.EulerForward())  # Convert to NumPy array
y_rk2 = np.array(solver1.RK2())  # Convert to NumPy array
y_rk4 = np.array(solver1.RK4())
y_rk4derr = np.array(solver2.RK4())
y_eulerbkd = np.array(solver1.EulerBackward())
y_trap = np.array(solver1.Trapezoidal())

# Generate x values (use np.arange for proper step size alignment)
t_values, y_derrk4 = numerical_derivative(y_rk4, stepsize)

# Plot results

plt.plot(t_values, y_eulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
plt.plot(t_values, y_eulerbkd, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(t_values, y_rk4, label="RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, y_rk2, label="RK2 Method", linestyle = "dashed", color = "black")
plt.plot(t_values, y_trap, label="Trapezoidal", linestyle = "dashed", color = "purple")

plt.plot(t_values, y_rk4derr, label="Diff then RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, L * y_derrk4, label="RK4 Method then Diff", linestyle="dashed", color="black")
#plt.plot(t_values, g(t_values,0))
'''
plt.plot(t_values, y_diff)
plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK4")
plt.legend()
plt.grid()
plt.show()