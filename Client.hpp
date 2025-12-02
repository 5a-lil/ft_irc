#ifndef CLIENT_HPP
#define CLIENT_HPP

# include "Server.hpp"

//A REVOIR
class Channel;

class Client {
    private :

    int _fd;
    std::string _pass;
	std::string _nick;
	std::string _user;
	// std::string _hostname;
	std::string _buffer;
    bool _verif;
	bool _connected;
    std::vector<Channel*> _joinedChannels;

    public :

    Client(int fd);
	~Client();

    bool operator==(const Client& user);
	
	std::string& getBuffer();
	void appendToBuffer(const std::string& data);
    
    //getter
    std::string getPass() const;
    std::string getNick() const;
    std::string getUser() const;

	int getFd() const;
    bool getVerif() const;
    bool getConnected() const;
    //setter
    void setPass(const std::string& pass);
    void setUser(const std::string& user);
    void setNick(const std::string& nick);
    void setBufferEmpty();
    
    // void setJoin(const std::string& join);
    void setVerif(bool value);
    void setConnected(bool value);
    void joinChannel(Channel* channel);

    void addBuffer(std::string cmdslice);
    std::string getBuffer() const;
    
    std::vector<Channel *> getJoinedChannels();
    
};

#endif