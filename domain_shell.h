#pragma once
#ifndef _SIMPLE_SHELL_H_
#define _SIMPLE_SHELL_H_

#if defined __linux__

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <vector>
#include <map>
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <mutex>

class Unix_Socket
{
public:

  bool Create()
  {
    m_fd = __create(AF_UNIX, SOCK_STREAM, 0);//socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (m_fd == -1)
    {
      perror("socket");
      exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  int __create(int __domain, int __type, int __protocol)
  {
    return ::socket(__domain, __type, __protocol);
  }

  bool Bind(const char * name)
  {
    //============
    memset(&m_name, 0, sizeof(struct sockaddr_un) );

    /* Bind socket to socket name. */

    m_name.sun_family = AF_UNIX;
    strcpy(m_name.sun_path, name);

    int ret = ::bind(m_fd, (const struct sockaddr *) &m_name,
                   sizeof(struct sockaddr_un));

    if (ret == -1)
    {
      printf("Error no: %d\n", errno );
      perror("bind");
      exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  bool Listen()
  {
    int ret = listen(m_fd, 20);
    if (ret == -1) {
      perror("listen");
      //exit(EXIT_FAILURE);
      return false;
    }
    return true;
  }

  Unix_Socket Accept()
  {
    Unix_Socket c;
    c.m_fd = ::accept(m_fd, NULL, NULL);
    if (c.m_fd == -1 || c.m_fd == 0)
    {
      c.m_fd = -1;
      //perror("accept");
      //exit(EXIT_FAILURE);
      return c;
    }
    return c;
  }

  size_t Read(void * buffer, size_t size )
  {
    //ssize_t ret = read(m_fd, buffer, size);
    ssize_t ret = recv(m_fd, buffer, size,  0);
    if(ret == -1 || ret == 0)
    {
      m_fd = -1;
    }
    return ret;
  }

  size_t Write( void const * buffer , size_t size)
  {
    return send(m_fd, buffer, size, 0);
    //return write(m_fd, buffer, size);
  }

  bool Shutdown()
  {
      if(m_fd != -1)
      {
        ::shutdown(m_fd, SHUT_RDWR);
      }
      return true;
  }

  bool Close()
  {
    if(m_fd == -1) return false;

    auto ret = close(m_fd);
    if( ret == -1)
    {
        //std::cout << "Error closing socket" << std::endl;
    }
    m_fd = -1;
    return true;
  }

  operator bool()
  {
      return m_fd != -1;
  }

private:
  struct sockaddr_un m_name;
  int                m_fd = -1;
};

class DomainShell
{
public:
    Unix_Socket m_Socket;
    using function_t = std::function<void(Unix_Socket&,char const * args)>;
    using map_t      = std::map<std::string, function_t >;

    DomainShell()
    {

    }

    ~DomainShell()
    {
        //std::cout << "Destroying" << std::endl;
        __disconnect();
        Unlink();
    }

    void Unlink()
    {
        unlink(m_Name.c_str());
    }

    void Start( char const * name)
    {
        m_Name = name;
        unlink(m_Name.c_str());
        m_Socket.Create();
        m_Socket.Bind(m_Name.c_str());


        std::thread t1(&DomainShell::__listen, this);
        m_ListenThread = std::move(t1);
    }


    void AddCommand(std::string const & s, function_t f)
    {
        m_cmds[s] = f;
    }


private:
    void parse(const char * buffer, Unix_Socket & client)
    {
        char cmd[50];
        std::sscanf(buffer,"%s",cmd);

        std::string command(cmd);

        auto it = m_cmds.find(cmd);
        if(it!=m_cmds.end())
        {
            int i=0;
            while(buffer[i] == ' ')
                ++i;
            it->second(client, &buffer[i]);
        }
    }


    void __client(Unix_Socket * p_Client)
    {
        auto & Client = *p_Client;
        //std::cout << "Client Connected" << std::endl;
        char buffer[256];

        while( Client && !m_exit)
        {
            auto ret = Client.Read(buffer, 255);
            if(ret == -1)
            {
                break;
            }

            buffer[ret] = 0;
            parse(buffer, Client);
           // Client.Write(buffer, ret);
        }
        Client.Close();
        __erase_client(p_Client);
        //std::cout << "Client disconnected" << std::endl;
    }

    void __erase_client(Unix_Socket * p)
    {
        m_Mutex.lock();
        for(int i=0; i < m_Clients.size();i++)
        {
            if(&m_Clients[i]->second == p)
            {
                std::swap( m_Clients[i] , m_Clients.back() );
                m_Clients.pop_back();
            }
        }
        m_Mutex.unlock();
    }

    void __listen()
    {
        m_Socket.Listen();

        while(!m_exit)
        {
          auto Client = m_Socket.Accept();

          if(!Client)
          {
              //std::cout << "Client is bad" << std::endl;
              continue;
          }
          int result = 0;

          pair_t * client = new pair_t();

          client->second = std::move(Client);

          std::thread tc( &DomainShell::__client, this, &client->second);

          client->first  = std::move(tc);

          m_Mutex.lock();
          m_Clients.push_back( client );
          m_Mutex.unlock();
        }

        //std::cout << "Listen thread exited" << std::endl;

    }


    void __disconnect()
    {
        m_exit = true;

        int i=0;

        for(auto & c : m_Clients)
        {
            //std::cout << "Disconnecting client " << i++ << std::endl;
            c->second.Close();
            if(c->first.joinable())
                c->first.join();

            delete(c);
        }

        //std::cout << "Closing Listen socket" << std::endl;
        m_Socket.Shutdown();

        if(m_ListenThread.joinable() )
            m_ListenThread.join();
    }


    using pair_t = std::pair< std::thread, Unix_Socket >;

    std::thread m_ListenThread;
    std::string m_Name;
    bool m_exit = false;
    std::mutex m_Mutex;
    std::vector<pair_t*> m_Clients;
    map_t m_cmds;
};
#else
    #error User a proper operating system!
#endif

#endif
