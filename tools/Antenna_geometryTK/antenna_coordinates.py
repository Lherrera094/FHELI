import numpy as np

def construct_circular(R, ant_x, ant_y):
    
    x_pos = []
    y_pos = []

    for theta in range(0,360):
        #computes the radial position of the antenna, 
        #given the radius and the center antenna position
        x_pos.append( round( (ant_x/2) + (R/2) * np.cos(theta), 0) )
        y_pos.append( round( (ant_y/2) + (R/2) * np.sin(theta), 0) )
    
    return x_pos, y_pos

def construct_linear():