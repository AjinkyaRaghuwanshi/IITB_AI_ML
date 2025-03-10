#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "mempool.h"
#include "tensor.h"
#include "createTensor.h"
#include "readWriteTensorsFromStandardIO.h"

// This testbench reads a csv file as input, creates a tensor
// with the data available data inside it and writes back the 
// data from the created tensor into a new file.

MemPool pool1;
Tensor image_A;
int _err_ = 0;

#define MAX_PAGES 1024

void fillTensorDescriptor(Tensor t)
// Takes details from the user about the tensor to be created.
{
    Tensor dummy;
    int i,num_elems=1;

    printf("Enter the data-type of the tensor:\n");
    printf("0. uint_8\t1. uint16_t\t2. uint32_t\t3. uint64_t\n");
    printf("4. int8_t\t5. int16_t\t6. int32_t\t7. int64_t\n");
    printf("8. float8\t9. float16\t10. float\t11. double\n");
    scanf("%u",&dummy.descriptor.data_type);
    printf("Enter:\t0.Column-Major form \t1.Row-Major form\n");
    scanf("%u",&dummy.descriptor.row_major_form);
    if(dummy.descriptor.row_major_form > 1 || dummy.descriptor.row_major_form<0)
    {
        printf("ERROR!SELECT PROPER VALUES.\n");
        exit(0);
    }
    printf("Enter number of dimensions:");
    scanf("%u",&dummy.descriptor.number_of_dimensions);
    if(dummy.descriptor.number_of_dimensions > 64)
    {
        printf("ERROR! MAX DIMENSION PERMISSIBLE IS 64.");
        exit(0);
    }
    printf("Fill the dimensional array:");
    for (i=0;i< dummy.descriptor.number_of_dimensions;i++)
    {
        scanf("%u",&dummy.descriptor.dimensions[i]);
    }
    t.descriptor.data_type = dummy.descriptor.data_type;
    t.descriptor.row_major_form = dummy.descriptor.row_major_form;
    t.descriptor.number_of_dimensions = dummy.descriptor.number_of_dimensions;
    for(i=0;i< dummy.descriptor.number_of_dimensions;i++)
    {
        t.descriptor.dimensions[i] = dummy.descriptor.dimensions[i];
    }
}

int main(int argc, char *argv[])
{
    initMemPool(&pool1,1,MAX_PAGES);

    _err_ = readTensorFromFile("util/sample_csv/newformat.csv", &image_A, &pool1) ||
            writeTensorToFile("util/sample_csv/def_write.csv", &image_A) ||
            _err_;

    _err_ = destroyTensor(&image_A) || _err_; 
    if(_err_)
        printf("ERROR!\n");
    else
        printf("SUCCESS!\n");
    return 0;
}

