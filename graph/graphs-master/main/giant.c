#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>
#include <igraph.h>
#include <time.h>
#include "random.h"
#include "cluster.h"
//#include "extras.h"

igraph_integer_t nodes;			/* number of nodes */


int main(int argc, char **argv){

/************* CHECK NUMBER OF ARGUMENTS ******************************/

	if(argc<4){
		printf("Set following parameters:\n \
	1) number of vertices;\n \
	2) number of edges;\n \
	3) number of samples.\n");
		exit(EXIT_FAILURE);
	}
	
	
/************* DECLARATION OF VARIABLES *******************************/
	
	FILE *out;			/* output file pointer */
	char *outstring;	/* output file name */
	
	int i, j, rep;		/* counters in cycles */
	int REPS;			/* number of iteration (sample size) */
	int k;			/* fixed degree*/
	
	igraph_t graph;	/* pointer to the main graph object */
	
	igraph_vector_t sizes;		/* vector of component sizes */
	
	cluster *gsize; 	/* jackknife clusters for largest comps sizes */
	
/************* INITIALIZATION/ASSIGNMENT OF MAIN VARIABLES ************/
	
	/* main arguments */
	nodes	= atoi(argv[1]);
	k		= atoi(argv[2]);
	REPS	= atoi(argv[3]);
	
	
	/* output file name and pointer */
	outstring = malloc(100*sizeof(char));
	sprintf(outstring, "output/C_sym_rand_dilution_N%d_Cm%d", nodes, k);
	out = fopen(outstring, "w");
	
/*******************************/

	/* initialization of the RANLUX random number generator */
	srand(time(NULL));
	rlxd_init(1,rand());

		for(rep=0; rep<REPS; rep++){ // repetitions for statistics
			
			/* initialization igraph vectors */
			igraph_vector_init(&sizes, nodes);

			igraph_k_regular_game(&graph, nodes, k, IGRAPH_UNDIRECTED, IGRAPH_NO_LOOPS);
			
			/* size of largest component */
			igraph_clusters(&graph, NULL, &sizes, NULL, IGRAPH_WEAK);
		
			igraph_write_graph_edgelist(&graph, out);
			
			/* destroy graph and free space */
			igraph_destroy(&graph);
			igraph_vector_destroy(&sizes);
		}
	fclose(out);
	exit(EXIT_SUCCESS);
}

