#include "SoundManager.h"
#include <string>
#include <iostream>

const std::string SUCCESS = "media/sounds/Success/success5.wav";
const std::string FAILURE = "media/sounds/Failure/failure4.wav";
const std::string BOUNCE = "media/sounds/Bounce/bounce1.wav";
const std::string BACKGROUND = "media/sounds/Background/background1.wav";

//sound effect ids
int successId; 	
int failureId; 	
int bounceId; 

std::vector<Mix_Chunk*> SoundList;
Mix_Music* backgroundMusic;


static bool mute = false;
bool init = false;
bool debug = false;

SoundManager::SoundManager() {
	if(debug){ std::cout<<"Entered SoundManager Constructor"<<std::endl;}
	
	init();
	
	if(debug){ std::cout<<"Exiting SoundManager Constructor"<<std::endl;}
}
 
SoundManager::~SoundManager() { 
	if(debug){std::cout<<"Entering SoundManager Destructor"<<std::endl;}
	
	Cleanup();
	Mix_CloseAudio();
	
	if(debug){std::cout<<"Exiting SoundManager Destructor"<<std::endl;}
}
 
void SoundManager::resetIDs(void){
	successId=-1; 	
	failureId=-1; 	
	bounceId=-1; 	
}

void SoundManager::init(void){

	int errorCode = Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096);
	if(errorCode != 0){
		std::cout<<"Error: Could not open SDL Mixer.\n"<<Mix_GetError()<<std::endl;
	}
	resetIDs();
	backgroundMusic=NULL;
	loadSounds();
	
}

void SoundManager::loadSounds(void){
	//load default sound effects
	if(debug){
		std::cout<<"Entered loadSounds()"<<std::endl;
		std::cout<<"success:"<<successId<<
			", failure:"<<failureId<<
			",bounce:"<<bounceId<<
			std::endl;
			
	}
	if(successId<0){
		successId = LoadChunk(SUCCESS.c_str());
	}
	if(failureId<0){
		failureId = LoadChunk(FAILURE.c_str());
	}
	if(bounceId<0){
		bounceId = LoadChunk(BOUNCE.c_str());
	}
	if(backgroundMusic==NULL){
		backgroundMusic=Mix_LoadMUS(BACKGROUND.c_str());
	}
	if(debug){
		std::cout<<"Exiting loadSounds()"<<std::endl;
		std::cout<<"success:"<<successId<<
			", failure:"<<failureId<<
			",bounce:"<<bounceId<<
			std::endl;
	}
}

Mix_Music* SoundManager::LoadMusic(const char* File){
	//File type can be wav, mod, midi, ogg, mp3, or flac
	if(debug){std::cout<<"Entered LoadMusic()"<<std::endl;}
	
	if(File==NULL){
		std::cout<<"Error in LoadMusic(), File==NULL"<<std::endl;
		return NULL;
	}	
		
	Mix_Music* musicPtr = Mix_LoadMUS(File);
	
	if(musicPtr==NULL){
		std::cout<<"Error in MixLoadMUS(), could not load file."<<std::endl;
	}
	
	if(debug){ std::cout<<"Exiting LoadMusic()"<<std::endl;}
	
	return musicPtr;
}

int SoundManager::LoadChunk(const char* File) {
	//File type can be wav, mod, midi, ogg, or mp3
	if(debug){ std::cout<<"Entered LoadChunk()"<<std::endl;}
    Mix_Chunk* TempSound = NULL;
 	if(File==NULL){
 		std::cout<<"Error: File=NULL."<<std::endl;
 		if(debug){ std::cout<<"Exiting LoadChunk()"<<std::endl;}
        return -1;
    }
    //Mix_LoadWav name is misleading, it also supports other filetypes
    if((TempSound = Mix_LoadWAV(File)) == NULL) {
    	std::cout<<"Error: "<<File<<" cound not be loaded."<<std::endl;
    	if(debug){ std::cout<<"Exiting LoadChunk()"<<std::endl;}
        return -1;
    }
 	if(debug){std::cout<<"SoundList.size() Before:"<<SoundList.size()<<std::endl;}
    SoundList.push_back(TempSound);
 	if(debug){std::cout<<"SoundList.size() After:"<<SoundList.size()<<std::endl;}
 	if(debug){ std::cout<<"Exiting LoadChunk()"<<std::endl;}
 	
 	//chunkID
    return (SoundList.size() - 1);
    
}
 
void SoundManager::Cleanup() {
    for(int i = 0;i < SoundList.size();i++) {
        Mix_FreeChunk(SoundList[i]);
    }
    SoundList.clear();
}
 
void SoundManager::fadeInLoopMusic(Mix_Music* music, int loops, int ms){
	//ms= milliseconds it takes to fade in
	if(music==NULL){
		std::cout<<"Error in LoopMusic(), *music=NULL."<<std::endl;
		return;
	}
	if(ms<0){
		std::cout<<"Error in LoopMusic(), ms < 0."<<std::endl;
		return;
	}
	
	int errorCode = Mix_FadeInMusic(music, loops, ms);
	if(errorCode!=0){
		std::cout<<"Error in Mix_FadeInMusic(): "<< Mix_GetError() << std::endl;
		return;
	}
}

void SoundManager::LoopMusic(Mix_Music* music, int loops){
	//loops==-1 plays music infinitely
	if(music==NULL){
		std::cout<<"Error in LoopMusic(), *music=NULL."<<std::endl;
		return;
	}	
	
	int errorCode = Mix_PlayMusic(music, loops);
	if(errorCode!=0){
		std::cout<<"Error in Mix_PlayMusic(): "<< Mix_GetError() << std::endl;
		return;
	}
}
 
void SoundManager::PlaySound(int chunkID) {
	
	if(debug){std::cout<<"Entered PlaySound("<<chunkID<<")"<<std::endl;}
    if(chunkID < 0){
    	std::cout<<"Invalid. ID < 0"<<std::endl;
    	return;
    }
    if(chunkID >= SoundList.size()){ 
    	std::cout<<"Invalid. ID >= SoundList.size() = "<<SoundList.size()<<std::endl;
    	return;
    }
    if(SoundList[chunkID] == NULL){
    	std::cout<<"Empty SoundList."<<std::endl;
    	return;
    }
    Mix_PlayChannel(-1, SoundList[chunkID], 0);
    if(debug){std::cout<<"Exiting PlaySound()"<<std::endl;}
}

void SoundManager::toggleMute(void){
	setMute(!mute);
}

void SoundManager::setMute(bool val){
	if(val==true){
		stopAllAudio();
	}
	if(val==false){
		playBackground(-1);
	}
	
	mute=val;
}

void SoundManager::playFailure(void){
	if(!mute){

		if(debug){std::cout<<"entered playFailure()"<<std::endl;}

		PlaySound(failureId);
		if(debug){std::cout<<"exiting playFailure()"<<std::endl;}
	}
}

void SoundManager::playSuccess(void){
	if(!mute){

		if(debug){std::cout<<"entered playSuccess("<<successId<<")"<<std::endl;}
		PlaySound(successId);
		if(debug){std::cout<<"Exiting playSuccess()"<<std::endl;}
	}
}
 
void SoundManager::playBounce(void){
	if(!mute){
		if(debug){
			std::cout<<"entered playBounce()"<<std::endl;
		}
		PlaySound(bounceId);
		
		if(debug){std::cout<<"exiting playBounce()"<<std::endl;}
	}
} 

void SoundManager::playBackground(int numLoops){
	playBackground(numLoops, 0);
}

void SoundManager::playBackground(int numLoops, int ms){
	if(!mute){
		if(debug){std::cout<<"entered playBackground()"<<std::endl;}
		if(backgroundMusic==NULL){
			backgroundMusic=Mix_LoadMUS(BACKGROUND.c_str());
		}
		fadeInLoopMusic(backgroundMusic, numLoops, ms);
		if(debug){std::cout<<"exiting playBackground()"<<std::endl;}
	}
} 
 
void SoundManager::pauseSounds(void){
	//can't pause background music, only play and stop.
	//pause sound effects
	Mix_Pause(-1);
}

void SoundManager::resumeSounds(void){
	//resume sound effects
	Mix_Resume(-1);
}

void SoundManager::stopMusic(void)
{  	//stop background music
	if(backgroundMusic!=NULL){
		Mix_FreeMusic(backgroundMusic);
	}
   	backgroundMusic=NULL; //backgroundMusic now points to garbage, so set it to null
} 

void SoundManager::stopSounds(void)
{  	//stop sound effects
	Mix_HaltChannel(-1); 
}

void SoundManager::stopAllAudio(void)
{  	
   	stopMusic();
   	stopSounds();
   	
}
