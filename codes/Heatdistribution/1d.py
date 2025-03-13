import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as anim

rodLength = 10 # Length of Rod
maxTime = 0.5 # Time till which simulation will run
lengthNodes = 150 # The rod is divided into 150 parts, we discretized the system
alpha = 10 # Thermal Conductivity
dx = rodLength / lengthNodes # Length of a small part of rod
dt = 0.5 * (dx**2) / alpha # Time step, should be atleast this much for error to not explode
timeNodes = int(maxTime/dt) # Discretizing the time as well

def f(x): # Initial Temperature Distribution of rod
    return np.where(np.mod(x, 2 * np.pi) <= np.pi, 100, -100) # Returns 100 if x is less than pi, else -100

x = np.linspace(0, rodLength, lengthNodes)
tempDistribution = f(2 * np.pi * x / rodLength).astype(float) # Normalize x to match periodicity
# .astype(float) is used to convert each element to float as the function was returning all elements as integers

history = np.zeros((timeNodes + 1, lengthNodes))
# History is basically a time frame, It stores the temperature at all length nodes at a specific point of time

history[0] = tempDistribution.copy()
# Initial distibution is same as distibution at the staring of the time

for t in range(1, timeNodes + 1):
    # Finite DIfference Method for solving Partial Differential Equations
    tempDistribution[1:lengthNodes-1] += alpha * (dt / dx**2) * (tempDistribution[2:lengthNodes] -
                                        2 * tempDistribution[1:lengthNodes-1] + tempDistribution[0:lengthNodes-2])
    # This is used to solve the PDE du/dt = alpha(d^2u/dx^2) where u = (x,t) which discribes the heat distibution at each point
    # At every time

    # Applying Neumann's Boundary condition du/dx at x = 0 and x = rodLenght is 0, as this is how real world behaviour is
    # For more info, see 3b1b's Differential equation series 
    tempDistribution[0] = tempDistribution[1]
    tempDistribution[-1] = tempDistribution[-2]

    history[t] = tempDistribution.copy()
    # Copying this distibution for time = t*dt

# Right now, we have the entire data of how the temperature distribution is at any time t (after discritizing) in history
# history[t][x] is the temperature of point at a distance of x*dx from X = 0 of the rod at time = t*dt

fig, axis = plt.subplots()
axis.set_xlim(-1, rodLength+1) # Setting the limits of X-axis during the animation
axis.set_ylim(-4, 4) # Setting the limits of Y-axis during animation, Below we will send the rod height to be unit length
axis.set_xlabel("Rod Postion (m)") # Labelling X-axis
axis.set_ylabel("Fixed Unit Height") # Labelling Y-axis
title = axis.set_title("Temperature Distribution at t = 0.000s") # Setting the title, we will update it as time passes

heatmap = axis.imshow(history[0].reshape(-1, 1), # a.reshape(-1, 1) turn a into a column vector
                        # The input should be a column vector 
                        cmap = "jet",
                        aspect = "auto",
                        extent = [0, rodLength, 0, 1], # the heatmap (history basically) will show on X-axis in the
                        # interval (0, rodLength) and Y-axis in the interval (0, 1)
                        vmin = -125, # The colour map starts from vmin to vmax
                        vmax = 125,)

fig.colorbar(heatmap, ax = axis, label = "Temperature (°C)")

def animate(frame):
    # Update the heatmap according the history
    heatmap.set_array(history[frame].reshape(1, -1)) # a.reshape(1, -1) turns a into a row vector which is the required input
    title.set_text(f"Temperature Distribution at t = {frame * dt:.3f}s") # Updating the title, :.3f makes sure that only
    # 3 decimals are printed
    return heatmap, title

ani = anim.FuncAnimation(fig, animate, frames = range(0, timeNodes, 50), interval = dt * 1e3, blit=False)
# A frame is one plot of history[t], One frame is shown for the time t = interval, Right now I am displaying only
# 1 in 10 frames so that animation goes fast otherwise its too slow
# blit = True means the function redraws only the changed parts of figure, faster but doesn't work in case there are elements
# like set_title (title is not considered part of figure), blit = False draws the entire thing including the title but it is slower
plt.show()