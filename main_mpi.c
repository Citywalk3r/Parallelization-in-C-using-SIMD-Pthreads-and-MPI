#include "mpi_l.h"

int main(int argc, char ** argv){
	assert(argc == 3);
	
	/* Initialize MPI */
	MPI_Init(&argc, &argv);
	
	/* Get number of processes */
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	/* Get rank of processes */
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// 	printf("We run process %d of %d processes\n", world_rank, world_size);
	
	unsigned int N = (unsigned int)atoi(argv[1]);
	THREADS = (unsigned int)atoi(argv[2]);
	unsigned int iters = 1;
	
	double timeTotalMainStart = gettime();
	double timeOmegaTotalStart;
	float avgF = 0.0f;
	float maxF = 0.0f;
	float minF = FLT_MAX;
	
	/* Making equal parts for each process */
	unsigned int _i = world_rank * N / (float)world_size;
	unsigned int _N = (world_rank + 1) * N / (float)world_size;
// 	printf("Process[%d] _i is %d\n", world_rank, _i);
// 	printf("Process[%d] _N is %d\n", world_rank, _N);
	float results[3];
	MPI_Func(_i, _N, THREADS, &maxF, &minF, &avgF, &timeOmegaTotalStart);
	results[0] = maxF;
	results[1] = minF;
	results[2] = avgF;

	
	/* Send the values to process 0 */
	if (world_rank != 0) 
		MPI_Send(results, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
	
	/* Receive the values in process 0 */
	if (world_rank == 0){
		for (unsigned int i = 1; i < world_size; i++){
			MPI_Recv(results, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD,\
						MPI_STATUS_IGNORE);
// 			printf("Received from %d min: %f max: %f avg: %f\n", i, results[1],\
// 					results[0], results[2]);
			maxF = results[0] > maxF ? results[0] : maxF;
			minF = results[1] < minF ? results[1] : minF;
			avgF += results[2];
		}
	}
	
	
	double timeOmegaTotal = gettime() - timeOmegaTotalStart;
	double timeTotalMainStop = gettime();

	if (world_rank == 0){
		printf("Omega time %fs - Total time %fs - Min %e - Max %e - Avg %e\n",\
				timeOmegaTotal/iters, timeTotalMainStop-timeTotalMainStart,\
				(double)minF, (double)maxF, (double)avgF/N);
// 		MPI_Finalize();
	}
	MPI_Finalize();
}
