<h3 align="center"> FOCAL HELICON Waves </h3>

Developers: [Luis Carlos Herrera Quesada](https://www.linkedin.com/in/lherreraquesada/)\
&emsp;&emsp;&emsp;&emsp;&emsp;&nbsp;&nbsp;[Alf Köhn-Seemann](https://www.igvp.uni-stuttgart.de/team/Koehn-Seemann/)
	    
**Description**: 3D FDTD code for propagation of electromagnetic waves in cold magnetized plasma, especially modified for studies of helicon waves propagation.

* **Features**: FOCAL receives a JSON file as an input, using the cJSON library provided by Dave Gamble (see https://github.com/DaveGamble/cJSON), allowing a faster and easiest operation. Focal have a series of plasma density profiles that can be selected by the user, also the code can read external profiles developed by the user. Code is fully parallelized with OpenMP. User may choose between different implemented boundaries as ABC, Mur and Uniaxial Perfectly Matching Layer(UPML). The output consists of a text file for the energy power values (Timetraces), a copy of the input JSON file, and a HDF5 file containing the electric field, plasma density, volume and important parameters for the simulation. Code has been benchmarked for vacuum propagation, power calculation, against EMIT-3D and cold plasma theory (to be updated).

* **Example Results**: 

<p align="center">
  
![Helical](/tools/fullwave_simulation.png "3D FDTD simulation of half-helical antenna.")

<p align="center">
  
![Helical_ant](/tools/Nagoya_J_20.gif "2D FDTD simulation of half-helical antenna .")
  
</p>

* **Tools**: Contains a series of Python codes to help user in the input profiles, and results visualization. 

* **Future additions**: Helical antenna for modes m=0 and m=1 plasma injection.

Note that this code is still evolving. 
