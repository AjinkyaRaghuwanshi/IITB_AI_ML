#ifndef _unary_inplace_fn_h____
#define _unary_inplace_fn_h____

<<<<<<< HEAD
#include <stdint.h>
#include "mempool.h"//mempool.h
#include "tensor.h"

=======
>>>>>>> 9b21310ae4f6644a76394d2f4c41a7fb81f5c3ac

// Maximum allowed chunk size is 1024 
#define CHUNK_SIZE      32 // in dwords --> (CHUNK_SIZE*8) bytes 

typedef enum {
	SINE, 
	EXP,
	RELU,
	SQUARE,
    ABSOLUTE,
	SIGMOID
} Operation;

// Datatypes supported include: u8, u16, u32, u64, i8, i16, i32, i64, float32, float64.
// 3-stages have been separately shown as an attempt to demonstrate pipelining in hardware model.

// ASSUMPTIONS:
//      1. The memory space for tensor in a mempool is contiguously allocated
//      2. Appropriate datatypes are used as per the input data. 
// 		3. The input tensor and output tensor datatypes are same by default 
// SUMMARY:
//      unaryOperateOnTensor performs unary operation on the given Tensor t
//		and writes the result back to same Tensor t. 
// SIDE-EFFECTS:
//      NULL
// RETURN VALUES:
//      NULL
void unaryOperateOnTensor_inplace(Tensor* t, Operation op);

#endif