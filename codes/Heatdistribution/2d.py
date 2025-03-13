import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as anim

# Simulation Parameters
plateLengthx = 10 # Width of the plate
plateLengthy = 10 # Length of the plate
maxtime = 0.5  # Time till which simulation will run
lengthNodesx = 100 # The plate is divided into 150 strips along Y-axis, we discretized the system
lengthNodesy = 100 # The plate is divided into 150 strips along X-axis, we discretized the system
# Now the plate has been divided into lengthNodesx * lengthNodesy elements
alphax = 10 # Thermal diffusivity along X-axis
alphay = 10 # Thermal diffusivity along Y-axis
dx = plateLengthx / lengthNodesx # Lenght of the strip divided along X-axis
dy = plateLengthy / lengthNodesy # Lenght of the strip divided along X-axis
# The area of a single areal element i s dx*dy
dt = 0.25 * min(dx, dy)**2 / ((alphax**2 + alphay**2) ** (0.5))  # Stability condition
timeNodes = int(maxtime / dt)

def f(x, y):
    return 100 * np.cos(np.pi * x) * np.cos(np.pi * y)

# Grid Setup
x = np.linspace(0, plateLengthx, lengthNodesx)
y = np.linspace(0, plateLengthy, lengthNodesy)
X, Y = np.meshgrid(x, y, indexing="ij")  # Ensure correct shape (i,j)
tempDistribution = f(X,Y).astype(float)

store_interval = 5  # Save every 100th step
history = np.zeros((timeNodes // store_interval + 1, lengthNodesx, lengthNodesy))
history[0] = tempDistribution.copy()

# **Vectorized Finite Difference Update**
for t in range(1, timeNodes + 1):
    new_temp = tempDistribution.copy()

    # Internal Node Updates (Vectorized)
    new_temp[1:-1, 1:-1] += dt * (
        alphay * (tempDistribution[2:, 1:-1] - 2 * tempDistribution[1:-1, 1:-1] + tempDistribution[:-2, 1:-1]) / dy**2 +
        alphax * (tempDistribution[1:-1, 2:] - 2 * tempDistribution[1:-1, 1:-1] + tempDistribution[1:-1, :-2]) / dx**2
    )

    # Neumann Boundary Conditions (Zero gradient)
    new_temp[0, :] = new_temp[1, :]
    new_temp[-1, :] = new_temp[-2, :]
    new_temp[:, 0] = new_temp[:, 1]
    new_temp[:, -1] = new_temp[:, -2]

    tempDistribution = new_temp.copy()
    # Store only every `store_interval` step
    if t % store_interval == 0:
        history[t // store_interval] = tempDistribution

# ** PLOTTING AND ANIMATION **
fig, axis = plt.subplots(figsize=(6, 5))
axis.set_xlim(-1, plateLengthx + 1)
axis.set_ylim(-1, plateLengthy + 1)
axis.set_xlabel("Plate width (m)")
axis.set_ylabel("Plate length (m)")
title = axis.set_title(f"Temperature Distribution at t = 0.000 s")

# Use `imshow()` for animation
heatmap = axis.imshow(history[0], cmap="jet", aspect="auto", 
                      extent=[0, plateLengthx, 0, plateLengthy], 
                      vmin=-125, vmax=125)

fig.colorbar(heatmap, ax=axis, label="Temperature (°C)")

# Animation Function
def animate(frame):
    heatmap.set_array(history[frame])  # Update color data
    title.set_text(f"Temperature Distribution at t = {frame * dt:.3f} s")  # Update title
    return heatmap, title

# Create and Run Animation
ani = anim.FuncAnimation(fig, animate, frames=range(0, timeNodes, 1), interval=dt * 1e3, blit=False)

plt.show()
