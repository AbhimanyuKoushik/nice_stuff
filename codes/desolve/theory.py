import numpy as np
import matplotlib.pyplot as plt

def theory(t, n, timePeriod, alpha, Amplitude):
    vt = Amplitude * alpha
    w = 2 * np.pi / timePeriod
    for i in range(1, n+1):
        vt += (Amplitude * np.sin(2 * np.pi * i * alpha)/(np.pi * i)) * np.cos(w * i * t)
        vt += (Amplitude * (1 - np.cos(2 * np.pi * i * alpha))/(np.pi * i)) * np.sin(w * i * t)
    return vt

# Define parameters
t_init = 0
t_max = 3
stepsize = 1e-5
T = 1.0        # Period of the square wave
Amplitude = 10
dutyratio = 0.1  # 40% duty cycle

# Generate time values
times = np.arange(t_init, t_max, stepsize)

signal = theory(times, 1000, T, dutyratio, Amplitude)

# Plot the results
plt.figure(figsize=(8, 5))
plt.plot(times, signal, label="Signal", color="blue")

plt.xlabel("Time (s)")
plt.ylabel("$V_L$")
plt.legend()
plt.grid()
plt.show()
