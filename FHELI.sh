#!/bin/bash
#SBATCH --nodes=1                                       # Use 1 node (OpenMP is shared-memory)
#SBATCH --ntasks-per-node=1                             # Run 1 task (OpenMP threads handle parallelism)
#SBATCH --cpus-per-task=96                              # Max=96 logical cores (48 cores × 2 threads)
#SBATCH --mem=100000M                                   # ~370GB (leave 10GB for system overhead)
#SBATCH --time=72:00:00                                 # Max runtime 72:00:00 (3 days)
#SBATCH --mail-type=BEGIN,END,FAIL                      # Replaces `-m bea` in msub or qsub
#SBATCH --job-name=helicon_sim_Double                   # Job name
#SBATCH --output=omp_job_1.out                       	# Output file (%j = job ID)
#SBATCH --error=omp_job_1.err                        	# Error file
#SBATCH --partition=cpu                                 # bwHPC=cpu_il, cpu, highmem, gpu_h100, gpu_mi300
#SBATCH --mail-user=luis.herrera-quesada@igvp.uni-stuttgart.de

EXECUTABLE=bin/./exe

module load lib/hdf5/1.14-gnu-14.2-serial
module load mpi/openmpi/5.0-gnu-14.2

# Set OpenMP environment variables
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK             # Use all allocated threads. Max=96
export OMP_PLACES=cores                                 # Bind threads to physical cores
export OMP_PROC_BIND=close                              # Keep threads close to their memory

echo "Running ${EXECUTABLE} with ${OMP_NUM_THREADS} threads"

${EXECUTABLE}
