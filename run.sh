#!/bin/sh

#
# Run script
#

BIN="bin/"

mpi=false
pt=false
sses=false
sse=false
uj=false
u=false
b=false

echo "Which program do you want to run?"
echo "1) Baseline"
echo "2) Unrolled"
echo "3) Unrolled + Jammed"
echo "4) SSE"
echo "5) SSE Scrambled"
echo "6) SSE with MLT"
echo "7) Pthreads"
echo "8) MPI"
echo "9) All of the above"

read -p "Selection: " sel
echo

if [ "${sel}" = "9" ]; then
	mpi=true
	pt=true
	b=true
elif [ "${sel}" = "8" ]; then
	mpi=true
	pt=true
	sse=true
	b=true
elif [ "${sel}" = "7" ]; then
	pt=true
	sse=true
	b=true
elif [ "${sel}" = "6" ]; then
	sse=true
	b=true
elif [ "${sel}" = "5" ]; then
	sse=true
	b=true
elif [ "${sel}" = "4" ]; then
	sse=true
	b=true
else
	b=true
fi

if [ "${b}" = true ]; then
	read -p "Enter N: " N
fi
if [ "${mpi}" = true ]; then
	read -p "Enter number of processes " P
fi
if [ "${pt}" = true ]; then
	read -p "Enter number of threads: " T
fi

if [ "${sel}" = "9" ]; then
	echo "Baseline:"
	./"${BIN}"baseline "${N}"
	echo "Unrolled:"
	./"${BIN}"unrolled "${N}"
	echo "Unrolled and jammed:"
	./"${BIN}"unrolled_n_jammed "${N}"
	echo "SSE:"
	./"${BIN}"sse "${N}"
	echo "SSE Scrambled:"
	./"${BIN}"sse_scrambled "${N}"
	echo "SSE MLT:"
	./"${BIN}"sse_MLT "${N}"
	echo "Pthreads:"
	./"${BIN}"pthreads "${N}" "${T}"
	echo "MPI:"
	mpiexec -n "${P}" ./"${BIN}"mpi "${N}" "${T}"
elif [ "${sel}" = "8" ]; then
	mpiexec -n "${P}" ./"${BIN}"mpi "${N}" "${T}"
elif [ "${sel}" = "7" ]; then
	./"${BIN}"pthreads "${N}" "${T}"
elif [ "${sel}" = "6" ]; then
	./"${BIN}"sse_MLT "${N}"
elif [ "${sel}" = "5" ]; then
	./"${BIN}"sse_scrambled "${N}"
elif [ "${sel}" = "4" ]; then
	./"${BIN}"sse "${N}"
elif [ "${sel}" = "3" ]; then
	./"${BIN}"unrolled_n_jammed "${N}"
elif [ "${sel}" = "2" ]; then
	./"${BIN}"unrolled "${N}"
else
	./"${BIN}"baseline "${N}"
fi
