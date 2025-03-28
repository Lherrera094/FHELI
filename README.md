<h3 align="center"> FOCAL HELICON Waves </h3>

Developers: [Luis Carlos Herrera Quesada](https://www.linkedin.com/in/lherreraquesada/)\
&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;[Alf Köhn-Seemann](https://www.igvp.uni-stuttgart.de/team/Koehn-Seemann/)
	    
**Description**: 3D FDTD code for propagation of electromagnetic waves in cold magnetized plasma, especially modified for studies of helicon waves propagation.

* **Features**: FHELI reads a JSON file as an input, using the cJSON library provided by Dave Gamble (see https://github.com/DaveGamble/cJSON), allowing a faster and easiest operation. FHELI have a series of plasma density profiles that can be selected by the user, the code can also read external profiles for more variability. Code is fully parallelized with OpenMP. User may choose between different implemented boundaries as ABC, Mur and Uniaxial Perfectly Matching Layer(UPML). The output consists of a text file for the energy power values (Timetraces), a copy of the input JSON file, and a HDF5 file containing the electric field, plasma density, volume and important parameters for the simulation. Code has been benchmarked for vacuum propagation, power calculation, against EMIT-3D and cold plasma theory.

* **Example Results**: 

<p align="center">
  
![Helical](/tools/fullwave_simulation.png "3D FDTD simulation of Nagoya Type-III antenna.")

<p align="center">
  
![Helical_ant](/tools/Fullwave_phase_Sims.gif "2D FDTD simulation of Nagoya Type-III antenna .")
  
</p>

* **Tools**: Contains a series of Python codes to help users with the simulations visualization. The GUIs for [Antenna Modeling](https://github.com/Lherrera094/Antenna_modeling) and [Data Analysis](https://github.com/Lherrera094/FHELI_DataAnalysis_Pipeline) can be found in the shared links.

Note: The code is still evolving, new features and improvements for helicon antennas studies will be implemented. 
