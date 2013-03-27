#ifndef _NetworkManager_H_
#define _NetworkManager_H_

#include <SDL/SDL_net.h>



struct gameUpdate
{	
	//must be smaller than 512 bytes
	float ballPos[3];
	float ballVel[3];
	float paddlePos[3];
	bool  paddleDir[4];

};


class NetworkManager {
    
    public:   
		NetworkManager(bool isClient);
		virtual ~NetworkManager();
		void checkForClient();
		bool checkForServer();
		bool checkForPackets();
		bool sendPacket(TCPsocket socket, gameUpdate update);
		void broadcastToClients(IPaddress data);
	
	
	private:
		void readPacket(TCPsocket socket);
};

#endif 
