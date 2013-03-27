#ifndef _SoundManager_H_
#define _SoundManager_H_
 
#include <SDL/SDL.h>
//#include <SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
 
class SoundManager {
    public:   
        SoundManager();
        virtual ~SoundManager();
        
        std::vector<Mix_Chunk*> SoundList;
        
        Mix_Music* LoadMusic(const char* File);
        int LoadChunk(const char* File);
        void fadeInLoopMusic(Mix_Music* music, int loops, int ms);
        void LoopMusic(Mix_Music* music, int loops);
        void PlaySound(int chunkID);
        void toggleMute(void);
        void setMute(bool val);
        void playFailure(void);
      	void playSuccess(void);
     	void playBounce(void);
      	void playBackground(int numOfLoops);
      	void playBackground(int numOfLoops, int ms);
      	void pauseSounds(void);
      	void resumeSounds(void);
      	void stopMusic(void);
      	void stopSounds(void);
        void stopAllAudio(void);
        
	private:
		void resetIDs(void);
		void init(void);
		void Cleanup();
		void loadSounds(void);
};
 
#endif
