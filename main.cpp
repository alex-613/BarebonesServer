//for reference:
// https://www.youtube.com/watch?v=WDn-htpBlnU&t=3s
// https://www.youtube.com/watch?v=TE1XQ8U_JNA

#include <iostream>
#include <WS2tcpip.h> //This is winsoc, windows sockets is the frame work that windows uses to access sockets

#pragma comment (lib, "ws2_32.lib") //Effectively this saves the effort of going through the include libraries mess, this is the same as adding a library using the include libraries function under properties

using namespace std;

int main() {//error checking should be included
	//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2); //Basically a microsoft accepted way of making an unsigned int, in this case gives us version 2.2

	int wsOK = WSAStartup(ver, &wsData);  //Start Winsock, WSAStartup needs a version and a pointer pointing to the data.
	if (wsOK != 0) {

		cerr << "Can't Initialize winsock! Quitting" << endl;//This is a shortened version of cout, where the namespace is already defined as standard.
		return 0;
	}
	// create a socket (A socket is an endpoint, it is matched up with a port and an ip address tuple, in short it is just a reference number used to access this port on this ip address, but it hasn't been bounded)
	// (so I can use this socket to write to another component)
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); //In windows, the socket number is also a type, but it is not any different than a number.
	//AF_INET is an address family for IPv4, essentially a list of constant used to initiate a socket
	// We are opening a connection_oriented, TCP/IP socket so we use this, refer to the python internet guide

	//One more error check
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl;
		return 0;
	}
	

	//Bind the ip address and port to a socket, watch the clip. We are tring to create the serverside tuple (something like an array).
	sockaddr_in hint; //This is a structure we must fill in. A structrue in cpp is a class with public members.
	hint.sin_family = AF_INET; //Accessing an element in the hint structure
	hint.sin_port = htons(54000); // The socket that was used specifically for listening only! htons stands for host to newtwork short.  https://www.youtube.com/watch?v=NcaiHcBvDR4
	//Networking is big endian, and PC, or intel processor computers speficially are little endian, so we must use htons to convert that.
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint)); //bind the socket to the port(on any address, defined by the address family)
	//Now we have socket, binded to network codes, and we use the socket to reference the network codes so we can send things out and receive things.

	//Tell winsock the socket is for listening
	listen(listening, SOMAXCONN); // SOMAXCONN is maximum connection you can have, but it is a number in essence

	//Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	//We are listening for a connection, connection happens, and that happens on a particualr port on a server. The socket that actually returns to the clientSocket number received from the client will be another number
	
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize); //Now we have a valid socket
	
	//If we have a valid socket, lets print out some info. To do so we need data, from the object

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXSERV]; //Service (i.e. port) the client is connect on
	//Again, NI_MAXHOST and NI_MAXSERV are simple maximum numbers that sizes that these numbers can take, we do this to initialize our array

	ZeroMemory(host, NI_MAXHOST); // It is a define used by windows
	ZeroMemory(service, NI_MAXSERV);
	
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		cout << host << " connected on port " << service << endl;
	}
	//getnameinfo allows us to try to access the host name of the client that has connected to the server 
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " <<
			ntohs(client.sin_port) << endl;
	}
	//Close listening socket, since we are no longer listening if a connection is established
	closesocket(listening);

	//While loop: accept and echo message back to client, tells the client that the connection is good!
	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);

		//wait for client to send data
		int bytesReceived = recv(clientSocket, buf, 4096, 0); //receive function returns the number of bytes received
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0) {
			cout << "Client disconnected" << endl;
			break;
		}
		//echo message back to client
		send(clientSocket, buf, bytesReceived + 1, 0);
		
	}
	// close the socket
	closesocket(clientSocket);

	//Shutdown winsock
	WSACleanup();
	
}