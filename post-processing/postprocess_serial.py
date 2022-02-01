import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation


""" Post-processing script to load in the serial solution to the matrix and plot the animated solution using python.
This script will be the base for loading the parallel outputs later on. """


# The output file name
output_dir = "./output_serial"

# Get the number of files in the output directory
num_outputfiles = 125

# Setup  figure
fig = plt.figure(figsize=(10,10))
ax = fig.add_subplot(111)

def animate_output(i):
    # Format the file string
    output_file =  "./output_serial/output_{}.dat".format(i)
    
    # Load data from file
    data = np.loadtxt(output_file, dtype=float)
    ax.clear()
    # Plot the data
    ax.imshow(data, cmap="winter")

# Initialize the animation object
ani = matplotlib.animation.FuncAnimation(fig, animate_output, frames=num_outputfiles, repeat=False)
# plt.show()

# # Initialize the writer function
writergif = animation.PillowWriter(fps=30) 
# # # Save the animation using the writer
ani.save("./animate_serial.gif", writer=writergif)