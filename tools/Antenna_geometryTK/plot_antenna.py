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

#C0 and C1 are the circular antennas at the end points of the full antenna
#helic_0 and helic_1 gives the linear/helical antenna for the different configurations 
def plot_3DAntenna( coordinate_C0, coordinate_C1, helic_0, helic_1, NX, NY, NZ ):
   
    #call the function figure
    fig = go.Figure()
    
    #add circular antenna at Z_0
    fig.add_trace( go.Scatter3d
                  (
                      x = coordinate_C0[:,0],
                      y = coordinate_C0[:,1],
                      z = coordinate_C0[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color="navy",
                          opacity=0.8
                      )
                  ))
    
    #add circular antenna at Z_0
    fig.add_trace( go.Scatter3d
                  (
                      x = coordinate_C1[:,0],
                      y = coordinate_C1[:,1],
                      z = coordinate_C1[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color="navy",
                          opacity=0.8
                      )
                  ))
    
    #add linear/helical antenna at x=ant_x + R
    fig.add_trace( go.Scatter3d
                  (
                      x = helic_0[:,0],
                      y = helic_0[:,1],
                      z = helic_0[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color = "navy",
                          opacity=0.8
                      )
                  ))
    
    #add linear/helical antenna at x=ant_x - R
    fig.add_trace( go.Scatter3d
                  (
                      x = helic_1[:,0],
                      y = helic_1[:,1],
                      z = helic_1[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color = "navy",
                          opacity=0.8
                      )
                  ))
    
    fig.update_layout(
        title = "Antenna Geometry",
        scene = dict(
            xaxis = dict(title = "X", range = [0,NX/2]),
            yaxis = dict(title = "Y", range = [0,NY/2]),
            zaxis = dict(title = "Z", range = [0,NZ/2])
        )
    )

    fig.show()
