import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap

k = 9e9 # Coulomb's constant

# Load the viridis colormap
viridis = plt.colormaps['viridis']

# Create a truncated version that avoids yellow (e.g., keep 0 to 0.8 of the range)
new_viridis = viridis(np.linspace(0, 0.8, 256))

# Create a new colormap
custom_cmap = LinearSegmentedColormap.from_list("custom_viridis", new_viridis)


# To ignore invalid and division errors and warnings
np.seterr(divide = 'ignore', invalid = 'ignore')

# Grid size, that is number of points in each direction
gridx = 30
gridy = 30

# The limits of the graph
xmin, xmax = -4, 4
ymin, ymax = -4, 4

# Creating Coordinate grid
X = np.linspace(xmin, xmax, gridx) # The points will be evenlyl spaced
Y = np.linspace(ymin, ymax, gridy)
X, Y = np.meshgrid(X, Y) # Creating a Mesh grid

# Initializing the electric field
potentialx = np.zeros((gridx, gridy))
potentialy = np.zeros((gridx, gridy))

# Number of charges and their properties
numberofCharges = 4
chargeValues = [1, 2, -2, -1]
chargeCoordinates = [[0, 0, 0, -1], [1, -1, 0, 0]] # (x, y) positions of charges

# Compute electric field at each point on the grid
for i in range(numberofCharges):
    x_charge = chargeCoordinates[0][i]
    y_charge = chargeCoordinates[1][i]
    q = chargeValues[i]
    for n in range(gridx):
        for m in range(gridy):
            distx = X[n, m] - x_charge
            disty = Y[n, m] - y_charge
            distance = (distx**2 + disty**2) ** 0.5

            potentialy[n, m] += - k * q * distx / (distance ** 2)
            potentialx[n, m] += k * q * disty / (distance ** 2)

# Compute arrow colors based on field magnitude
C = np.log1p(np.hypot(potentialx, potentialy))  # Magnitude of field vectors

# Normalize vectors which represent equipotential lines to unit length to show direction
potential = np.sqrt(potentialx**2 + potentialy**2)
potentialx = potentialx / potential
potentialy = potentialy / potential

# Plot the electric field
plt.figure(figsize=(8, 6))
for i in range(numberofCharges):
    customLabel = 'Charge ' + str(chargeValues[i]) + 'C at (' + str(chargeCoordinates[0][i]) + ',' + str(chargeCoordinates[1][i]) + ')'
    if(chargeValues[i] >= 0):
        plt.plot(chargeCoordinates[0][i], chargeCoordinates[1][i], 'ro', markersize=5, label=customLabel)
    else:
        plt.plot(chargeCoordinates[0][i], chargeCoordinates[1][i], 'bo', markersize=5, label=customLabel)
# Draw the vector field
plt.quiver(X, Y, potentialx, potentialy, C, pivot='mid', cmap=custom_cmap)

# Add colorbar
cbar = plt.colorbar()
cbar.ax.set_ylabel('Log(absolute value of Potential+1)')

plt.xlim(xmin, xmax)
plt.ylim(ymin, ymax)
plt.xticks(np.arange(xmin, xmax + 0.5, 0.5))
plt.yticks(np.arange(ymin, ymax + 0.5, 0.5))
plt.grid(True, linestyle="--", linewidth=0.7)
plt.gca().set_aspect("equal", adjustable="box")  # Ensure equal aspect
plt.tight_layout()  # Remove extra padding
plt.legend(loc="upper right")
plt.show()
