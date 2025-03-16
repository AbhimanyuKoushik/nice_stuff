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
    delta_height = Amplitude / stepsize
    return np.where(np.abs(np.mod(t, timePeriod)) <= stepsize, delta_height, 
                    np.where(np.abs(np.mod(t, timePeriod) - dutyratio) <= stepsize / 2, -delta_height, 0))

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
    return squarewave(x, Amplitude, T, alpha) / L - (R * y)/ L

def g(x, y):
    return diff_squarewave(x, Amplitude, T, alpha, stepsize) - (R * y) / L 

def numerical_derivative(y, stepsize):
    dy = [(y[i+1] - y[i]) / stepsize for i in range(len(y) - 1)]  # Forward difference method
    dy = np.append(dy, dy[-1])  # Repeat the last derivative value to match length
    x = np.linspace(t_init, t_max, len(dy))  # Ensure x matches y length
    return np.array(x), np.array(dy)
    
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
'''
plt.plot(t_values, y_eulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
plt.plot(t_values, y_eulerbkd, label="Backward Euler", linestyle="dashed", color="green")
plt.plot(t_values, y_rk4, label="RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, y_rk2, label="RK2 Method", linestyle = "dashed", color = "black")
plt.plot(t_values, y_trap, label="Trapezoidal", linestyle = "dashed", color = "purple")
'''
plt.plot(t_values, y_rk4derr, label="Diff then RK4 Method", linestyle="dashed", color="red")
plt.plot(t_values, L * y_derrk4, label="RK4 Method then Diff", linestyle="dashed", color="black")
#plt.plot(t_values, g(t_values,0))

plt.xlabel("x")
plt.ylabel("y")
plt.title("Solution using Euler's Method and RK4")
plt.legend()
plt.grid()
plt.show()