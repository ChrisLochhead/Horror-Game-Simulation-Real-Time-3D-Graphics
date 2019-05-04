#pragma once
#include <iostream>
#include "Bass.h"

using namespace std;

class Audio {
public:

	Audio(char* filename) 
	{
		sound = loadSample(filename);

	}
	HSAMPLE getSample() { return sound; }
	HSAMPLE loadSample(char * filename);
	
	void playSound(float volume);

private:
	HSAMPLE sound;
};