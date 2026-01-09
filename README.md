# IRC_Protocol_Server

Using **sys/poll.h** header for the **poll()** function, we could handle at first the requests for I/O (Input/Output) interactions. Then when getting the full requests from the client with **recv()** we could then process the data to then **send()** the correct RPL for the client, taking as reference the **RFC 1459**. **Irssi** client was took as reference for all features handled.

Now do:
```
$ make
```
and then launch the server:
```
$ ./ircserv <port> <server-password>
```
Handles from the server:
- Connection to server without any errors
- Communication between client and server using TCP/IP (v4)
- Fully handled authentification feature and private message system
- Channel management 
- Operators and regular users managed (creation, destruction, differences, channel rights etc...)
- KICK command
- INVITE command
- TOPIC command
- MODE command (i, t, k, o and l modes)

Now get your client and have fun ! 🔥
