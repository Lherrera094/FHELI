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

def plot_3DAntenna( coordinate_C0, coordinate_C1, linear_0, linear_1 ):
   
    #call the function figure
    fig = go.Figure()
    
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
    
    fig.add_trace( go.Scatter3d
                  (
                      x = coordinate_C1[:,0],
                      y = coordinate_C1[:,1],
                      z = coordinate_C1[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color="red",
                          opacity=0.8
                      )
                  ))
    
    fig.add_trace( go.Scatter3d
                  (
                      x = linear_0[:,0],
                      y = linear_0[:,1],
                      z = linear_0[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color=linear_0[:,2],
                          colorscale = "viridis",
                          opacity=0.8
                      )
                  ))
    
    fig.add_trace( go.Scatter3d
                  (
                      x = linear_1[:,0],
                      y = linear_1[:,1],
                      z = linear_1[:,2],
                      mode = 'markers',
                      marker = dict(
                          size=2,
                          color=linear_0[:,2],
                          colorscale = "viridis_r",
                          opacity=0.8
                      )
                  ))
    
    fig.update_layout(
        title = "Antenna Geometry",
        scene = dict(
            xaxis_title = "X",
            yaxis_title = "Y",
            zaxis_title = "Z"
        )
    )

    fig.show()
