import os
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.animation as animation

""" Post-processing script to load in the MPI output solution-files to the matrix and plot the animated solution using python.
This script will be the base for loading the parallel outputs later on. """

# Can ask the user to provide some vital information to plot

output_config = pd.read_csv("./output/mpi_config.txt", delimiter="=", names=["Parameter", "Value"])
num_outputs = int(output_config["Value"][0])
topology_rows = int(output_config["Value"][1])
topology_cols = int(output_config["Value"][2])
dt = float(output_config["Value"][3])


# function which combines the data of the processes for particul iteration. Can be used seperately to obtain the data from process files in seperate scipt
def get_combined_data(iteration):
    data = []
    for i in np.arange(topology_rows):
        data_row = []
        for j in np.arange(topology_cols):
            process_data = np.loadtxt("./output/output_it{}_p{}.dat".format(iteration, i*topology_cols + j), dtype=float)
            data_row.append(process_data)
        
        combined_row = np.hstack(data_row)
        data.append(combined_row)

    return np.vstack(data)


fig = plt.figure(figsize=(10,10))
ax = fig.add_subplot(111)

def animate_output(i):
    data = get_combined_data(i)
    ax.clear()
    # Plot the data
    ax.title.set_text('time: {0:.2f}'.format(i*dt))
    ax.imshow(data)
    
# Create animation and write to current directory
ani = matplotlib.animation.FuncAnimation(fig, animate_output, frames=num_outputs, interval=500)
writergif = animation.PillowWriter(fps=25) 
ani.save("./animate.gif", writer=writergif)
