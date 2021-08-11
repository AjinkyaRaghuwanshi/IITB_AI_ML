#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <inttypes.h>
#include "../../../C/primitives/include/tensor.h" 
#include "../include/utils/tb_utils.c"
#include "../include/conv.h"
#include "../src/conv.c"
#define NPAGES 30

MemPool 	pool;

Tensor a;
Tensor b;
Tensor r;

int _err_ = 0;

int main(){
    initMemPool(&pool,1,NPAGES);
	//define tensor
	const TensorDataType dataType = float32;
	const int8_t row_major_form = 1;
	const uint32_t ndim  = 3;
	// const uint32_t dx = 3,dim_dx = 1;

	a.descriptor.data_type = dataType;
	a.descriptor.row_major_form = row_major_form;
	a.descriptor.number_of_dimensions = ndim;
	a.descriptor.dimensions[0] = 4;
	a.descriptor.dimensions[1] = 4;
	a.descriptor.dimensions[2] = 1;

	b.descriptor.data_type = dataType;
	b.descriptor.row_major_form = row_major_form;
	b.descriptor.number_of_dimensions = ndim;
	b.descriptor.dimensions[0] = 3;
	b.descriptor.dimensions[1] = 3;
	b.descriptor.dimensions[2] = 1;

    r.descriptor.data_type = dataType;
	r.descriptor.row_major_form = row_major_form;
	r.descriptor.number_of_dimensions = ndim;
    r.descriptor.dimensions[0] = 2;
	r.descriptor.dimensions[1] = 2;
	r.descriptor.dimensions[2] = 1;
	
	MemPoolRequest req;
	MemPoolResponse resp;

	//create tensor
    _err_ = createTensor(&a,&pool,&req,&resp) + _err_;
    _err_ = createTensor(&b,&pool,&req,&resp) + _err_;
    _err_ = createTensor(&r,&pool,&req,&resp) + _err_;

    if(_err_!=0)
		fprintf(stderr,"create Tensor FAILURE.\n");

	uint32_t element_size = sizeofTensorDataInBytes(a.descriptor.data_type); 
    fprintf(stderr,"1*****\n");

	//fill tensor A values
	/*
		{{1, 2, 3, 4},
		 {5, 6, 7, 8},
		 {...}
		 {13, ..., 16}}
	*/
	fillTensorValues(&a, 16, 2,&req,&resp);
	fillTensorValues(&b, 9, 1.5,&req,&resp);
	fillTensorValues(&r, 4, -1,&req,&resp);
    int strides[2] = {0, 0};
    int padding[4] = {0, 0, 0, 0};
    fprintf(stderr,"2*****\n");

    convTensors(&a, &b, &r, strides, padding);

	// printf("\nTensor A\n");
	// print2dTensor(&a,&req,&resp);
	// printf("\nTensor B\n");
	// print2dTensor(&b,&req,&resp);
	printf("\nTensor Result\n");
	print2dTensor(&r,&req,&resp);

	// printf("%" PRIx64 "",b.mem_pool_identifier->mem_pool_buffer[b.mem_pool_buffer_pointer+1]);
}

