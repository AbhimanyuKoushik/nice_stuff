#!/usr/bin/env python

import linear_de
import numpy as np
import matplotlib.pyplot as plt

# Define Python function f(x, y)
def f(x, y):
    return np.tan(y)  # dy/dx = y
x0 = 0.0
y0 = 1.0
finalx = 2.0
stepsize = 0.01

# Create ODE solver instance
solver = linear_de.LinearDE(f, x0, y0, finalx, stepsize)

# Compute solutions
y_euler = np.array(solver.euler_forward())  # Convert to NumPy array
y_rk2 = np.array(solver.rk2())  # Convert to NumPy array
error = np.abs(y_euler - y_rk2)

# Generate x values (use np.arange for proper step size alignment)
x_values = np.arange(x0, finalx, stepsize)

# Plot results
plt.plot(x_values, y_euler, label="Euler's Method", linestyle="dashed", color="blue")
plt.plot(x_values, y_rk2, label="RK2 Method", linestyle="solid", color="red")
#plt.plot(x_values, error, label="Error", linestyle = "dashed", color = "black")
plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK2")
plt.legend()
plt.grid()
plt.show()
