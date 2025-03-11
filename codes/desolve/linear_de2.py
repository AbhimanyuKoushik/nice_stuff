import linear_de
import numpy as np
import matplotlib.pyplot as plt

def squarewave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

def diff_squarewave(t, Amplitude, timePeriod, dutyratio, stepsize):
    delta_height = Amplitude / stepsize
    return np.where((np.fabs(t % timePeriod) <= stepsize) & (t > stepsize), delta_height, 
                    np.where(np.fabs(t % timePeriod - dutyratio) <= stepsize, -delta_height, 0))

t_init = 0
y0 = 0
R = 10
L = 1e-6
t_max = 1
stepsize = 1e-3
alpha = 0.3
T = 2.0
Amplitude = 10

# Define Python function f(x, y)
def f(x, y):
    return diff_squarewave(x, Amplitude, T, alpha, stepsize) - (R * y) / L 

def g(x, y):
    return squarewave(x, Amplitude, T, alpha) - (R * y) / L

# Create ODE solver instances
solver1 = linear_de.LinearDE(g, t_init, y0, t_max, stepsize)
solver2 = linear_de.LinearDE(f, t_init, y0, t_max, stepsize)

# Compute solutions
y_rk4 = np.array(solver1.RK4())     # Solving for square wave response
y_rk4sure = np.array(solver2.RK4()) # Solving for differential square wave

# Compute numerical derivative manually
def numerical_derivative(y, stepsize):
    dy = [(y[i+1] - y[i]) / stepsize for i in range(len(y) - 1)]  # Forward difference method
    x = np.linspace(t_init, t_max - stepsize, len(dy)+1)  # Adjust x-values to match derivative array
    return np.array(x), np.array(dy)

# Compute derivatives
x_values, rk4_derivative = numerical_derivative(y_rk4, stepsize)
# Plot results
plt.plot(x_values, y_rk4)
#plt.plot(x_values, L * rk4_derivative, label="RK4 Method (Derivative)", linestyle="dashed", color="red")
#plt.plot(np.linspace(t_init, t_max, len(y_rk4sure)), y_rk4sure, label="RK4 Method (Non-Derivative)", linestyle="dashed", color="black")

plt.xlabel("Time (t)")
plt.ylabel("dy/dt")
plt.title("Numerical Derivative using RK4 Method")
plt.legend()
#plt.ylim((-10,10))
plt.grid()
plt.show()
