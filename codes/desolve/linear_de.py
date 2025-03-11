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

def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    delta_height = Amplitude/stepsize
    return np.where(np.fabs(t%T) <= stepsize, delta_height, np.where(np.fabs(t%T - dutyratio) <= stepsize, -delta_height, 0))

t_init = 0
y0 = 0
R = 0
L = 1e-3
t_max = 1
stepsize = 0.0001
alpha = 0.3
T = 1.0
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return diff_squarewave(x, Amplitude, T, alpha, stepsize) - (R * y)/L # dy/dx = y
def g(x, y):
    return squarewave(x, Amplitude, T, alpha) - (R * y)/L

def diff(f,x_init, x_max, stepsize):
    toreturnx = np.arange(x_init,x_max,stepsize)
    size = int((x_max - x_init)/stepsize)+1
    x = [x_init for i in range(size)]
    for i in range(1,size):
        x[i] = x_init + i*stepsize
        toreturx[i] = (f(x[i]) - f(x[i-1]))/stepsize
    return toreturnx
    
# Create ODE solver instance
solver = linear_de.LinearDE(g, t_init, y0, t_max, stepsize)

# Compute solutions
#y_eulerfwd = np.array(solver.EulerForward())  # Convert to NumPy array
#y_rk2 = np.array(solver.RK2())  # Convert to NumPy array
y_rk4 = np.array(solver.RK4())
#y_eulerbkd = np.array(solver.EulerBackward())
#y_trap = np.array(solver.Trapezoidal())

rk4_to_plot = diff(y_rk4, t_init, t_max, stepsize) 
# Generate x values (use np.arange for proper step size alignment)
x_values = np.arange(t_init, t_max, stepsize)

# Plot results
#plt.plot(x_values, y_eulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
#plt.plot(x_values, y_eulerbkd, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(x_values, rk4_to_plot, label="RK4 Method", linestyle="dashed", color="red")
#plt.plot(x_values, y_rk2, label="RK2 Method", linestyle = "dashed", color = "black")
#plt.plot(x_values, y_trap, label="Trapezoidal", linestyle = "dashed", color = "purple")

plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK2")
plt.legend()
plt.grid()
plt.show()