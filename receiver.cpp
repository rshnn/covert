
#include"util.hpp"

class ConfigReceiver
{
	public:
	bool debug_mode; 
	double period;  // in microseconds 
	list<ADDR_PTR> probe_list; 

	ConfigReceiver(bool debug_mode, double period)
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
	parse_input_flags(&configuration); 

	build_probe_list(&configuration);

	printf("Please press enter.\n");

	char text_buf[2];
	fgets(text_buf, sizeof(text_buf), stdin);

	printf("Receiver now listening.\n");

	bool listening = true;
	while (listening) {


		


	}

	printf("Receiver finished.\n");

	return 0;
}




