#!/bin/sh

#
# Compiling script
#

LD_SRC="src/"
LD_INC="include/"
BIN="bin/"

lib0=""${LD_SRC}"generic.c"
lib1=""${LD_SRC}"sse.c"
lib2=""${LD_SRC}"pthreads.c"
lib3=""${LD_SRC}"mpi_l.c"

# Native C variants. No use of SSE, pthreads or MPI
gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}"\
	main_baseline.c -o "${BIN}"baseline
gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}"\
	main_unrolled.c -o "${BIN}"unrolled
gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}"\
	main_unrolled_n_jammed.c -o "${BIN}"unrolled_n_jammed

# SSE variants
gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" -march=native\
	-msse -msse2 -msse3 -msse4.1 -msse4.2 main_sse.c -o "${BIN}"sse
gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" -march=native\
	-msse -msse2 -msse3 -msse4.1 -msse4.2 main_sse_scrambled.c -o "${BIN}"sse_scrambled
	gcc -D_GNU_SOURCE -Wall -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" -march=native\
	-msse -msse2 -msse3 -msse4.1 -msse4.2 main_sse_MLT.c -o "${BIN}"sse_MLT

# Pthreads + SSE variant
gcc -D_GNU_SOURCE -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" "${lib2}"\
	-march=native -msse -msse2 -msse3 -msse4.1 -msse4.2 -lpthread\
	main_pthreads.c -o "${BIN}"pthreads

# MPI + Pthreads + SSE variant
mpicc -D_GNU_SOURCE -ggdb3 -I"${LD_INC}" "${lib0}" "${lib1}" "${lib2}"\
	"${lib3}" -march=native -msse -msse2 -msse3 -msse4.1 -msse4.2 -lpthread\
	main_mpi.c -o "${BIN}"mpi
