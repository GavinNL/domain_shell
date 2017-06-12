/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

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

    S.Disconnect();
    return 0;
}

