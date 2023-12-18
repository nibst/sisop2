#include "server.hpp"

/* Server entrypoint */
int main(int argc, char** argv)
{
    try
    {
        // Create an instance of Server
        UDPServer server = UDPServer();

        // Start listening to connections
        server.run();
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
    }
    // End
    std::cout << "Server stopped safely" << std::endl;
	return 0;
}