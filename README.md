# Domain Shell #

Domain Shell is a simple library which allows you to add a shell to your
application. The shell can be communicated with using an external application
such as  ncat or socat.

The library provides multi-client shell so multiple clients can connect to it
at once.

Domain Shell uses a Unix Domain socket to provide communication to the running
process.

Once the shell is running, you can connect to it using an external application
like socat or netcat.


```bash
sudo apt-get install soccat
```

```bash
sudo apt-get install netcat
```

Connect to the socket using the command line:

```bash
socat - UNIX-CONNECT:/socket_path
```


```bash
netcat -U /socket_path
```

## Example ##

See the [main.cpp](main.cpp) for a quick start on how to use Domain Shell


## Task List ##

- [ ] - Add TCP sockets as well.



