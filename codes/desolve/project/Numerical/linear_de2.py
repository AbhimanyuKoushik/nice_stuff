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
    """
    t = np.asarray(t)  # Ensure t is an array
    fractional = np.mod(t, timePeriod) / timePeriod
    return np.where(fractional < dutyratio, Amplitude, 0)


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
    sigma = (stepsize) * (timePeriod)  # Set Gaussian width based on period (1% of the period)

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

def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    delta_height = Amplitude / stepsize
    return np.where(np.abs(np.mod(t, timePeriod)) < stepsize, delta_height, 
                    np.where(np.abs(np.mod(t, timePeriod) - dutyratio) < stepsize / 2, -delta_height, 0))

def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    return (squarewave(t + stepsize/2, Amplitude, timePeriod, dutyratio) - 
            squarewave(t - stepsize/2, Amplitude, timePeriod, dutyratio))/stepsize
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
