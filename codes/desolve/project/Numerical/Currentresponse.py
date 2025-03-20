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

def CurrentrespFourier(t, n, timePeriod, alpha, Amplitude):
    vt = Amplitude * alpha
    w = 2 * np.pi / timePeriod
    for i in range(1, n+1):
        vt += (Amplitude * np.sin(2 * np.pi * i * alpha)/(np.pi * i)) * np.cos(w * i * t)
        vt += (Amplitude * (1 - np.cos(2 * np.pi * i * alpha))/(np.pi * i)) * np.sin(w * i * t)
    return vt

t_init = 0
y0 = 0
R = 1e3
L = 1e-3
t_max = 0.1
stepsize = 5e-7
alpha = 0.1
T = 0.01
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return SquareWave(x, Amplitude, T, alpha) / L - (R * y)/ L
    
# Create ODE solver instance
solver = linear_de.LinearDE(f, t_init, y0, t_max, stepsize)

# Compute solutions
yEulerfwd = np.array(solver.EulerForward())  # Convert to NumPy array
yRK2 = np.array(solver.RK2())  # Convert to NumPy array
yRK4 = np.array(solver.RK4())
yEulerbkd = np.array(solver.EulerBackward())
yTrapezoidal = np.array(solver.Trapezoidal())

t_values = np.arange(t_init, t_max, stepsize)
#currentresponse = CurrentrespFourier(t_values, 200, T, alpha, Amplitude)

# Plot results
plt.plot(t_values, yEulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
plt.plot(t_values, yEulerbkd, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(t_values, yRK4, label="RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, yRK2, label="RK2 Method", linestyle = "dashed", color = "black")
plt.plot(t_values, yTrapezoidal, label="Trapezoidal", linestyle = "dashed", color = "purple")
#plt.plot(t_values, currentresponse, label="Theoritical", linestyle = "solid", color = "orange")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Current response of an Inductor for square wave Input")
plt.legend(loc = "upper right")
plt.grid()
plt.show()