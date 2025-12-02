#include "Channel.hpp"

Channel::Channel(std::string name) : _name(name), _modes("+") {}

Channel::~Channel(){}


std::string Channel::getName()
{
	return _name;
}

void Channel::addUser(Client* user)
{
	this->_users.push_back(user);
}

void Channel::addOperator(Client* user)
{
	this->_operators.push_back(user);
}

void	Channel::addInvited(Client* user)
{
	this->_invited.push_back(user);
}

void Channel::eraseUser(Client* user)
{
	std::vector<Client*>::iterator it = std::find(this->_users.begin(), this->_users.end(), user);
	if (it != this->_users.end())
        this->_users.erase(it);
}

void Channel::eraseOperator(Client* user)
{
	std::vector<Client*>::iterator it = std::find(this->_operators.begin(), this->_operators.end(), user);
	if (it != this->_operators.end())
        this->_operators.erase(it);
}

void Channel::sendToUsersNewUser(Client* sender)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender->getFd() != (*it)->getFd())
		{
			std::string cmd = ":" + sender->getNick() + "!" + sender->getUser() + "@localhost JOIN :#" + this->_name + "\r\n";
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::sendToUsersMessage(std::string message, Client* sender)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
	{
		if (sender->getFd() != (*it)->getFd())
		{
			std::string cmd = ":" + sender->getNick() + "!" + sender->getUser() + "@localhost PRIVMSG #" + this->_name + " :" + message + "\r\n";
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
		}
	}
}

void Channel::sendToUsersCommand(std::string cmd)
{
	std::vector<Client*>::iterator it;

	for(it = this->_users.begin(); it != this->_users.end(); it++)
			send((*it)->getFd(), cmd.c_str(), cmd.size(), 0);
}

bool Channel::isOperator(Client* user)
{
	return (std::find(this->_operators.begin(), this->_operators.end(), user) != this->_operators.end());
}

bool Channel::isOnChannel(Client* user)
{
	return (std::find(this->_users.begin(), this->_users.end(), user) != this->_users.end());
}

std::vector<Client*> Channel::getUsers()
{
	return (this->_users);
}

void Channel::setTopic(std::string topic)
{
	_topic = topic;
}

std::string Channel::getTopic()
{
	return _topic;
}

std::string Channel::getPassKey()
{
	return _passKey;
}

bool Channel::getInviteOnly()
{
	return _inviteOnly;
}

bool Channel::getTopicOpOnly()
{
	return _topicChangeOpOnly;
}

int Channel::getUserLimit()
{
	return _userLimit;
}

std::string Channel::getModes()
{
	return _modes;
}

std::vector<Client*>	Channel::getOperators()
{
	return _operators;
}

void Channel::setPassKey(const std::string &passKey)
{
	_passKey = passKey;
}

void Channel::setInviteOnly(const bool &inviteOnly)
{
	_inviteOnly = inviteOnly;
}

void Channel::setTopicChOnly(const bool &topicChangeChOnly)
{
	_topicChangeOpOnly = topicChangeChOnly;
}

void Channel::setUserLimit(int userLimit)
{
	_userLimit = userLimit;
}
bool Channel::isInvited(Client* user)
{
	return (std::find(this->_invited.begin(), this->_invited.end(), user) != this->_invited.end());
}
void Channel::setModes(bool changeType, char mode)
{
	if (changeType == ADD && _modes.find(mode) == std::string::npos)
		_modes += mode;
	else if (changeType == REMOVE && _modes.find(mode) != std::string::npos)
		_modes.erase(_modes.find(mode));
}

void Channel::printUsers(Client* client)
{
    std::string channel = this->_name;
    std::string userList = RPL_NAMREPLY + client->getNick() + " = #" + channel + " :";

    for (size_t i=0; i < this->getUsers().size(); i++)
    {
        if (this->isOperator(this->getUsers()[i]))
            userList += "@";
        userList += this->getUsers()[i]->getNick();
        if (this->getUsers()[i] != this->getUsers().back())
            userList += " ";
    }
    userList += "\r\n";
    userList += RPL_ENDOFNAMES + client->getNick() + " #" + channel + " :" + "End of /NAMES list.\r\n";
    send(client->getFd(), userList.c_str(), userList.size(), 0);
}

void Channel::printTopic(Client* client)
{
	std::string RPL;

	if (this->getTopic().empty())
		RPL = RPL_NOTOPIC + client->getNick() + " #" + this->_name+ " :No topic is set\r\n";
	else
		RPL = RPL_TOPIC + client->getNick() + " #" + this->_name + " :" + this->getTopic() + "\r\n";
	send(client->getFd(), RPL.c_str(), RPL.size(), 0);
}

