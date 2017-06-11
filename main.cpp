#define SOCKET_NAME "/tmp/mysocket.socket"

#include "domain_shell.h"

#include <iostream>

void cmd_exit(Unix_Socket & client, char const * args)
{
    const char msg[] = "From server: Good Bye!!\n";

    client.Write(msg, sizeof(msg));

    client.Close();
}

void cmd_none(Unix_Socket & client, char const * args)
{
    std::string response = "Server Received: ";
    response += args;
    client.Write(response.data(), response.size() );
}

void on_connect(Unix_Socket & client)
{
    std::cout << "Client connected" << std::endl;

    const char msg[] = "Welcome to the Shell!\n"
                       "- Any command you type will be echoed back.\n"
                       "- type 'exit' to disconnect\n";

    client.Write(msg, sizeof(msg));
}

void on_disconnect(Unix_Socket & client)
{
    std::cout << "Client Disconnected" << std::endl;


}

int main()
{

    DomainShell S;

    S.AddCommand("exit", cmd_exit);

    S.AddOnConnect( on_connect );
    S.AddDefaultCommand(cmd_none);

    S.AddOnDisconnect( on_disconnect );

    S.Start(SOCKET_NAME);

    std::cout << "Connect to the shell from your bash terminal using:" << std::endl << std::endl;
    std::cout << "socat - UNIX-CONNECT:" << SOCKET_NAME << std::endl << std::endl;
    std::cout << "   or " <<  std::endl << std::endl;
    std::cout << "netcat -U " << SOCKET_NAME << std::endl << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(10000));
    return 0;
}

