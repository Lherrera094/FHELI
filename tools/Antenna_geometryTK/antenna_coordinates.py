import numpy as np

def construct_circular( R, ant_x, ant_y, Z ):
    
    theta = 360
    circular = np.zeros( (theta, 3) )

    for i in range(0, theta):
        #computes the radial position of the antenna, 
        #given the radius and the antenna center position
        circular[i,0] = round( (ant_x/2) + (R/2) * np.cos(i*np.pi/180) )
        circular[i,1] = round( (ant_y/2) + (R/2) * np.sin(i*np.pi/180) )
        circular[i,2] = round( Z/2 ) 
    
    return circular

def construct_linear( x_pos, y_pos, Z_0, Z_1 ):

    linear = np.zeros( ( int( (Z_1-Z_0)/2 ) + 1,3) )

    for i in range(0, int((Z_1-Z_0)/2) + 1 ):
        linear[i,0] = round( x_pos/2 )
        linear[i,1] = round( y_pos/2 )
        linear[i,2] = round( (Z_0/2) + i ) 

    return linear
