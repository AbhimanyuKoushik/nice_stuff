import numpy as np
import matplotlib.pyplot as plt

# Square wave generator
def squarewave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

# Numerical differentiation for discrete sharp transitions
def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    diff = np.zeros_like(t)
    for i in range(1, len(t)):
        diff[i] = (squarewave(t[i], Amplitude, timePeriod, dutyratio) -
                   squarewave(t[i-1], Amplitude, timePeriod, dutyratio)) / stepsize
    return diff

# Function for RK4 solution of dy/dt = f(t, y)
def f(t, y, A, T, alpha, R, L):
    return squarewave(t, A, T, alpha) / L - (R * y) / L

# RK4 solver
def rk4_step(func, t, y, h, *args):
    k1 = h * func(t, y, *args)
    k2 = h * func(t + 0.5 * h, y + 0.5 * k1, *args)
    k3 = h * func(t + 0.5 * h, y + 0.5 * k2, *args)
    k4 = h * func(t + h, y + k3, *args)
    return y + (k1 + 2 * k2 + 2 * k3 + k4) / 6

# Parameters
t_init = 0
y0 = 0
R = 10       # Resistance
L = 1e1      # Inductance
t_max = 3
stepsize = 1e-3
alpha = 0.5  # Duty cycle
T = 1.0      # Period of square wave
Amplitude = 10

# Time array
times = np.arange(t_init, t_max, stepsize)
y_rk4 = np.zeros(len(times))

# Solve using RK4
for i in range(1, len(times)):
    y_rk4[i] = rk4_step(f, times[i-1], y_rk4[i-1], stepsize, Amplitude, T, alpha, R, L)

# Compute the numerical derivative of RK4 output
def numerical_derivative(y, stepsize):
    dy = np.diff(y) / stepsize  # Forward difference method
    dy = np.append(dy, dy[-1])  # Repeat last value for matching length
    return dy

y_rk4_derivative = L * numerical_derivative(y_rk4, stepsize)

# Generate the direct differentiation of the square wave
y_rk4_diff = diff_squarewave(times, Amplitude, T, alpha, stepsize)

# Plot results
plt.figure(figsize=(6, 5))
plt.plot(times, y_rk4_diff, label="Diff then RK4 Method", linestyle="dashed", color="red")
plt.plot(times, y_rk4_derivative, label="RK4 Method then Diff", linestyle="dashed", color="black")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK4")
plt.legend()
plt.grid()
plt.show()
