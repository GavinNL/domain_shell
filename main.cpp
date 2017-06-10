#define SOCKET_NAME "/tmp/mysocket.socket"

#include "domain_shell.h"

void cmd_ls(Unix_Socket & client, char const * args)
{
    std::cout << "ls command received" << std::endl;
}

void cmd_exit(Unix_Socket & client, char const * args)
{
    client.Close();
}

int main()
{

    DomainShell S;
    S.AddCommand("ls", cmd_ls);
    S.AddCommand("exit", cmd_exit);

    S.Start(SOCKET_NAME);

    std::cout << "Connect to the shell using" << std::endl << std::endl;
    std::cout << "socat - UNIX-CONNECT:" << SOCKET_NAME << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10000));
    return 0;
}

