#ifndef _concat_h__
#define _concat_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mempool.h"
#include "tensor.h"

#define CEILING(x,y) (((x) + (y) - 1) / (y))
#define MIN(a,b) (((a)<(b))?(a):(b))

// increments vec from dim_start dimension
void incrementCoordinateVectorFromDim (int ndim, int dim_start, uint32_t* dims, uint32_t* vec, uint8_t row_major_form);

// increments vec by a offset
void incrementCoordinateVectorByOffset (int ndim, int offset, uint32_t* dims, uint32_t* vec, uint8_t row_major_form);

// copies tensor array from index_low to index_high 
// src_offset: data offset in word
// dest_offset: data offset in word
// 012345678 012345678
//     (            )
// copy from     copy to
// i.e src_offset = 4 and dest_offset = 7
uint32_t copyTensorArray(TensorDescriptor* td, void* tensor_data_array,uint32_t* indices_low, uint32_t* indices_high,void* section_data_array,uint32_t src_offset,uint32_t dest_offset);

int concatTensors (Tensor* a, Tensor*  b, Tensor* result);

int concatTensorsAlongDim (Tensor* a, Tensor*  b, Tensor* result, uint32_t dim);

#endif