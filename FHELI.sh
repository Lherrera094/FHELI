        #!/bin/sh
	#MSUB -l nodes=1:ppn=8
        #MSUB -l pmem=100mb
        #MSUB -q singlenode
        #MSUB -N exe
        
        EXECUTABLE=bin/./exe

        export OMP_NUM_THREADS=${MOAB_PROCCOUNT}
        
        echo "Executable ${EXECUTABLE} running 
        with ${OMP_NUM_THREADS} threads"
        
        ${EXECUTABLE}
