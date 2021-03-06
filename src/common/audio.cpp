#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#include "audio.hpp"

static ALCdevice* al_device;
static ALCcontext* al_context;

void Audio::initialize()
{
   al_device = alcOpenDevice(NULL);
   if(al_device == NULL) {
      std::cerr << "Warning: OpenAL device could not be opened. Audio is disabled!\n";
      return;
   }

   al_context = alcCreateContext(al_device, NULL);
   alcMakeContextCurrent(al_context);

   // TODO: properly configure AL_SPEED_OF_SOUND
}

void Audio::shutdown()
{
   if(al_device)
      alcCloseDevice(al_device);

   alcDestroyContext(al_context);
}
