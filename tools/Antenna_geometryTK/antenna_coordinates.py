import numpy as np
from save_coordinates import *

def construct_circular( R, ant_x, ant_y, Z, folder_name, sec ):
    
    coordinates = set()
    num_points = 2000
    theta_step = 2*np.pi/num_points

    for i in range( num_points ):
        theta = i*theta_step

        #computes the radial position of the antenna, 
        #given the radius and the antenna center position
        x = round( (ant_x/2) + (R/2) * np.cos(theta) )
        y = round( (ant_y/2) + (R/2) * np.sin(theta) )
        z = round( Z/2 )

        coordinates.add( (x,y,z) ) 

    coordinates = np.array( list(coordinates) )
    save_data( coordinates, f"{folder_name}/Section_{sec}" )
    
    return coordinates

def construct_linear( x_pos, y_pos, Z_0, Z_1, folder_name, sec ):

    #linear = np.zeros( ( int( (Z_1-Z_0)/2 ) + 1,3) )
    coordinates = set()

    for i in range(0, int((Z_1-Z_0)/2) + 1 ):
        x = round( x_pos/2 )
        y = round( y_pos/2 )
        z = round( (Z_0/2) + i ) 
        coordinates.add( (x,y,z) )

    coordinates = np.array( list(coordinates) )
    save_data( coordinates, f"{folder_name}/Section_{sec}" )

    return coordinates

def right_helical( ant_x, ant_y, Z_0, Z_1, num_turns, R, chirality, folder_name, sec ):

    pitch = (Z_1 - Z_0)/(2*num_turns)
    coordinates = set()
    num_points = 2000
    theta_step = 2*np.pi/num_points

    for i in range( round(num_turns*num_points) ):

        theta = i*theta_step
        x = round( (ant_x/2) + (R/2) * np.cos(chirality * theta) )
        y = round( (ant_y/2) + (R/2) * np.sin(chirality * theta) )
        z = round( (Z_0/2) + ( pitch * (i)/( num_points ) ) )

        coordinates.add( (x,y,z) ) 

    coordinates = np.array( list(coordinates) )
    save_data( coordinates, f"{folder_name}/Section_{sec}" )

    return coordinates

def left_helical( ant_x, ant_y, Z_0, Z_1, num_turns, R, chirality, folder_name, sec ):

    pitch = (Z_1 - Z_0)/(2*num_turns)
    coordinates = set()
    num_points = 2000
    theta_step = 2*np.pi/num_points

    for i in range( round(num_turns*num_points) ):

        theta = i*theta_step
        x = round( (ant_x/2) + (R/2) * np.cos( (chirality * theta) + np.pi) )
        y = round( (ant_y/2) + (R/2) * np.sin( (chirality * theta) + np.pi) )
        z = round( (Z_0/2) + ( pitch * (i)/( num_points ) ) )

        coordinates.add( (x,y,z) ) 

    coordinates = np.array( list(coordinates) )
    save_data( coordinates, f"{folder_name}/Section_{sec}" )

    return coordinates

