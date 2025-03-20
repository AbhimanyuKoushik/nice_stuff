import linear_de
import numpy as np
import matplotlib.pyplot as plt
import math

'''
def S(t, Amplitude, timePeriod, Offset, timeShift = 0):
    w = 2*(np.pi/timePeriod)
    phase = w*timeShift
    return (Amplitude * np.sign(np.sin(w*t + phase)))*0.5 + Offset
'''

def SquareWave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

t_init = -1
y0 = 0
R = 10
L = 1e1
t_max = 5
stepsize = 1e-3
alpha = 0.1
T = 1.0
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return SquareWave(x, Amplitude, T, alpha) / L - (R * y)/ L

def numerical_derivative(y, stepsize):
    dy = [(y[i+1] - y[i]) / stepsize for i in range(len(y) - 1)]  # Forward difference method
    dy = np.append(dy, dy[-1])  # Repeat the last derivative value to match length
    x = np.linspace(t_init, t_max, len(dy))  # Ensure x matches y length
    return np.array(x), np.array(dy)
    
# Create ODE solver instance
solver = linear_de.LinearDE(f, t_init, y0, t_max, stepsize)

# Compute solutions
yEulerfwd = np.array(solver.EulerForward())  # Convert to NumPy array
yRK2 = np.array(solver.RK2())  # Convert to NumPy array
yRK4 = np.array(solver.RK4())
yEulerbkd = np.array(solver.EulerBackward())
yTrapezoidal = np.array(solver.Trapezoidal())

# Generate x values (use np.arange for proper step size alignment)
t_valuesEf, yEulerfwd_derivative = numerical_derivative(yEulerfwd, stepsize)
t_valuesRK2, yRK2_derivative = numerical_derivative(yRK2, stepsize)
t_valuesRK4, yRK4_derivative = numerical_derivative(yRK4, stepsize)
t_valuesEb, yEulerbkd_derivative = numerical_derivative(yEulerbkd, stepsize)
t_valuesTrp, yTrapezoidal_derivative = numerical_derivative(yTrapezoidal, stepsize)

# Plot results
plt.plot(t_valuesEf, L * yEulerfwd_derivative, label="Forward Euler", linestyle="dashed", color="blue")
plt.plot(t_valuesEb, L * yEulerbkd_derivative, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(t_valuesRK4, L * yRK4_derivative, label="RK4 Method", linestyle="dashed", color="red")
plt.plot(t_valuesRK2, L * yRK2_derivative, label="RK2 Method", linestyle = "dashed", color = "black")
plt.plot(t_valuesTrp, L * yTrapezoidal_derivative, label="Trapezoidal", linestyle = "dashed", color = "purple")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK4")
plt.legend(loc = "upper right")
plt.grid()
plt.show()