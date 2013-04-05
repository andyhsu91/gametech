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
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//modified from http://content.gpwiki.org/index.php/SDL:Tutorial:Using_SDL_net#Server_side
//	and http://r3dux.org/2011/01/a-simple-sdl_net-chat-server-client/
//  and http://stackoverflow.com/questions/5362730/finding-out-udp-broadcast-source-ip-with-sdl-net


//constants
const unsigned int MAX_SOCKETS = 2;
const unsigned int BUFFER_SIZE = sizeof(gameUpdate);
const unsigned int PORT_NUM = 57996; //36066 in network byte order
const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1;
const int serverSearchTimeout = 5; //number of seconds to search for server
bool NM_debug = true;

//local variables
UDPsocket UdpSocket;		//socket for broadcasting and recieving UDP packets so that clients can find a server on the local network.
SDLNet_SocketSet socketSet;	//socketSet for checking for new packets
TCPsocket serverSocket; 	//server only socket where clients can request connections
TCPsocket peerSocket; 		//socket for communicating once connection is established
IPaddress myIp; 			//this computer's ip address
IPaddress *remoteIP; 		//other computer's ip address

char buffer[BUFFER_SIZE];	//buffer for gameUpdates to be copied into when packets are recieved
char* conversion;


NetworkManager::NetworkManager() {
	connectionOpen=false;
	
	if(NM_debug){std::cout<<"Entered Network Manager()"<<std::endl;}
	//SDL_Init(SDL_INIT_EVERYTHING);
	
	
	//check for server, and attempt to become client
	bool serverFound = checkForServer();

	
	//if no server found, become a server, and check for clients
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
		
		
		serverSocket = SDLNet_TCP_Open(&myIp); //open a socket to listen for clients
	
		myIp.host = getMyIp();
		
		
		if(serverSocket == NULL){
			std::cout<<"Error: could not create server socket."<<std::endl;
			return;
		}
		
		std::cout<<"Server has opened socket with IP: "<< intToIpAddr(myIp.host, true)<<" and Port: "<<SDLNet_Read16(&myIp.port)<<std::endl;
		//add serverSocket to socketSet
		//SDLNet_TCP_AddSocket(socketSet, serverSocket);
		isServer=true;
		
		broadcastToClients();
		usleep(1000);
		checkForClient();
	}

	
	
	if(NM_debug){std::cout<<"Exiting Network Manager()"<<std::endl;}
}

bool NetworkManager::isConnectionOpen(){
	return connectionOpen;
}

bool NetworkManager::isThisServer(){
	return isServer;
}

int NetworkManager::getMyIp(){

	//this is ghetto, shouldn't be doing this, but SDL_net does not provide this functionality for some reason.
	
	//write this computer's Ip address to myIp.txt
	int retVal = system("/sbin/ifconfig eth0 | grep 'inet addr:' | cut -d: -f2 | awk '{ print $1}' > myIp.txt");
	
	//read myIp.txt to get it
	std::string line;
  	std::ifstream myfile;
  	usleep(5000); //wait 5 milliseconds
  	myfile.open("myIp.txt");
  	
  	bool gotString = false;
  	
	if (myfile.is_open() && myfile.good()){
		std::getline(myfile,line);
		myfile.close();
		gotString = true;
	}
	
	if(gotString){
		
		int retVal = inet_addr(line.c_str());
		if(NM_debug){std::cout<<"ifconfig says myIp="<<line<<", converted to "<<retVal<<std::endl;}
		return retVal;
	}
	else{
		if(NM_debug){std::cout<<"Could not read file."<<line<<std::endl;}
		return -1;
	}
}

char*  NetworkManager::intToIpAddr(long ipAddress, bool networkByteOrder){
	//if(NM_debug){std::cout<<"Entered intToIpAddr()"<<std::endl;}
	char* ipAddr = new char[16]; //allocate it on heap
    if(!networkByteOrder){
    	snprintf(ipAddr,16,"%lu.%lu.%lu.%lu",(ipAddress & 0xff000000) >> 24 
                                                ,(ipAddress & 0x00ff0000) >> 16
                                                ,(ipAddress & 0x0000ff00) >> 8
                                                ,(ipAddress & 0x000000ff));
    }
    else{
    	snprintf(ipAddr,16,"%lu.%lu.%lu.%lu", (ipAddress & 0x000000ff)
                                                ,(ipAddress & 0x0000ff00) >> 8
                                                ,(ipAddress & 0x00ff0000) >> 16
                                                ,(ipAddress & 0xff000000) >> 24);
    }
  
	std::cout<<"Converted "<<ipAddress<<" to "<<ipAddr<<std::endl;
	
	if(conversion){delete conversion;}//delete possible previous conversions
	conversion = ipAddr;
	//if(NM_debug){std::cout<<"Exiting intToIpAddr()"<<std::endl;}
	return ipAddr;
}

bool NetworkManager::checkForServer(){

	if(NM_debug){std::cout<<"Entered checkForServer()"<<std::endl;}
	
	/* Listen for a broadcasted a UDP packet to all devices on the local network.
	   If a UDP packet is received then the other computer is the server.
	*/
	
	if(UdpSocket==NULL){	
		UdpSocket = SDLNet_UDP_Open(PORT_NUM);
	}
	
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(IPaddress));
	
	
	IPaddress packetData;
	
	if(UdpSocket == NULL){
		std::cout<<"Error: could not create UDP socket"<<std::endl;
		return false;
	}
	
	
	int count = 0;
	bool serverFound=false;
	
	if(NM_debug){std::cout<<"Listening for server broadcast packets..."<<std::endl;}
	//check for server broadcast packet
	while(count<(serverSearchTimeout*1000) && !serverFound){
		int errorCode = SDLNet_UDP_Recv(UdpSocket, packet);
		
		if(errorCode == 1){
			//successfully recieved UDP packet, copy packet data to local packet data
			memcpy(&packetData, packet->data, sizeof(IPaddress));
			if(NM_debug){std::cout<<"Packet recieved."<<std::endl;}
			if(NM_debug){std::cout<<"packetData.host="<<intToIpAddr(packetData.host, true)<<", packetData.port="<<packetData.port<<std::endl;}
			if(NM_debug){std::cout<<"packet.host="<<intToIpAddr(packet->address.host, true)<<", packet.port="<<packet->address.port<<std::endl;}
			packetData.host = packet->address.host;
			packetData.port = packet->address.port;
			if(packetData.port == PORT_NUM || packet->address.port == PORT_NUM){
				if(NM_debug){std::cout<<"Packet is server packet."<<std::endl;}
				serverFound=true;
			} else{
				if(NM_debug){std::cout<<"Packet is garbage."<<std::endl;}
			}
		}
		if(!serverFound){
			count++;
			usleep(1000); //sleep for 1000 microseconds = 1 millisecond
		}
	}
	
	if(serverFound){
		//received UDP packet from server, so make connection as a client
		if(NM_debug){std::cout<<"creating socket set."<<std::endl;}
		socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
		//if(NM_debug){std::cout<<"convert uint32 to char*"<<std::endl;}
		
		intToIpAddr(packet->address.host, true);
		//char* serverIP = conversion;
		if(conversion == NULL){
			std::cout<<"Error: conversion error"<<std::endl;
		}else{
			
			std::cout<<"serverIP:"<<conversion<<std::endl;
		}
		//if(NM_debug){std::cout<<"resolving serverIP"<<std::endl;}
		//snprintf(serverIP, sizeof(serverIP), "%lu", (unsigned long)packet->address.host); //convert uint32 address.host to char*
		char* ipAddr=intToIpAddr(packet->address.host, true);
		//std::cout<<"result of intToIpAddr:"<<ipAddr<<std::endl;
		
		
		
		//this damn line keeps causing segfaults...
		//int errorCode = SDLNet_ResolveHost(remoteIP, ipAddr, PORT_NUM);
		
		
		
		int errorCode = 0;
		
		
		if(errorCode != 0){ 
			//failure
			std::cout<<"Error: could not resolve host."<<std::endl;
			return false;
		}
		if(NM_debug){std::cout<<"opening TCP connection with server"<<std::endl;}
		peerSocket = SDLNet_TCP_Open(&packetData); //open TCP connection with the server
	
		if(peerSocket == NULL){
			std::cout<<"Error: could not open connection with server"<<std::endl;
			return false;
		}
	
		//add peerSocket to socketSet
		if(NM_debug){std::cout<<"adding peerSocket to socketSet"<<std::endl;}
		SDLNet_TCP_AddSocket(socketSet, peerSocket);
		if(NM_debug){std::cout<<"Opening Connection. Connecting as client."<<std::endl;}
		connectionOpen=true;
		isServer=false;
		std::cout<<"Exiting checkForServer(). Returning true."<<std::endl;
		return true;
	}
	else{
		if(NM_debug){std::cout<<"No server broadcast packet recieved."<<std::endl;}
	}
	
	if(NM_debug){std::cout<<"Exiting checkForServer()."<<std::endl;}
	return false;
	
}

void NetworkManager::waitForClientConnection(){
	int millisecondsWaited = 0;
	if(NM_debug){std::cout<<"Entered waitForClientConnection()"<<std::endl;}
	while(!connectionOpen && millisecondsWaited<=15000){
		broadcastToClients();
		usleep(2000); //wait for little bit to give clients time to respond
		checkForClient();
		usleep(498000); //wait some more so that we don't congest the network 
		millisecondsWaited+=500;
	}
	if(NM_debug){std::cout<<"Exiting waitForClientConnection()"<<std::endl;}
}

void NetworkManager::broadcastToClients(){
	//server is broadcasting to all connected devices on local network to PORT_NUM
	//clients should listen for the packet and respond by requesting a TCP connection
	//if(NM_debug){std::cout<<"Entering broadcastToClients()."<<std::endl;}
	UDPpacket* packet = SDLNet_AllocPacket(sizeof(IPaddress));
	UDPsocket UdpSocket = SDLNet_UDP_Open(0);

	IPaddress addr;
	SDLNet_ResolveHost(&addr, "255.255.255.255", PORT_NUM); //sending to 255.255.255.255 broadcasts the packet on the local network

	packet->address.host = addr.host;
	packet->address.port = addr.port;
	packet->len = sizeof(myIp);
	memcpy(packet->data, &myIp, sizeof(myIp));
	if(NM_debug){std::cout<<"Broadcasting to clients..."<<std::endl;}
	SDLNet_UDP_Send(UdpSocket, -1, packet); //broadcast packet
	
	//if(NM_debug){std::cout<<"Exiting broadcastToClients()."<<std::endl;}
}

void NetworkManager::checkForClient(){
	//if(NM_debug){std::cout<<"Entering checkForClients()."<<std::endl;}
	//check to see if any client has requested a connection with server
	peerSocket = SDLNet_TCP_Accept(serverSocket);
	if(peerSocket==NULL){
		//no client requested connection
		//printf("SDLNet_TCP_Accept: %s. No client found.\n", SDLNet_GetError());
		if(NM_debug){std::cout<<"No client response..."<<std::endl;}
	}
	else{
		/* Now we can communicate with the client using peerSocket
		* serverSocket will remain opened waiting other connections */
	
		/* Get the remote address */
		if ((remoteIP = SDLNet_TCP_GetPeerAddress(peerSocket))){
			/* Print the address, converting in the host format */
			printf("Host connected: %x %d\n", SDLNet_Read32(&remoteIP->host), SDLNet_Read16(&remoteIP->port));
		}
		else{
			fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());
			
		}
		
		SDLNet_TCP_AddSocket(socketSet, peerSocket); //add peerSocket to socketSet
		if(NM_debug){std::cout<<"Client Found. Opening Connection."<<std::endl;}
		isServer=true;
		connectionOpen = true;
	
	}
	
	//if(NM_debug){std::cout<<"Exiting checkForClients()."<<std::endl;}
}

bool NetworkManager::checkForPackets(){
	if(NM_debug){std::cout<<"Entering checkForPackets()."<<std::endl;}
	bool retVal=false;
	if(connectionOpen){
		//check for activity with 0 millisecond timeout
		int socketsWithActivity = SDLNet_CheckSockets(socketSet, 0); //refresh activity count of all sockets in set
		int peerIsReady = SDLNet_SocketReady(peerSocket); //returns non-zero if server is ready
		
		if(socketsWithActivity > 0 && peerIsReady != 0){
			readPacketToBuffer();
			socketsWithActivity--;
			retVal = true;
		}
		if(socketsWithActivity>0){
			std::cout<<"Error: more than 1 socket with activity."<<std::endl;	
		}
	}
	if(NM_debug){std::cout<<"Exiting checkForClients()."<<std::endl;}
	return retVal;
}


bool NetworkManager::sendPacket(gameUpdate update){	
	if(NM_debug){std::cout<<"Entering sendPacket()."<<std::endl;}

	char* byteArray = static_cast<char*>(static_cast<void*>(&update)); //cast gameUpdate to byteArray
	
	int numBytesSent = SDLNet_TCP_Send(peerSocket, byteArray, sizeof(update));
	
	if(numBytesSent == sizeof(update)){
		if(NM_debug){std::cout<<"Exiting sendPacket()."<<std::endl;}
		return true;
	}
	std::cout<<"Failed to send message: " << SDLNet_GetError() << std::endl;
	return false;
}

gameUpdate* NetworkManager::getGameUpdate(){
	gameUpdate* latestUpdate = (gameUpdate*) buffer; 
	return latestUpdate;
}


void NetworkManager::readPacketToBuffer(){
	if(NM_debug){std::cout<<"Entering readPacketToBuffer()"<<std::endl;}
	//blocking call, only call readPacket if you *know* a packet has been received
	int numBytesReceived = SDLNet_TCP_Recv(peerSocket, buffer, sizeof(buffer)-1); 

	if(numBytesReceived == -1){
		//error
		std::cout<< SDLNet_GetError() <<std::endl;
		std::cout<<"Exiting readPacket()."<<std::endl;
		return;
	}
	else if(numBytesReceived == 0){
		//connection was closed by peer
		SDLNet_TCP_DelSocket(socketSet, peerSocket);
		SDLNet_TCP_Close(peerSocket);
		peerSocket=NULL;
		connectionOpen = false;	
		if(NM_debug){std::cout<<"Connection closed by peer."<<std::endl;}
		std::cout<<"Exiting readPacket()."<<std::endl;
		return;
	}
	if(NM_debug){std::cout<<"Exiting readPacket()."<<std::endl;}
}

NetworkManager::~NetworkManager() {
	if(conversion){delete conversion;}
	if(peerSocket != NULL){
		SDLNet_TCP_DelSocket(socketSet, peerSocket);
		SDLNet_TCP_Close(peerSocket);
	}
	if(serverSocket != NULL){
		SDLNet_TCP_DelSocket(socketSet, serverSocket);
		SDLNet_TCP_Close(serverSocket);
	}
	SDLNet_Quit();
	//SDL_Quit();
	
}



