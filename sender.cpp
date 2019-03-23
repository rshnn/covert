
using namespace std; 

#include"util.hpp"


class ConfigSender
{
	public:
	bool debug_mode; 

	ConfigSender(bool debug_mode)
	{
		debug_mode = debug_mode; 
	}
};




int main(int argc, char **argv)
{
	// setup 
	clock_t begin, end; 
	const ConfigSender configuration = ConfigSender(true); 


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



        // send a initiate sequence 

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


