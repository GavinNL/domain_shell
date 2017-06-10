# Domain Shell #

A way to add shell type functionality to your project.

Communicate with your process using named Unix Socket.

You can connect to the shell using an external application such as "socat"

```bash
sudo apt-get install soccat
```

```bash
socat - UNIX-CONNECT:/socket_path
```

See the main.cpp example
