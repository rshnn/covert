
#include"util.hpp"
using namespace std; 

class ConfigSender
{
	public:
	bool debug_mode; 
	uint64_t period;  // in microseconds 
	uint64_t interval;  //in cycles 
	char* buffer; 
	list<ADDR_PTR> eviction_list; 


	void print_eviction_list()
	{
		
		cout << "Dumping eviction list.." << endl; 
		list<ADDR_PTR>::iterator i; 
		for(i = eviction_list.begin();
			i != eviction_list.end(); 
			i++)
		{
			cout << *i << endl;
		}
	}

};


/*
	Adds all addresses in buffer to the eviction list.
		Buffer is the size of the L3 cache.    
*/
void build_eviction_list(ConfigSender* configuration)
{

	int line_offsets = log2(CACHE_LINESIZE); 
	int sets = log2(CACHE_L3_SETS);

	// Create buffer at least as large as the L3 cache 
	// TODO what is c? 
	int buffer_size = CACHE_L3_ASSOC * exp2(line_offsets + sets);
	configuration->buffer = (char*) malloc(buffer_size);

	for(int i=0; i < CACHE_L3_SETS ; i++)
	{
    	for(int j=0; j < CACHE_L3_ASSOC; j++)
    	{
    		int idx = (int)(i * exp2(line_offsets)) + (int)(j * exp2(line_offsets + sets));
	    	ADDR_PTR addr = (ADDR_PTR) &(configuration->buffer[idx]);

			// Focus fire on a single cache set.  In particular, cache set 0.  
	        if (cache_set_index(addr) == 0x0) {
	    		configuration->eviction_list.push_back(addr); 
			}	
    	}
	}
}



/*
	Send a zero bit to the receiver through the covert channel.  
		To send zero, the cache is NOT flushed.  
		Stalls for configuration.period microseconds.  
*/
void send_zero(ConfigSender* configuration)
{
	
	uint64_t start = RDTSC();
	// clock_t start; 
	// start = clock();


	while(RDTSC() - start < configuration->period){
		// do nothing 
	}
}



/*
	Send a one bit to the receiver through the covert channel.  
		To send one, the entire LLC cache is flushed.  
		Flush until configuration.period microseconds has passed.  
*/
void send_one(ConfigSender* configuration)
{

	// while((RDTSC() - start) < configuration->period)
	
	while(1)
	{
		if(RDTSC() % configuration->interval == 0)
			break; 
	}
	


	uint64_t start = RDTSC();
	uint64_t dt = RDTSC() - start; 
	std::cout << "sending 1 " << start << std::endl; 
	
	// Sender flushes for 90% of the interval 
	while(dt < configuration->interval * 0.9)  
	{	
				
		list<ADDR_PTR>::iterator i; 
	
		for(i=configuration->eviction_list.begin(); 
			i != configuration->eviction_list.end(); 
			i++)
		{
			ADDR_PTR address = (ADDR_PTR) *i; 
			CLFLUSH(address);
		}

		dt = RDTSC() - start; 
	}

	std::cout << "done sending 1 " << RDTSC() << std::endl; 



}

/*
	Parse command line input flags.  
		-d for debug mode
		-p [double] to specify a different period length (in microseconds) 
*/
void parse_input_flags(ConfigSender* configuration, int argc, char** argv)
{
    int option = 0;
	string usage = "Usage: ./sender -d -p 170\n";
	string help = "\t-d for debug mode.\n\t-p to specify a period in microseconds.\n";

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
	clock_t begin, end; 
	ConfigSender configuration = ConfigSender();
	configuration.debug_mode = true; 
	configuration.interval = INTERVAL; 
	configuration.period = PERIOD; 


	parse_input_flags(&configuration, argc, argv);  

	build_eviction_list(&configuration); 
	
	if(configuration.debug_mode)
		configuration.print_eviction_list(); 


	bool sending = true;
	while (sending) {
		char text_buf[128];
		printf("Please type a message.\n");
		fgets(text_buf, sizeof(text_buf), stdin);
	
		// Turn buffer into binary (8 bits per character) 
		char* binary_payload = convert_to_binary(text_buf); 

		if(configuration.debug_mode)
			cout << "binary message\n" << binary_payload << endl; 


		// START THE CLOCK! 
        begin = clock();



        // send an initiate sequence  110011
        send_one(&configuration);
        send_one(&configuration);
        send_zero(&configuration);
        send_zero(&configuration);
        send_one(&configuration);
        send_one(&configuration);



        // send payload 
        size_t payload_length = strlen(binary_payload);
        for(int i=0; i<payload_length; i++)
        {
        	if(binary_payload[i] == '1')
        	{
        		// flush LLC
        // 		if(configuration.debug_mode)
	    			// cout << RDTSC() << " Sending 1." << endl;
        		send_one(&configuration);
        		// if(configuration.debug_mode)
	    			// cout <<  RDTSC()  << " Sent 1." << endl;
        	}else{
        		// do nothing 
        // 		if(configuration.debug_mode)
	    			// cout <<  RDTSC() << " Sending 0." << endl;
        		send_zero(&configuration);
        		// if(configuration.debug_mode)
        		// 	cout <<  RDTSC() << " Sent 0." << endl;
        	}

        } 


        // STOP THE CLOCK! 
        end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; 
        double bitrate = (double) strlen(text_buf) / (double) time_spent; 
        cout << "\nbitrate (B/s): " << bitrate << endl; 

	}



	printf("Sender finished.\n");

	return 0;
}


