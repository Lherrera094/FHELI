import numpy as np
import matplotlib.pyplot as plt

def plot_circular( x_coord, y_coord, NX, NY ):

    plt.plot( x_coord, y_coord, "go" )
    plt.xlim([0,NX/2])
    plt.ylim([0,NY/2])
    plt.grid(True)
    plt.show()

def plot_3DAntenna( x_coord, y_coord ):
