#include "../include_2/maxPoolOfTensors.h"

// Compute size of tensor
uint32_t getSizeOfTensor(Tensor *T)
{
	uint32_t size = 1;
	for (int i = 0; i < T->descriptor.number_of_dimensions; i++){
		size *= T->descriptor.dimensions[i];
	}
	return size;
}

void updateOutputDescriptorMaxPoolOfTensors(Tensor *src, Tensor *dst, int l, int stride, int num_dims_to_pool,int * dims_to_pool, int mode){
	dst->descriptor.row_major_form = src->descriptor.row_major_form;
	dst->descriptor.data_type = src->descriptor.data_type;
	dst->descriptor.number_of_dimensions = src->descriptor.number_of_dimensions;
	int8_t j;

	for (j = 0; j <  src->descriptor.number_of_dimensions; j++)
	// Loop through all dimensions
	{
		uint32_t x = src->descriptor.dimensions[j];
		dst->descriptor.dimensions[j] = x;
	}
	for (j = 0; j <  num_dims_to_pool; j++)
	{	
		uint32_t x = src->descriptor.dimensions[dims_to_pool[j]];
		int32_t factor = (x<l) ? mode : ((mode == 0) ? 1+((x-l)/stride) : 1+((x-1)/stride));
		dst->descriptor.dimensions[dims_to_pool[j]] = factor;
	}
}

// Core operation of maxPool
// Computes the max of num_max quantities in matrix starting at indice start based on the datatype
// Returns the data in at location temp
void maxWithSpacing(int num_max, int start, void* matrix,  TensorDataType dt, void * temp,int pos)
{	
	assert(num_max > 0);

	switch (dt)
	// Select comparator based on datatype
		{
		case i8:
		*((int8_t*)temp+pos ) = *((int8_t*)(matrix) +(start)*8);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((int8_t*)(matrix) +(start+i)*8)> *((int8_t*)temp+pos )) *((int8_t*)temp+pos ) = *((int8_t*)(matrix) +(start+i)*8);
		}
		break;
		case float8:
		case u8:
		*((uint8_t*)temp+pos ) = *((uint8_t*)(matrix) +(start)*8);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((uint8_t*)(matrix) +(start+i)*8)> *((uint8_t*)temp+pos )) *((uint8_t*)temp+pos ) = *((uint8_t*)(matrix) +(start+i)*8);
		}
		break;
		case i16:
		*((int16_t*)temp+pos ) = *((int16_t*)(matrix) +(start)*4);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((int16_t*)(matrix) +(start+i)*4)> *((int16_t*)temp+pos )) *((int16_t*)temp+pos ) = *((int16_t*)(matrix) +(start+i)*4);
		}
		break;
		case float16:
		case u16:
		*((uint16_t*)temp+pos ) = *((uint16_t*)(matrix) +(start)*4);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((uint16_t*)(matrix) +(start+i)*4)> *((uint16_t*)temp+pos )) *((uint16_t*)temp+pos ) = *((uint16_t*)(matrix) +(start+i)*4);
		}
		break;
		case i32:
		*((int32_t*)temp+pos ) = *((int32_t*)(matrix) +(start)*2);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((int32_t*)(matrix) +(start+i)*2)> *((int32_t*)temp+pos )) *((int32_t*)temp+pos ) = *((int32_t*)(matrix) +(start+i)*2);
		}
		break;
		case float32:
		*((float*)temp+pos ) = *((float*)(matrix) +(start)*2);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((float*)(matrix) +(start+i)*2)> *((float*)temp+pos )) *((float*)temp+pos ) = *((float*)(matrix) +(start+i)*2);
		}
		break;
		case u32:
		*((uint32_t*)temp+pos ) = *((uint32_t*)(matrix) +(start)*2);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((uint32_t*)(matrix) +(start+i)*2)> *((uint32_t*)temp+pos )) *((uint32_t*)temp+pos ) = *((uint32_t*)(matrix) +(start+i)*2);
		}
		break;
		case i64:
		*((int64_t*)temp+pos) = *((int64_t*)(matrix) + start);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((int64_t*)(matrix) +(start+i)*1)> *((int64_t*)temp+pos)) *((int64_t*)temp+pos) = *((int64_t*)(matrix) +(start+i)*1);
		}
		break;
		case float64:
		*((double*)temp+pos) = *((double*)(matrix) + start);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((double*)(matrix) +(start+i)*1)> *((double*)temp+pos)) *((double*)temp+pos) = *((double*)(matrix) +(start+i)*1);
		}
		break;
		case u64:
		*((uint64_t*)temp+pos) = *((uint64_t*)(matrix) + start);
		for (int i = 1;i< num_max;i++)
		{
			// Update if larger
			if (*((uint64_t*)(matrix) +(start+i)*1)> *((uint64_t*)temp+pos)) *((uint64_t*)temp+pos) = *((uint64_t*)(matrix) +(start+i)*1);
		}
		break;
		default:
			break;
		}
	return;
}

// Pools one dimension at a time, parameters size , x , l ,s ,cs , mode
// Input tensor src, output dst
void maxpool1D(Tensor *src, uint32_t size, uint32_t x, int l, int s, int cs, Tensor *dst, int mode)
{
	MemPoolRequest req1, *req;
	MemPoolResponse resp1, *resp;
	req = &req1;
	resp = &resp1;
	// Compute essential constants
	uint8_t dt = src->descriptor.data_type;
	uint8_t dsize = sizeofTensorDataInBytes(dt);
	// dim_to_pool refers to the dimension along which we are currently pooling
	// Number of pooling units = size of tensor upto dim_to_pool (dim_to_pool not included)
	int num_units = size/(x*cs);
	// num_1D_steps = floor/ceil (1 + ((x-l)/s)), i.e. the size of output along dim_to_pool 
	int num_1D_steps = ((mode == floor) ? 1 + (x-l)/s : 1 + (x-1)/s);
	if (l>x)
	{
		fprintf(stderr,"Warning: Length exceeds dimension.\n");
		num_1D_steps = mode;
	}

	// Define temporary memory variables
	// Future goal: Improve size by packing data in temp_old
	uint64_t temp_new , temp_old[l];

	// Stride = 1; number of elements to fetch = 1
	req->arguments[0] = 1;
	req->arguments[2] = 1;

	uint32_t i=0,j=0,k=0;
	for (i = 0;i<num_units;i++)
	{	
		for (j = 0; j < num_1D_steps; j++)
		{
			for (k = 0;k < cs;k++)
			{	
				// Read the line from src
				// Optimization opportunity: Use fetched values from previous pool (if overlapping values)
				req->request_type = READ;
				uint32_t var_max = ((l < x - j*s) ? l : x - j*s);
				for (uint32_t var = 0; var < var_max; var++)
				{
					req->arguments[1] = src->mem_pool_buffer_pointer+ (i*x*cs+k+(j*s+var)*cs)*dsize/8;
					memPoolAccess((MemPool*)(src->mem_pool_identifier),req,resp);
					if (resp->status == NOT_OK)
					{
						fprintf(stderr,"Mempool read error. Called from maxpool1D()");
						exit(-1);
					}
					temp_old[var] = (resp->read_data[0] >> (8*dsize*((i*x*cs+k+(j*s+var)*cs)&((8/dsize)-1))));
				}

				// Perform max operation
				maxWithSpacing(var_max, 0,temp_old, dt,&temp_new,(i*num_1D_steps*cs + k + j*cs) & ((8/dsize)-1));
				
				if (((i*num_1D_steps*cs + k+j*cs)&((8/dsize)-1))==(8/dsize-1)){
					
					// Write back to dst
					req->request_type = WRITE;
					req->arguments[1] = dst->mem_pool_buffer_pointer+ (i*num_1D_steps*cs + k+j*cs)*dsize/8;
					req->write_data[0] = temp_new;
					memPoolAccess((MemPool*)(dst->mem_pool_identifier),req,resp);
					if (resp->status == NOT_OK)
					{
						fprintf(stderr,"Mempool write error. Called from maxpool1D()");
						exit(-1);
					}
				}
			}			
		}
	}
	if ((((i-1)*num_1D_steps*cs + (k-1)+(j-1)*cs)&((8/dsize)-1))!=(8/dsize-1)){
		req->request_type = WRITE;
		req->arguments[1] = dst->mem_pool_buffer_pointer+ ((i-1)*num_1D_steps*cs + (k-1)+(j-1)*cs)*dsize/8;
		req->write_data[0] = temp_new;
		memPoolAccess((MemPool*)(dst->mem_pool_identifier),req,resp);
		if (resp->status == NOT_OK)
		{
			fprintf(stderr,"Mempool write error. Called from maxpool1D()");
			exit(-1);
		}
	}
}


void maxPoolOfTensors (Tensor *src, Tensor *dst, int l, int stride, int num_dims_to_pool, int * dims_to_pool, int mode)
{
	// Assure that the inputs lie in a meaningful range
	assert(num_dims_to_pool > 0);
	assert(num_dims_to_pool <= src->descriptor.number_of_dimensions);
	assert(dims_to_pool[0]>=0);
	assert(dims_to_pool[num_dims_to_pool-1]<=src->descriptor.number_of_dimensions);
	assert(l>0);
	assert(stride>0);
	
	// Define and initialize variables
	uint8_t row_major = src->descriptor.row_major_form;
	uint64_t size = getSizeOfTensor(src);
	uint32_t x;
	int64_t cs = 1;

	// Decide direction of movement based on row-major/column-major form
	int8_t i,j,iStart,iEnd,iInc,jStart,jEnd,jInc;
	if (row_major == 1)
	{
		iStart = num_dims_to_pool - 1;
		iInc = -1;
		iEnd = 0;
		jStart = src->descriptor.number_of_dimensions - 1;
		jEnd = -1;
		jInc = -1;
	}
	else
	{
		iStart = 0;
		iInc = 1;
		iEnd = num_dims_to_pool - 1;
		jEnd = src->descriptor.number_of_dimensions;
		jStart = 0;
		jInc = 1;
	}
	i = iStart;


	/* //Future optimization
	//Do the most common case (2D pool with continuous dims and l = stride)
	//efficiently
	if (num_dims_to_pool == 2)
	{
		return;
	}
	*/

	// To be later replaced with
	// Tensor temp_Tensor = createTensor(...);
	Tensor temp_Tensor;

	Tensor *in,*out;
	// Generalised maxPool (num_dims_to_pool > 1)
	for (j = jStart; j != jEnd;j += jInc)
	// Loop through all dimensions
	{
		x = src->descriptor.dimensions[j];
		if ((j == dims_to_pool[i]) && (i != (iEnd+iInc)))
		// Found a dimension to pool
		{
			in = src;
			out = dst;

			if ((i == iStart) && (i == iEnd))
			{
			}
			
			// First dimension : From src to temp_Tensor
			else if (i == iStart)
			{
				updateOutputDescriptorMaxPoolOfTensors(src, &temp_Tensor, l, stride, 1, &dims_to_pool[i], mode);
				createTensorAtHead(&temp_Tensor,(MemPool*)src->mem_pool_identifier);
				out = &temp_Tensor;
			}

			// Last dimension : From temp_Tensor to dst
			else if	(i == iEnd)
			{
				in = &temp_Tensor;
			}

			// Intermediate dimensions : From temp_Tensor to temp_Tensor
			else 
			{
				in = &temp_Tensor;
				out = &temp_Tensor;
			}
			maxpool1D(in, size, x, l, stride, cs, out, mode);			
			// Update parameters
			int32_t factor = (x<l) ? mode : ((mode == 0) ? 1+((x-l)/stride) : 1+((x-1)/stride));
			size = size*factor/x;
			cs *= factor;
			i += iInc;
		}
		else
		{
			// Update parameters
			cs *= x;
		}
	}
	if (in == &temp_Tensor)
		destroyTensor(&temp_Tensor);
	return;
}