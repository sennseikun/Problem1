#!/bin/sh
#SBATCH --partition=TRAINING
#SBATCH --time=00:15:00
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=8
#SBATCH --mem=12000
# Memory per node specification is in MB. It is optional. 
# The default limit is 3000MB per core.
#SBATCH --job-name="julia_test"
#SBATCH --output=test-srun.out
#SBATCH --mail-user=thomborr@stud.ntnu.no
#SBATCH --mail-type=ALL
 
 
module load GCC
module load OpenMPI/2.0.2
mpirun ./julia
 
thomborr -a
