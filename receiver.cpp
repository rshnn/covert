
#include"util.hpp"

class ConfigReceiver
{
	public:
	bool debug_mode; 
	double period;  // in microseconds 
	list<ADDR_PTR> probe_list; 
	char* buffer; 

	ConfigReceiver(bool debug_mode, double period)
	{
		debug_mode = debug_mode; 
		period = period; 
	}


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

	list<ADDR_PTR>::iterator i;
	for(i=configuration->probe_list.begin(); 
		i != configuration->probe_list.end();
		i++)
	{

		ADDR_PTR addr = (ADDR_PTR) *i;
		CYCLES x = measure_one_block_access_time(addr);

		sum = sum + x;

		// cout << "cycles for " << addr << " " << x << endl; 
	}

	avg = (double) sum / (double) configuration->probe_list.size();

	// cout << "at cycle" << RDTSC() << " avg cycles " << avg << endl; 

	if(avg >= 100)
	{
		return 1; 
	}else
	{
		return 0;
	}

} 



void sync(ConfigReceiver* configuration)
{

	// cout << clock() << endl;

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
	ConfigReceiver configuration = ConfigReceiver(true, PERIOD);
	parse_input_flags(&configuration, argc, argv); 

	build_probe_list(&configuration);


	if(configuration.debug_mode)
		configuration.print_probe_list();
	
	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);


	// sync(&configuration);



	printf("Receiver now listening.\n");
	bool listening = true;
	while (listening) {


		/*testing*/
		int bit = listen_for_bit(&configuration);
		cout<< bit << " at " << RDTSC() << endl;	
		sleep(configuration.period * 100);

		// Recognize initiate sequence & sync up with sender   



		// if(listen_for_init())
		// {
		// 	// Start listening for bits  
			
		// }


			


	}

	printf("Receiver finished.\n");

	return 0;
}




