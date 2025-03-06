import tkinter as tk
from tkinter import ttk
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import linear_de

def squarewave(t, Amplitude, timePeriod, dutyratio):
    fractional = (1 - (t / timePeriod)) - np.floor(1 - (t / timePeriod))
    return np.where(fractional > 1 - dutyratio, Amplitude, 0)

# Function to update the plot with current parameters
def update_plot():
    R = float(R_entry.get())
    alpha = float(alpha_entry.get())
    T = float(T_entry.get())
    x_min = float(xmin_entry.get())
    x_max = float(xmax_entry.get())
    
    x0, y0, stepsize, Amplitude = x_min, 0, 0.01, 10
    num_steps = int((x_max - x0) / stepsize) + 1  # Ensure consistent length
    
    # Define the ODE: y' = squarewave(x) - R*y
    def f(x, y):
        return squarewave(x, Amplitude, T, alpha) - (R * y)
    
    solver = linear_de.LinearDE(f, x0, y0, x_max, stepsize)
    y_eulerfwd = np.array(solver.EulerForward())[:num_steps]
    y_rk2      = np.array(solver.RK2())[:num_steps]
    y_rk4      = np.array(solver.RK4())[:num_steps]
    y_eulerbkd = np.array(solver.EulerBackward())[:num_steps]
    y_trap     = np.array(solver.Trapezoidal())[:num_steps]
    
    x_values = np.arange(x0, x_max + stepsize, stepsize)[:num_steps]  # Ensures correct length
    ax.clear()
    ax.plot(x_values, y_eulerfwd, label="Forward Euler", linestyle="dashed", color="blue")
    ax.plot(x_values, y_eulerbkd, label="Backward Euler", linestyle="dashed", color="green")
    ax.plot(x_values, y_rk2, label="RK2", linestyle="dashed", color="red")
    ax.plot(x_values, y_rk4, label="RK4", linestyle="dashed", color="black")
    ax.plot(x_values, y_trap, label="Trapezoidal", linestyle="dashed", color="purple")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_title("Numerical Solution of ODE")
    ax.legend(loc="upper right")
    ax.grid(True)
    canvas.draw()

# Create the main application window
root = tk.Tk()
root.title("ODE Solver GUI")
root.geometry("900x600")
root.configure(bg="#ffffff")

# Use a modern ttk theme for a better look
style = ttk.Style(root)
style.theme_use("clam")
style.configure("TFrame", background="#ffffff")
style.configure("TLabel", background="#ffffff", font=("Arial", 12))
style.configure("TButton", font=("Arial", 12))

# Create a frame for controls with padding
control_frame = ttk.Frame(root, padding=20)
control_frame.pack(side=tk.LEFT, fill=tk.Y)

# Variables for entry values
ttk.Label(control_frame, text="Damping Coefficient (R)").pack(pady=5)
R_entry = ttk.Entry(control_frame)
R_entry.pack(pady=5)
R_entry.insert(0, "0.1")

ttk.Label(control_frame, text="Duty Ratio (Alpha)").pack(pady=5)
alpha_entry = ttk.Entry(control_frame)
alpha_entry.pack(pady=5)
alpha_entry.insert(0, "0.5")

ttk.Label(control_frame, text="Time Period (T)").pack(pady=5)
T_entry = ttk.Entry(control_frame)
T_entry.pack(pady=5)
T_entry.insert(0, "1.0")

ttk.Label(control_frame, text="X-Axis Min").pack(pady=5)
xmin_entry = ttk.Entry(control_frame)
xmin_entry.pack(pady=5)
xmin_entry.insert(0, "0")

ttk.Label(control_frame, text="X-Axis Max").pack(pady=5)
xmax_entry = ttk.Entry(control_frame)
xmax_entry.pack(pady=5)
xmax_entry.insert(0, "100")

# Button to trigger the ODE solve and plot update
solve_button = ttk.Button(control_frame, text="Solve", command=update_plot)
solve_button.pack(pady=20)

# Create a frame for the matplotlib plot
plot_frame = ttk.Frame(root, padding=10)
plot_frame.pack(side=tk.RIGHT, expand=True, fill=tk.BOTH)

# Set up the matplotlib figure and canvas
fig, ax = plt.subplots(figsize=(8, 5))
canvas = FigureCanvasTkAgg(fig, master=plot_frame)
canvas.get_tk_widget().pack(expand=True, fill=tk.BOTH)

# Draw initial plot
update_plot()

root.mainloop()
