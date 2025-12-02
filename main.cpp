#include "ft_irc.hpp"

int main(int argc, char **argv)
{
	try
	{
		if(argc != 3)
			throw BadArguments();
		std::string pass = argv[2];
		int port = strtol(argv[1], NULL, 10);
		if (port <= 0 || port > 65535)
			throw BindFailed();
		Server server(pass, port);

		server.start();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error :" << e.what() << '\n';
	}
	(void) argv;
}