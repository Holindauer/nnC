#include "functions.h"
#include "structs.h"
#include "libraries.h"


//--------------------------------------------------------------------------------- Memory Allocation
void check_memory_allocation(double* arr)
{
	if (!arr) {
		fprintf(stderr, "Memory allocation failed!\n");
		exit(EXIT_FAILURE);  // Exit the program.
	}
}


//---------------------------------------------------------------------------------Initialize Memory

void arr_init_zero(double* arr, int num_elements)
{
	for (int i = 0; i < num_elements; i++)
	{
		arr[i] = 0;
	}
}

/* This function initializes each hidden state array within the network */
void init_hidden(weights* net)
{

	net->hidden = (double**)malloc(net->batch_size * sizeof(double*));
	check_memory_allocation(net->hidden);

	// initialize each batch's hidden array 
	for (int b = 0; b < net->batch_size; b++)
	{
		net->hidden[b] = (double*)malloc(LAYER_1_NEURONS * sizeof(double));
		check_memory_allocation(net->hidden[b]);
		arr_init_zero(net->hidden[b], LAYER_1_NEURONS);   // look into whether -> in this case is passing not a pointer into the func
	}

}

void init_batches(batch_outputs* batch, int batch_size)
{
	// initialize batches
	for (int i = 0; i < batch_size; i++)
	{
		batch[i].output_vector = malloc(LAYER_2_NEURONS * sizeof(double));
		check_memory_allocation(batch[i].output_vector);
		arr_init_zero(batch[i].output_vector, LAYER_2_NEURONS);
	}

}

//---------------------------------------------------------------------------------Free Memory
void free_dataset(example* dataset, int num_examples)
{
	for (int i = 0; i < num_examples; i++)
	{
		free(dataset[i].image);
	}

}

void free_network(weights* net)
{

	free(net->W_1);
	free(net->W_2);
	free(net->b_1);
	free(net->b_2);

	free(net->W_1_grad);
	free(net->W_2_grad);
	free(net->b_1_grad);
	free(net->b_2_grad);

	for (int h = 0; h < net->batch_size; h++)
	{
		free(net->hidden[h]);  // free each nested hidden array
	}
	free(net->hidden);         // free outer array

}
