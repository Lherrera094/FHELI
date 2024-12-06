import numpy as np

def control_antenna(select, ):


def construct_circular(R, ant_x, ant_y):
    
    x_pos = []
    y_pos = []

    for theta in range(0,360):
        #computes the radial position of the antenna, 
        #given the radius and the antenna center position
        x_pos.append( round( (ant_x/2) + (R/2) * np.cos(theta), 0) )
        y_pos.append( round( (ant_y/2) + (R/2) * np.sin(theta), 0) )
    
    return x_pos, y_pos

def construct_linear(Z_0, Z_1):

    Z = []

    for k in range(Z_0, Z_1):
        Z.append(k)

    return Z
