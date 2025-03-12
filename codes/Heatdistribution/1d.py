import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as anim

# Simulation Parameters
rodLength = 10
maxtime = 2.5
lengthNodes = 150
alpha = 100
dx = rodLength / lengthNodes
dt = 0.5 * (dx**2) / alpha
timeNodes = int(maxtime / dt)

def f(x):
    return np.where(np.mod(x, 2*np.pi) < np.pi, 100, -100)  # Alternating 1 and 0 every π

# Initial Temperature Distribution
x = np.linspace(0, rodLength, lengthNodes)
tempDistribution = f(2 * np.pi * x / rodLength).astype(float)  # Normalize x to match periodicity

# Store Temperature History
history = np.zeros((timeNodes + 1, lengthNodes))
history[0] = tempDistribution.copy()

# Finite Difference Time Evolution
for t in range(1, timeNodes + 1):
    # Internal Node Updates
    tempDistribution[1:-1] += alpha * (dt / dx**2) * (
        tempDistribution[2:] - 2 * tempDistribution[1:-1] + tempDistribution[:-2]
    )
    # Neumann Boundary Conditions
    tempDistribution[0] = tempDistribution[1]
    tempDistribution[-1] = tempDistribution[-2]

    # Store in History
    history[t] = tempDistribution.copy()

# ** PLOTTING AND ANIMATION **
fig, axis = plt.subplots(figsize=(8, 4))  
axis.set_xlim(-1, rodLength + 1)  # Correct x-limits
axis.set_ylim(-4, 4)  # Correct y-limits (Rod height from 0 to 1)
axis.set_xlabel("Rod Position (m)")
axis.set_ylabel("Fixed Unit Height")
title = axis.set_title(f"Temperature Distribution at t = 0.000 s")

# Use `imshow()` for animation
heatmap = axis.imshow(history[0].reshape(1, -1), 
                      cmap="jet", 
                      aspect="auto", 
                      extent=[0, rodLength, 0, 1],  # Matches rod length exactly
                      vmin=-125, 
                      vmax=125)

fig.colorbar(heatmap, ax=axis, label="Temperature (°C)")

# Animation Function
def animate(frame):
    heatmap.set_array(history[frame].reshape(1, -1))  # Update color data
    title.set_text(f"Temperature Distribution at t = {frame * dt:.3f} s")  # Update title
    return heatmap, title

# Create and Run Animation
ani = anim.FuncAnimation(fig, animate, frames= range(0,timeNodes,10) , interval=dt * 1e3 , blit=False)

plt.show()
