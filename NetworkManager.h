#ifndef _NetworkManager_H_
#define _NetworkManager_H_

#include <SDL/SDL_net.h>
#include <gameUpdate.h>



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
