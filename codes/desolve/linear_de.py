#!/usr/bin/env python

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

def squarewave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

x0 = 0
y0 = 0
R = 0.1
finalx = 100
stepsize = 0.01
alpha = 0.5
T = 1
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return squarewave(x, Amplitude, T, alpha) -R*y # dy/dx = y

# Create ODE solver instance
solver = linear_de.LinearDE(f, x0, y0, finalx, stepsize)

# Compute solutions
y_euler = np.array(solver.EulerForward())  # Convert to NumPy array
y_rk2 = np.array(solver.RK2())  # Convert to NumPy array
y_rk4 = np.array(solver.RK4())

# Generate x values (use np.arange for proper step size alignment)
x_values = np.arange(x0, finalx, stepsize)

# Plot results
plt.plot(x_values, y_euler, label="Euler's Method", linestyle="dashed", color="blue")
plt.plot(x_values, y_rk2, label="RK2 Method", linestyle="dashed", color="red")
plt.plot(x_values, y_rk4, label="RK4 Method", linestyle = "dashed", color = "black")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK2")
plt.legend()
plt.grid()
plt.show()
