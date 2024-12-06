import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import plotly.graph_objects as go

def plot_circular( x_coord, y_coord, NX, NY ):

    plt.plot( x_coord, y_coord, "go" )
    plt.xlim([0,NX/2])
    plt.ylim([0,NY/2])
    plt.grid(True)
    plt.show()

def plot_3DAntenna( x_coord, y_coord, z_coord ):
    
    #call the function figure
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    ax.plot(positions_i[:, 0], positions_i[:, 1], positions_i[:, 2],color="darkred"
            ,alpha=0.3,linestyle="--")

    ax.scatter(last_point[0], last_point[1], last_point[2],label=r"+$q$", color='darkred', s=50)

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    ax.xaxis.set_tick_params(labelsize=10,pad=0.5)
    ax.yaxis.set_tick_params(labelsize=10,pad=1)
    ax.zaxis.set_tick_params(labelsize=10,pad=1)

    ax.set_title(f"Charged Particle in Homogeneous "+ r"$\textbf{B}$")
    plt.show()
