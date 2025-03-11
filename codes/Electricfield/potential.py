import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from mpl_toolkits.mplot3d import Axes3D  # Import for 3D plotting

k = 9e9 # Coulomb's constant


# Load the viridis colormap
viridis = plt.colormaps['viridis']
new_viridis = viridis(np.linspace(0, 0.8, 256))
custom_cmap = LinearSegmentedColormap.from_list("custom_viridis", new_viridis)

# Ignore invalid and division errors
np.seterr(divide='ignore', invalid='ignore')

# Grid settings
gridx, gridy = 45, 45  # Increase resolution for smoother plot
xmin, xmax = -2, 2
ymin, ymax = -2, 2

# Create coordinate grid
X = np.linspace(xmin, xmax, gridx)
Y = np.linspace(ymin, ymax, gridy)
X, Y = np.meshgrid(X, Y)  # 2D meshgrid for plotting
Z = np.zeros_like(X)  # Initialize potential field

# Define charges and positions
chargeValues = [1, 1, -1, -1]
chargeCoordinates = [[0, 0, 1, -1], [1, -1, 0, 0]]  # (x, y) positions of charges

# Compute electric potential at each (x, y) point
for i in range(len(chargeValues)):
    x_charge, y_charge = chargeCoordinates[0][i], chargeCoordinates[1][i]
    q = chargeValues[i]
    distance = np.sqrt((X - x_charge) ** 2 + (Y - y_charge) ** 2)
    Z += k * q / distance  # Superposition principle

# Plot 3D Surface
fig = plt.figure(figsize=(10, 7))
ax = fig.add_subplot(111, projection='3d')

# Surface plot
surf = ax.plot_surface(X, Y, Z, cmap=custom_cmap, edgecolor='none')

# Labels
ax.set_xlabel('X-axis')
ax.set_ylabel('Y-axis')
ax.set_zlabel('Potential V(x, y)')
ax.set_title('Electric Potential due to Point Charges')

# Color bar
fig.colorbar(surf, ax=ax, shrink=0.6, aspect=15, label="Potential")

# Set view angle
ax.view_init(elev=30, azim=45)  # Adjust for better perspective

plt.show()
