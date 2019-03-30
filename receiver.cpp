
#include"util.hpp"

class ConfigReceiver
{
	public:
	bool debug_mode; 
	uint64_t period;  // in microseconds 
	uint64_t interval; //in cycles 
	list<ADDR_PTR> probe_list; 
	char* buffer; 

	
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



int listen_for_bit(ConfigReceiver* configuration)
{
	int sum = 0;
	int avg = 0;
	int access_count = 0;

	list<ADDR_PTR>::iterator i;
		
	
	while(1)
	{
		if(RDTSC() % configuration->interval == 0)//< configuration->interval/10)
			break; 
	}

	uint64_t start = RDTSC();
	uint64_t dt = RDTSC() - start; 
	// std::cout << "\tlistening at " << start << std::endl; 


	while(dt < configuration->period){

		for(i=configuration->probe_list.begin(); 
			i != configuration->probe_list.end();
			i++)
		{

			ADDR_PTR addr = (ADDR_PTR) *i;
			CYCLES x = measure_one_block_access_time(addr);

			if(x > 1000)
				continue; 

			sum = sum + x;
			access_count++; 


			uint64_t time_after_check = RDTSC(); 
			uint64_t sleep_time = RDTSC() - time_after_check; 
			while(sleep_time < configuration->period*0.01)
			{
				sleep_time = RDTSC() - time_after_check; 
			}



		}
		dt = RDTSC() - start; 
	}


	avg = (double) sum / (double) access_count;

	while(1){
		if(RDTSC() - start < configuration->interval*0.9)
			break;
	}



	if(avg >= 100)
	{
		// cout << "\ttotal " << sum << " count " << access_count << endl; 
		cout << "\t\tat cycle " << start << " avg cycles " << avg  << " " << sum << " " << access_count << endl; 
		return 1; 
	}else
	{
		return 0;
	}

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
	ConfigReceiver configuration = ConfigReceiver();
	configuration.debug_mode = true; 
	configuration.interval = INTERVAL; 
	configuration.period = PERIOD; 


	parse_input_flags(&configuration, argc, argv); 

	build_probe_list(&configuration);


	if(configuration.debug_mode)
		configuration.print_probe_list();
	
	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);




	printf("Receiver now listening.\n");
	bool listening = true;
	while (listening) {


		/*testing*/
		int bit = listen_for_bit(&configuration);
		// cout<< bit << " at " << RDTSC() << endl;	

		// Recognize initiate sequence & sync up with sender   





		// if(listen_for_init())
		// {
		// 	// Start listening for bits  
			
		// }


			


	}

	printf("Receiver finished.\n");

	return 0;
}




