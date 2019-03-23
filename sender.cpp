
using namespace std; 

#include"util.hpp"


class ConfigSender
{
	public:
	bool debug_mode; 
	double period;  // in microseconds 

	ConfigSender(bool debug_mode, double period)
	{
		debug_mode = debug_mode; 
		period = period; 
	}
};



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
	ConfigSender configuration = ConfigSender(true, PERIOD);
	parse_input_flags(&configuration, argc, argv);  


	printf("Please type a message.\n");

	bool sending = true;
	while (sending) {
		char text_buf[128];
		fgets(text_buf, sizeof(text_buf), stdin);
	
		// Turn buffer into binary (8 bits per character) 
		char* binary_payload = string_to_binary(text_buf); 

		if(configuration.debug_mode)
			cout << "binary message\n" << binary_payload << endl; 


		// START THE CLOCK! 
        begin = clock();



        // send an initiate sequence 


        // send payload 
        size_t payload_length = strlen(binary_payload);
        for(int i=0; i<payload_length; i++)
        {
        	if(binary_payload[i] == '1')
        	{
        		// flush LLC

        	}else{
        		// wait 
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


