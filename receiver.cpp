#include <fstream>
#include"util.hpp"

class ConfigReceiver
{
	public:
	bool debug_mode; 
	uint64_t period;  // in microseconds 
	list<ADDR_PTR> probe_list; 
	char* buffer; 
	int decision_boundary; 

	// used for testing
	list<CYCLES> probe_time;

	// 0 = seeking   
	// 1 = listening to init
	// 2 = reading message  
	int mode; 

	
	void print_probe_list()
	{
		
		cout << "Dumping probe list.." << endl; 
		list<ADDR_PTR>::iterator i; 
		for(i = probe_list.begin();
			i != probe_list.end(); 
			i++)
		{
			cout << *i << endl;
		}
	}

};




void build_probe_list(ConfigReceiver* configuration)
{

	int line_offsets = log2(CACHE_LINESIZE);
	int sets = log2(CACHE_L1_SETS);
	int c = 2;  		// constant multiplier 

	// Create a buffer of at least as large as the L1 cache.
	int buffer_size = c * CACHE_L1_ASSOC * exp2(line_offsets + sets);

	configuration->buffer = (char*) malloc(4 * buffer_size);

	for(int i=0; i < CACHE_L1_ASSOC * c; i++)
	{
		int idx = (int)exp2(line_offsets + sets) * i;
		ADDR_PTR addr = (ADDR_PTR) &(configuration->buffer[idx]);

		// Focus fire on a single cache set.  In particular, cache set 0.  
        if (cache_set_index(addr) == 0x0) {
			configuration->probe_list.push_back(addr);
		}	
	}
}



void time_correction(ConfigReceiver* configuration, int misses, int total)
{
	clock_t start = clock();

	// (TUNE) correction parameter:  hit expectation 
	double H = (double) total * 0.75;
	double out_of_sync = (1 - misses / H);

	double pause_time = configuration->period * out_of_sync;

	while((clock() - start) < pause_time)
	{
		// do nothing
	}

	return; 
}



int listen_for_bit(ConfigReceiver* configuration)
{
	int access_count = 0;
	int cache_hit = 0;
	int cache_miss = 0;
	double hit_ratio = -1; 

	list<ADDR_PTR>::iterator i;

	clock_t start = clock();
	clock_t dt = clock() - start; 


	while(dt < configuration->period){

		for(i=configuration->probe_list.begin(); 
			i != configuration->probe_list.end();
			i++)
		{

			ADDR_PTR addr = (ADDR_PTR) *i;
			CYCLES x = measure_one_block_access_time(addr);

			// for debugging decision boundary 	
			if(configuration->mode == 2)
			{
				configuration->probe_time.push_back(x);
			}


			if(x > 1000)
				continue; 

			access_count++; 

			if(x <= configuration->decision_boundary)
				cache_hit++;
			else
				cache_miss++;

			// (TUNE) lag time for sender to flush L3 
            for(int j = 0; 
            	j < (configuration->period * 0.2) &&
                (clock() - start) < configuration->period; 
                j++) 
            {
            	// do nothing 
            }

		}
		dt = clock() - start; 
	}

	// Apply time correction if in seek mode. 
	if(configuration->mode == 0)
	{
		// (TUNE) Percent of misses in seek mode to enact time_correction.  
		if(cache_miss > (access_count*0.20))
			time_correction(configuration, cache_miss, access_count);

		// (TUNE) Percent of hits in seek mode to return 1.  Relax the threshold.  
		if(cache_hit < ((double) access_count * 0.8))
			return 1; 
		else
			return 0; 
	}



	hit_ratio = (double) cache_hit / (double) access_count; 
	// (TUNE) ratio of total 
	if(hit_ratio < 0.5)
		return 1;
	else
		return 0; 

} 



void write_probe_times_to_file(ConfigReceiver* configuration)
{
	ofstream myfile("probes.txt");

	list<CYCLES>::iterator i;


	if (myfile.is_open())
	{
	
		for(i=configuration->probe_time.begin(); 
			i != configuration->probe_time.end(); 
			i++)
		{
				myfile << *i << " " ;
		}
	    myfile.close();
	}
	else cout << "Unable to open file";

}



/*
	Parse command line input flags.  
		-d for debug mode
		-p [double] to specify a different period length (in microseconds) 
*/
void parse_input_flags(ConfigReceiver* configuration, int argc, char** argv)
{
    int option = 0;
	string usage = "Usage: ./sender -d -p 170\n";
	string help = "\t-d for debug mode.\n\t-p to specify a period in microseconds.(must match with sender)\n";

    while ((option = getopt(argc, argv, "dp:")) != -1) {
        switch (option) {
            case 'd':
            	cout << "DEBUG MODE" << endl; 
                configuration->debug_mode = true;
                break;
            
            case 'p':
                configuration->period = atoi(optarg);
            	cout << "Setting period to " << configuration->period << endl;
                break;
            
            case '?':
            	cout <<  usage + help << endl;
                exit(1);
            
            default:
                exit(1);
        }
    }
}



int main(int argc, char **argv)
{
	// setup 
	ConfigReceiver configuration = ConfigReceiver();
	configuration.debug_mode = false; 
	configuration.period = PERIOD; 
	configuration.decision_boundary = DECISION_BOUNDARY; 
	configuration.mode = 0;
	// maximum size of message is 128 chars.  8 bits per char.  
	char* bitstring = (char*) malloc(8 * 128); 
	int bitstring_size = 0; 

	parse_input_flags(&configuration, argc, argv); 

	build_probe_list(&configuration);


	if(configuration.debug_mode)
		configuration.print_probe_list();
	
	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);


	int bit = -1;
	int consecutive_zeros = 0; 
	int i; 

	printf("Receiver now listening.\n");
	bool listening = true;
	while (listening) {

		// Recognize initiate sequence & sync up with sender  
		// Init sequence is 10011011 
		// TODO clean up with loop 
		bit = listen_for_bit(&configuration);

		if(bit == 1)
		{
			configuration.mode = 1;
			bit = listen_for_bit(&configuration);
			if(bit == 0)
			{
				bit = listen_for_bit(&configuration);
				if(bit == 0)
				{
					bit = listen_for_bit(&configuration);
					if(bit == 1)
					{
						bit = listen_for_bit(&configuration);
						if(bit == 1)
						{
							bit = listen_for_bit(&configuration);
							if(bit == 0)
							{
								bit = listen_for_bit(&configuration);
								if(bit == 1)
								{
									bit = listen_for_bit(&configuration);
									if(bit == 1)
									{
										configuration.mode = 2; 
										if(configuration.debug_mode)
											cout << " Found the sequence " << endl;
									}
								}

							}
						}
					}
				}
			}	
		}


		// detected init sequence.  reading message 
		if(configuration.mode == 2)
		{

			for(i=0; i<(128*8); i++)
			{
				bit = listen_for_bit(&configuration); 
				bitstring_size++; 

				// detect end of message (null terminator = 8 0 bits)
				if(bit == 1)
				{
					strcat(bitstring, "1");
					consecutive_zeros = 0;
				}
				if(bit == 0)
				{
					strcat(bitstring, "0");
					consecutive_zeros++; 

					if( (bitstring_size % 8) == 0)
					{
						if(consecutive_zeros == 8)
						{
							if(configuration.debug_mode)
								cout << "End of message" << endl; 
							bitstring[bitstring_size] = '\0';
							break; 	
						}else{
							consecutive_zeros = 0;
						}
					}

				}

			}
			
			if(configuration.debug_mode)
				cout << "bitstring: " << bitstring << endl; 

			// decode the message 
			char* payload;  
			payload = convert_from_binary(bitstring, bitstring_size); 

			cout << "messsage: " << payload << endl; 
			write_probe_times_to_file(&configuration);
		}


			
		// Clean up run-state vars for next iteration 
		configuration.mode = 0; 
		bitstring[0] = '\0';
		bitstring_size = 0; 
	}

	printf("Receiver finished.\n");

	return 0;
}




