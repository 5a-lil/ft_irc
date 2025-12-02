#include "Server.hpp"

int g_stop = 0;

void ft_signal(int signum)
{
    (void)signum;
    std::cout << "Signal reçu ! Arrêt du serveur" << std::endl;
    g_stop = 1;
}

struct pollfd create_pollfd(int sock)
{
	struct	pollfd nfd;
	nfd.fd = sock;
	nfd.events = POLLIN;
	nfd.revents = 0;
	return (nfd);
}

Server::Server(std::string password, int port): _password(password), _port(port){}

Server::~Server(){}

void Server::checkRegistration(Client *client)
{
	if (client->getConnected())
	{
		std::string errorRPL = ERR_ALREADYREGISTRED + client->getNick() + " :You may not reregister\r\n";
        send(client->getFd(), errorRPL.c_str(), errorRPL.size(), 0); return;
	}
    if (client->getVerif() && !client->getNick().empty() && !client->getUser().empty())
    {
        std::string nick = client->getNick();
        std::string user = client->getUser();
        
        // Envoyer les messages de bienvenue
        // RPL_WELCOME (001)
        std::string welcome = RPL_WELCOME + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@localhost\r\n";
        send(client->getFd(), welcome.c_str(), welcome.size(), 0);
        
        // RPL_YOURHOST (002)
        std::string yourhost = RPL_YOURHOST + nick + " :Your host is localhost, running version 1.0\r\n";
        send(client->getFd(), yourhost.c_str(), yourhost.size(), 0);
        
        // RPL_CREATED (003)
        // std::string created = ":localhost 003 " + nick + " :This server was created June 2025\r\n";
        // send(client.getFd(), created.c_str(), created.size(), 0);
        
        // RPL_MYINFO (004)
        std::string myinfo = RPL_MYINFO + nick + " ircserv\r\n";
        send(client->getFd(), myinfo.c_str(), myinfo.size(), 0);

		client->setConnected(true);
    }
}

Channel* Server::findChannel(std::string name)
{
	std::vector<Channel*>::iterator it;

	for (it = this->_channels.begin(); it != this->_channels.end(); it++)
	{
		if ((*it)->getName() == name)
			return *it;
	}
	return (NULL);
}

Client *Server::findClient(const std::string& nick)
{
	std::vector<Client *>::iterator end = _clients.end();
	for (std::vector<Client *>::iterator it = _clients.begin(); it != end; it++)
		if ((*it)->getNick() == nick)
			return *it;
	return (NULL);
}

void Server::handleCommand(Client *client, const std::string &line)
{
	std::istringstream iss(line);
	std::string command;
	iss >> command;

	t_NameToFunc cmds[] = {
		{ "PASS", &Server::handlePass },
		{ "NICK", &Server::handleNick },
		{ "USER", &Server::handleUser },
		{ "JOIN", &Server::handleJoin },
		{ "PRIVMSG", &Server::handlePrivateMessage },
		{ "PING", &Server::handlePing },
		{ "KICK", &Server::handleKick },
		{ "TOPIC", &Server::handleTopic },
		{ "MODE", &Server::handleMode },
		{ "INVITE", &Server::handleInvite },
		{ "PART", &Server::handlePart },
		{ "QUIT", &Server::handleQuit },
		{ "", NULL }
	};
	short i = -1; 
	while (cmds[++i].RAW != "") 
		if (cmds[i].RAW == command) 
		{
			if (command != "PASS" && command != "NICK" && command != "USER" && !client->getConnected()) 
			{
				std::string errorReply = std::string(ERR_NOTREGISTERED) + "* :You have not registered\r\n";
        		send(client->getFd(), errorReply.c_str(), errorReply.size(), 0); return;
			}
			return ((this->*cmds[i].handle)(client, line), (void)0);
		}
}

void Server::add_client()
{
	int sock = this->_fds[0].fd;
	sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int clientfd = accept(sock, (struct sockaddr *)&client, &client_size);
	if (clientfd < 0)
		return;

	Client * new_client = new Client(clientfd);

	this->_clients.push_back(new_client);
	this->_fds.push_back(create_pollfd(clientfd));
}

void Server::handleClientInput(Client *client, const std::string &input, size_t fd_index)
{
    (void)fd_index;
	std::istringstream iss(input);
	std::string line;

	while (std::getline(iss, line))
	{
		if (!line.empty() && line[line.length() - 1] != '\r')
		{
			client->addBuffer(line);
			return;
		}
		if (!line.empty() && line[line.length() - 1] == '\r')
		{
			if (!client->getBuffer().empty())
			{
				line = client->getBuffer() + line.substr(0, line.length() - 1);
				client->setBufferEmpty();
			}
			else
				line = line.substr(0, line.length() - 1);
		}
		handleCommand(client, line);
	}
}

void Server::eraseUserServer(Client* user)
{
	std::vector<Client*>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), user);
	if (it != this->_clients.end())
        this->_clients.erase(it);
}

void Server::eraseChannelServer(Channel* channel)
{
	std::vector<Channel*>::iterator it = std::find(this->_channels.begin(), this->_channels.end(), channel);
	if (it != this->_channels.end())
        this->_channels.erase(it);
}

void Server::read_client()
{
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].revents & POLLIN)
		{
			if (i == 0)
			{
				add_client();
			}
			else
			{
				char receipt[4096];
				int bytes_receive = recv(this->_fds[i].fd, receipt, sizeof(receipt) - 1, 0);

				if (bytes_receive > 0)
				{
					receipt[bytes_receive] = '\0';
					std::string recu = receipt;
					std::cout << receipt;

					for (size_t j = 0; j < this->_clients.size(); ++j)
					{
						if (this->_clients[j]->getFd() == this->_fds[i].fd)
						{
							handleClientInput(this->_clients[j], recu, i);
							break;
						}
					}
				}
				else
				{
					close(this->_fds[i].fd);
					this->_fds.erase(this->_fds.begin() + i);
					for (size_t j = 0; j < this->_clients.size(); ++j)
					{
						if (this->_clients[j]->getFd() == this->_fds[i].fd)
						{
							this->eraseUserServer(this->_clients[j]);
						}
					}
					--i;
				}
			}
		}
		else if (this->_fds[i].revents & POLLHUP)
		{
			close(this->_fds[i].fd);
			this->_fds.erase(this->_fds.begin() + i);
			for (size_t j = 0; j < this->_clients.size(); ++j)
			{
				if (this->_clients[j]->getFd() == this->_fds[i].fd)
				{
					this->eraseUserServer(this->_clients[j]);
				}
			}
			--i;
		}
	}
}

void Server::init()
{
	int sock = socket(AF_INET, SOCK_STREAM,  IPPROTO_TCP);
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (sock == -1)
		std::cerr << "Error : Socket failed to create\n";
	
	sockaddr_in ipport;
	
	ipport.sin_family = AF_INET;
	ipport.sin_port = htons(this->_port);
	ipport.sin_addr.s_addr = inet_addr("127.0.0.1");	
	
	if (bind(sock, (struct sockaddr *)&ipport, sizeof(ipport)) == -1)
	{
		close(sock);
		throw BindFailed();
	}
		
	if (listen(sock, 10) == -1)
	{
		close(sock);
		throw ListenFailed();
	}

	this->_fds.push_back(create_pollfd(sock));
}

void Server::start()
{
	this->init();
	signal(SIGINT, ft_signal);
	signal(SIGQUIT, ft_signal);

	while (!g_stop)
	{
		int ret = poll(&this->_fds[0], this->_fds.size(), 100);
		if (ret > 0)
		    read_client();
	}
	
	// Fermeture de tous les sockets;
	for (size_t i=0; i < this->_fds.size(); i++)
		close(this->_fds[i].fd);
	
	for (size_t i=0; i < this->_clients.size(); i++)
		delete this->_clients[i];
	for (size_t i=0; i < this->_channels.size(); i++)
		delete this->_channels[i];
}