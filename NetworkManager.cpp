#include "NetworkManager.h"
#include <iostream>
//#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
//#include <pthread.h>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <GL/gl.h>
//#include <GL/glu.h>

//modified from http://content.gpwiki.org/index.php/SDL:Tutorial:Using_SDL_net#Server_side
//	and http://r3dux.org/2011/01/a-simple-sdl_net-chat-server-client/
//  and http://stackoverflow.com/questions/5362730/finding-out-udp-broadcast-source-ip-with-sdl-net


//constants
const unsigned int MAX_SOCKETS = 2;
const unsigned int BUFFER_SIZE = sizeof(gameUpdate);
const unsigned int PORT_NUM = 57996; //chosen randomly from range 49,152 to 65,535
const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1;
const int serverSearchTimeout = 1;	//search for server for 2 seconds

//local variables
UDPsocket UdpSocket;	//socket for broadcasting and recieving UDP packets so that clients can find a server on the local network.
SDLNet_SocketSet socketSet;
TCPsocket serverSocket; //server only socket where clients can request connections
TCPsocket clientSocket; //socket for communicating once connection is established
IPaddress myIp; 		//this computer's ip address
IPaddress *remoteIP; 	//other computer's ip address

char buffer[BUFFER_SIZE];
bool connectionOpen;
bool isServer;



NetworkManager::NetworkManager() {
	std::cout<<"Entered Network Manager()"<<std::endl;
	//SDL_Init(SDL_INIT_EVERYTHING);
	//check for server
	//bool serverFound = checkForServer();
	bool serverFound = false;
	//if no server found, become one
	if(!serverFound){
		if(SDLNet_Init() < 0){
			std::cout<<"Error: could not initialize SDLNet"<<std::endl;
			return;
		}
	
		socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
	
		if(SDLNet_ResolveHost(&myIp, NULL, PORT_NUM) < 0){
			std::cout<<"Error: could not resolve host."<<std::endl;
			return;
		}
	
		serverSocket = SDLNet_TCP_Open(&myIp);
	
		if(serverSocket == NULL){
			std::cout<<"Error: could not create server socket."<<std::endl;
			return;
		}
	
		//add serverSocket to socketSet
		SDLNet_TCP_AddSocket(socketSet, serverSocket);
		
		broadcastToClients();
	}
	
	//otherwise, become client
	
	
	std::cout<<"Exiting Network Manager()"<<std::endl;
}

bool NetworkManager::checkForServer(){

	std::cout<<"Entered checkForServer()"<<std::endl;
	
	/* Listen for a broadcasted a UDP packet to all devices on the local network.
	   If a UDP packet is received then the other computer is the server.
	*/
	
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(IPaddress));
	UDPsocket UdpSocket = SDLNet_UDP_Open(PORT_NUM);
	IPaddress packetData;
	
	if(UdpSocket == NULL){
		std::cout<<"Error: could not create UDP socket"<<std::endl;
		return false;
	}
	
	
	int count = 0;
	bool serverFound=false;
	
	//check for server broadcast packet
	while(count<(serverSearchTimeout*1000) && !serverFound){
		int errorCode = SDLNet_UDP_Recv(UdpSocket, packet);
		
		if(errorCode == 1){
			//success, copy UDP packet data to local packet data
			memcpy(&packetData, packet->data, sizeof(IPaddress));
			serverFound=true;
		}
		else{
			count++;
			usleep(1000); //sleep for 1000 microseconds = 1 millisecond
		}
	}
	
	if(serverFound){
		//received UDP packet from server, so make connection as a client
		socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
		char serverIP[16];
		
		snprintf(serverIP, sizeof(serverIP), "%lu", (unsigned long)&packet->address.host); //convert uint32 address.host to char*
		if(SDLNet_ResolveHost(remoteIP, serverIP, PORT_NUM) < 0){ //get ip address of server
			std::cout<<"Error: could not resolve host."<<std::endl;
			return false;
		}
	
		clientSocket = SDLNet_TCP_Open(remoteIP); //open TCP connection with the server
	
		if(clientSocket == NULL){
			std::cout<<"Error: could not create server socket."<<std::endl;
			return false;
		}
	
		//add serverSocket to socketSet
		SDLNet_TCP_AddSocket(socketSet, clientSocket);
		connectionOpen=true;
		isServer=false;
		std::cout<<"Exiting checkForServer(). Returning true."<<std::endl;
		return true;
	}
	
	std::cout<<"Exiting checkForServer(). Returning false."<<std::endl;
	return false;
	
}

void NetworkManager::broadcastToClients(){
	//server is broadcasting to all connected devices on local network
	//clients should recieve the packet and respond by requesting a connection
	std::cout<<"Entering broadcastToClients()."<<std::endl;
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(IPaddress));
	UDPsocket UdpSocket = SDLNet_UDP_Open(0);

	IPaddress addr;
	SDLNet_ResolveHost(&addr, "255.255.255.255", PORT_NUM);

	packet->address.host = addr.host;
	packet->address.port = addr.port;
	packet->len = sizeof(myIp);
	memcpy(packet->data, &myIp, sizeof(myIp));

	SDLNet_UDP_Send(UdpSocket, -1, packet);
	
	std::cout<<"Exiting broadcastToClients()."<<std::endl;
}

void NetworkManager::checkForClient(){
	std::cout<<"Entering checkForClients()."<<std::endl;
	//check to see if any client has requested a connection with server
	if(clientSocket = SDLNet_TCP_Accept(serverSocket)){
		
		/* Now we can communicate with the client using clientSocket
		* serverSocket will remain opened waiting other connections */
	
		/* Get the remote address */
		if ((remoteIP = SDLNet_TCP_GetPeerAddress(clientSocket))){
			/* Print the address, converting in the host format */
			printf("Host connected: %x %d\n", SDLNet_Read32(&remoteIP->host), SDLNet_Read16(&remoteIP->port));
		}
		else{
			fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
			
		}
		
		SDLNet_TCP_AddSocket(socketSet, clientSocket); //add clientSocket to socketSet
		isServer=true;
		connectionOpen = true;
	
	}
	
	std::cout<<"Exiting checkForClients()."<<std::endl;		
}

bool NetworkManager::checkForPackets(){
	std::cout<<"Entering checkForPackets()."<<std::endl;
	bool retVal=false;
	if(connectionOpen){
		//check for activity with 0 millisecond timeout
		int socketsWithActivity = SDLNet_CheckSockets(socketSet, 0); 
		int clientIsReady = SDLNet_SocketReady(clientSocket);
		int serverIsReady = SDLNet_SocketReady(serverSocket);
		
		if(socketsWithActivity > 0 && clientIsReady != 0 && isServer){
			readPacketToBuffer(clientSocket);
			retVal = true;
		}
		if(socketsWithActivity > 0 && serverIsReady != 0 && !isServer){
			readPacketToBuffer(serverSocket);
			retVal = true;
		}
		retVal = false;
	}
	std::cout<<"Exiting checkForClients()."<<std::endl;
	return retVal;
}


bool NetworkManager::sendPacket(gameUpdate update){	
	std::cout<<"Entering sendPacket()."<<std::endl;

	char* byteArray = static_cast<char*>(static_cast<void*>(&update));
	
	int numBytesSent = SDLNet_TCP_Send(clientSocket, byteArray, sizeof(update));
	
	if(numBytesSent == sizeof(update)){
		std::cout<<"Exiting sendPacket()."<<std::endl;
		return true;
	}
	std::cout<<"Failed to send message: " << SDLNet_GetError() << std::endl;
	return false;
}

gameUpdate* NetworkManager::getGameUpdate(){
	gameUpdate* latestUpdate = (gameUpdate*) buffer; 
	return latestUpdate;
}


void NetworkManager::readPacketToBuffer(TCPsocket socket){
	std::cout<<"Entering readPacket()."<<std::endl;
	//blocking call, only call readPacket if you *know* a packet has been received
	int numBytesReceived = SDLNet_TCP_Recv(socket, buffer, sizeof(buffer)-1); 

	if(numBytesReceived == -1){
		//error
		std::cout<< SDLNet_GetError() <<std::endl;
		std::cout<<"Exiting readPacket()."<<std::endl;
		return;
	}
	else if(numBytesReceived == 0){
		//connection was closed by peer
		SDLNet_TCP_DelSocket(socketSet, socket);
		SDLNet_TCP_Close(socket);
		clientSocket=NULL;
		connectionOpen = false;	
		std::cout<<"Exiting readPacket()."<<std::endl;
		return;
	}
	std::cout<<"Exiting readPacket()."<<std::endl;
}

NetworkManager::~NetworkManager() {
	if(clientSocket != NULL){
		SDLNet_TCP_DelSocket(socketSet, clientSocket);
		SDLNet_TCP_Close(clientSocket);
	}
	if(serverSocket != NULL){
		SDLNet_TCP_DelSocket(socketSet, serverSocket);
		SDLNet_TCP_Close(serverSocket);
	}
	SDLNet_Quit();
	//SDL_Quit();
	
}



