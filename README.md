# ft_irc

Using **sys/poll.h** header for the **poll()** function, we could handle at first the requests for I/O (Input/Output) interactions. Then when getting the full requests from the client with **recv()** we could then process the data to then **send()** the correct RPL for the client, taking as reference the **RFC 1459**.
Now do:
```sh
$ make
```
and then launch the server:
```sh
$ ./ircserv <port> <server-password>
```

Now get your client and have fun 🔥
