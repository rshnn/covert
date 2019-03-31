
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



uint64_t RDTSC()
{
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
 


/*
    Flushes all caches of the input address.  
*/
void CLFLUSH(ADDR_PTR addr)
{
    asm volatile ("clflush (%0)"::"r"(addr));
}



/*
 * Returns the 10 bits cache set index of a given address.
 */
uint64_t cache_set_index(ADDR_PTR addr)
{
    uint64_t mask = ((uint64_t) 1 << 16) - 1;
    return (addr & mask) >> 6;
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

    strcat(output, "00000000");

    return output; 
}


char* convert_from_binary(char* bitstring, int bitstring_size)
{ 
    int i;
    int num_chars = (int)bitstring_size / 8; 
    char* message = (char*) malloc(num_chars);


    // char* temp = bitstring; 
    int idx = 0;
    char eight_bits[8];

    for(i=0; i<num_chars-1; i++)
    {
        strncpy(eight_bits, &bitstring[idx], 8);
        char character = strtol(eight_bits, 0, 2);

        message[i] = character; 
        idx = idx + 8; 

    }

    message[num_chars] = '\0';

    return message; 
}
