
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
    Returns 8 character binary representation of character 
    Returns character array in heap space.  
*/
char* character_to_binary(char c)
{
	char* binary = (char*) malloc(8 * sizeof(char)); 

    for(int i=7; i>=0; --i){
        if(c & (1 << i)) 
        {
            strcat(binary, "1");
        } else {
            strcat(binary, "0");
        }

    }
    return binary; 
}



/*
    Returns a binary representation of input string 
    Returns pointer to character array in heap space. 
*/
char* string_to_binary(string input)
{

    size_t length = input.length();

    char* output = (char*) malloc(length * sizeof(char) + 1);   
    output[0] = '\0';

    for(int i=0; i<length; ++i)
    {
        char c = input[i];
        strcat(output, character_to_binary(c));
    }

    return output; 
}

