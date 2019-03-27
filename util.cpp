
#include "util.hpp"
using namespace std;

/* Measure the time it takes to access a block with virtual address addr. */
CYCLES measure_one_block_access_time(ADDR_PTR addr)
{
	CYCLES cycles;

	asm volatile("mov %1, %%r8\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"mov %%eax, %%edi\n\t"
	"mov (%%r8), %%r8\n\t"
	"lfence\n\t"
	"rdtsc\n\t"
	"sub %%edi, %%eax\n\t"
	: "=a"(cycles) /*output*/
	: "r"(addr)
	: "r8", "edi");	

	return cycles;
}



/*
    Flushes all caches of the input address.  
*/
void CLFLUSH(ADDR_PTR addr)
{
    asm volatile ("clflush (%0)"::"r"(addr));
}



/*
    Returns a binary representation of input string 
    Returns pointer to character array in heap space. 
*/
char* convert_to_binary(char* input)
{
    size_t length = strlen(input) - 1;

    char* output = (char*) malloc(length * sizeof(char) + 1);   
    output[0] = '\0';

    for(int i=0; i<length; ++i)
    {
        char c = input[i];

        for(int j=7; j>=0; --j){
            if(c & (1 << j)) 
            {
                strcat(output, "1");
            } else {
                strcat(output, "0");
            }
        }
    }

    return output; 
}


// char* convert_from_binary()
// { 
// }