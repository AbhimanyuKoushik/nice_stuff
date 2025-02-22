import ctypes
import numpy as np
import matplotlib.pyplot as plt
from random import random

# Load shared library using ctypes
lib = ctypes.CDLL("./nevillepolyinterpol.so")

# Define function prototypes
lib.polyinterpol.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.POINTER(ctypes.c_double))]
lib.polyinterpol.restype  = ctypes.CFUNCTYPE(ctypes.c_double, ctypes.c_double)

# Get user input for points
numpoints = int(input("Enter number of points: "))
points = []

print("Enter the points in the format 'x y': ")

for i in range(numpoints):
    # Consider input a b c d, it splits it into (a,b,c,d) and store in the tuple in similar format of the datatype mentioned
    # Here it splits the input in a,b and assigns it the float datatype and stores it as (a,b) after which x is assigned a, y to b
    #x,y = map(float, input().split())
    x,y = map(float, input().split())
    
    # Gets added to the points collection
    points.append((x,y))

# Converts points to C++-compatible format
PointArray = ctypes.POINTER(ctypes.c_double) * numpoints
points_c = PointArray(*[(ctypes.c_double * 2)(p[0], p[1]) for p in points])

# Call the C++ function and get the function pointer
interpolate = lib.polyinterpol(numpoints, points_c)

# Generating x,y values for plotting 
x_vals = np.linspace(min(p[0] for p in points), max(p[0] for p in points), 500)
y_vals = [interpolate(x) for x in x_vals]

# Plotting Original points
plt.scatter(*zip(*points), color = "orange", label = "Given points")
plt.plot(x_vals, y_vals, label = "Interpolated Curve", color = "blue")

plt.xlabel("X")
plt.ylabel("Y")
plt.legend()
plt.title("Neville's Interpolation Algorithm")
plt.show()
