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
def diff_squarewave1(t, Amplitude, timePeriod, dutyratio, stepsize):
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
    sigma = (timePeriod / 100) * stepsize # Set Gaussian width based on period (1% of the period)

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


def diff_squarewave2(t, Amplitude, timePeriod, dutyratio, stepsize):
    delta_height = Amplitude / stepsize
    return np.where((np.fabs(t % timePeriod) <= stepsize), delta_height, 
                    np.where(np.fabs(t % timePeriod - dutyratio) <= stepsize/2, -delta_height, 0))

# Define parameters
t_init = -1
t_max = 10
stepsize = 5e-4
T = 1.0        # Period of the square wave
Amplitude = 10
dutyratio = 0.1  # 40% duty cycle

# Generate time values
times = np.arange(t_init, t_max, stepsize)

# Compute the square wave
square_wave_signal = squarewave(times, Amplitude, T, dutyratio)

# Compute the Gaussian-based differentiation of the square wave
square_wave_derivative1 = diff_squarewave1(times, Amplitude, T, dutyratio, stepsize)
square_wave_derivative2 = diff_squarewave2(times, Amplitude, T, dutyratio, stepsize)

# Plot the results
plt.figure(figsize=(8, 5))
plt.plot(times, square_wave_signal, label="Square Wave", color="blue")
#plt.plot(times, square_wave_derivative1, label="Gaussian Derivative Approximation", linestyle="dashed", color="red")
plt.plot(times, square_wave_derivative2, label="Rectangular Derivative Approximation", linestyle="dashed", color="black")

plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.title("Square Wave and Its Gaussian-Based Differentiation")
plt.legend()
plt.grid()
plt.show()
