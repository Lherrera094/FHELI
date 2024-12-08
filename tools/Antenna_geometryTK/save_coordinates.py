import pandas as pd
import numpy as np
import os

def save_data( coordinates, name ):

    df = pd.DataFrame( coordinates )
    df.to_csv(f"{name}.txt", index=False, header=False)

def create_folder(folder_name):
    
    try:
        os.makedirs(folder_name, exist_ok=True)  # Create folder if it doesn't exist
        print(f"Folder '{folder_name}' created successfully!")
    except Exception as e:
        print(f"An error occurred: {e}")
