#include "audio_registry.h"
#include <AL/alc.h>
#include <alc/alcmain.h>
#include "audio_clip.h"

namespace Nit
{
    static AudioRegistry* audio_registry = nullptr;

    static bool IsAudioRegistryInvalid()
    {
        if (!audio_registry)
        {
            NIT_CHECK_MSG(false, "AudioRegistry not created!")
            return true;
        }

        if (!audio_registry->context || !audio_registry->device)
        {
            NIT_CHECK_MSG(false, "Missing AudioRegistry initialization!\n");
            return true;
        }

        return false;
    }

    void FnAudioRegistry::SetInstance(AudioRegistry* audio_registry_instance)
    {
        if (!audio_registry_instance)
        {
            NIT_CHECK(false);
            return;
        }

        audio_registry = audio_registry_instance;
    }

    bool FnAudioRegistry::HasInstance()
    {
        return audio_registry != nullptr;
    }

    AudioRegistry* FnAudioRegistry::GetInstance()
    {
        if (!audio_registry)
        {
            NIT_CHECK_MSG(false, "AudioRegistry not created!")
            return nullptr;
        }
        
        return audio_registry;
    }

    bool FnAudioRegistry::IsInitialized()
    {
        return audio_registry->context != nullptr && audio_registry->device != nullptr;
    }

    void FnAudioRegistry::Init()
    {
        if (!audio_registry)
        {
            NIT_CHECK_MSG(false, "AudioRegistry not created!")
            return;
        }
        
        if (ALCdevice* open_al_device = alcOpenDevice(nullptr))
        {
            ALCcontext* open_al_context = alcCreateContext(open_al_device, nullptr); 
            alcMakeContextCurrent(open_al_context);
            audio_registry->context = open_al_context;
            audio_registry->device  = open_al_device;
            return;
        }
    
        NIT_CHECK_MSG(false, "AudioRegistry initialization failed!\n");
    }

    void FnAudioRegistry::Finish()
    {
        if (!audio_registry)
        {
            NIT_CHECK_MSG(false, "AudioRegistry not created!")
            return;
        }

        if (audio_registry->context)
        {
            alcDestroyContext(static_cast<ALCcontext*>(audio_registry->context));
            audio_registry->context = nullptr;
        }

        if (audio_registry->device)
        {
            alcCloseDevice(static_cast<ALCdevice*>(audio_registry->device));
            audio_registry->device = nullptr;
        }
    }

    AudioSourceHandle FnAudioRegistry::CreateSource(AudioClip* clip)
    {
        if (IsAudioRegistryInvalid()) return 0;

        if (!clip)
        {
            NIT_CHECK(false);
            return 0;
        }
        
        AudioSourceHandle audio_source = 0;
        alGenSources(1, &audio_source);
        alSourcei(audio_source, AL_BUFFER, clip->buffer_id);
        return audio_source;
    }

    void FnAudioRegistry::DestroySource(AudioSourceHandle audio_source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        alDeleteSources(1, &audio_source_handle);
    }

    void FnAudioRegistry::Play(AudioSourceHandle audio_source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        alSourcePlay(audio_source_handle);
    }

    void FnAudioRegistry::Stop(AudioSourceHandle audio_source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        alSourceStop(audio_source_handle);
    }

    bool FnAudioRegistry::IsPlaying(AudioSourceHandle audio_source_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        ALint state;
        alGetSourcei(audio_source_handle, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void FnAudioRegistry::SetPitch(AudioSourceHandle audio_source_handle, const u32 pitch)
    {
        if (IsAudioRegistryInvalid()) return;
        alSourcei(audio_source_handle, AL_PITCH,
            static_cast<ALint>(pitch));
    }

    void FnAudioRegistry::SetGain(AudioSourceHandle audio_source_handle, const u32 gain)
    {
        if (IsAudioRegistryInvalid()) return;
        alSourcei(audio_source_handle, AL_GAIN,
            static_cast<ALint>(gain));
    }

    void FnAudioRegistry::SetLoop(AudioSourceHandle audio_source_handle, const bool loop)
    {
        if (IsAudioRegistryInvalid()) return;
        alSourcei(audio_source_handle, AL_LOOPING, loop);
    }

    void FnAudioRegistry::Translate(AudioSourceHandle audio_source_handle, const Vector3& position)
    {
        if (IsAudioRegistryInvalid()) return;
        alSource3f(audio_source_handle, AL_POSITION,
            position.x, position.y, position.z);
    }

    void FnAudioRegistry::SetVelocity(AudioSourceHandle audio_source_handle, const Vector3& velocity)
    {
        if (IsAudioRegistryInvalid()) return;
        alSource3f(audio_source_handle, AL_VELOCITY,
            velocity.x, velocity.y, velocity.z);
    }

    void FnAudioRegistry::Rotate(AudioSourceHandle audio_source_handle, const Vector3& orientation)
    {
        if (IsAudioRegistryInvalid()) return;
        alSource3f(audio_source_handle, AL_ORIENTATION,
            orientation.x, orientation.y, orientation.z);
    }

    void FnAudioRegistry::TranslateListener(const Vector3& position)
    {
        if (IsAudioRegistryInvalid()) return;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }
  
    void FnAudioRegistry::RotateListener(const Vector3& up, const Vector3& forward)
    {
        if (IsAudioRegistryInvalid()) return;
        const float values[] = {up.x, up.y, up.z, forward.x, forward.y, forward.z};
        alListenerfv(AL_ORIENTATION, values);
    }

    void FnAudioRegistry::SetListenerVelocity(const Vector3& velocity)
    {
        if (IsAudioRegistryInvalid()) return;
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
}
