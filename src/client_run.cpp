#include "client.hpp"
#include "profile.hpp"
#include <string.h>
int main(int argc, char** argv)
{
    // Parse command line input
    /*
        if (argc < 4){
        std::cerr << "Usage: " << argv[0] << " <username> <server_ip_address> <port> " << std::endl;
        return 1;
        }
    */
    try
    {   
        // Create an instance of Client
        UDPClient client(argv[1],argv[2]);

        // Setup a connection to the server
        client.setupConnection();

        // Start listening for the server
        client.getMessages();
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Exit with return code 0
	return 0;
}