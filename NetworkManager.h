#ifndef _NetworkManager_H_
#define _NetworkManager_H_

#include <SDL/SDL_net.h>
#include <gameUpdate.h>



class NetworkManager {
    
    public:   
		NetworkManager();
		virtual ~NetworkManager();
		void checkForClient();
		bool checkForServer();
		bool checkForPackets();
		bool sendPacket(gameUpdate update);
		void broadcastToClients();
		gameUpdate* getGameUpdate();
		bool isServer();
		bool connectionOpen();
	
	private:
		void readPacketToBuffer();
		bool connectionOpen;
		bool isServer;
		
};

#endif 
