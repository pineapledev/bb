#include "audio_registry.h"
#include <AL/alc.h>
#include <alc/alcmain.h>

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

            FnPool::Load<AudioSourceData>(&audio_registry->audio_sources, 100);
            FnPool::Load<AudioBufferData>(&audio_registry->audio_buffers, 100);
            
            return;
        }
    
        NIT_CHECK_MSG(false, "AudioRegistry initialization failed!\n");
    }

    void FnAudioRegistry::Finish()
    {
        if (IsAudioRegistryInvalid()) return;
        
        AudioBufferData* buffer_data = (AudioBufferData*) audio_registry->audio_buffers.elements;
        
        for (u32 i = 0; i < audio_registry->audio_buffers.sparse_set.count; ++i)
        {
            FreeBufferData(buffer_data + i);
        }

        FnPool::Free(&audio_registry->audio_sources);
        FnPool::Free(&audio_registry->audio_buffers);
        
        alcDestroyContext(static_cast<ALCcontext*>(audio_registry->context));
        audio_registry->context = nullptr;

        alcCloseDevice(static_cast<ALCdevice*>(audio_registry->device));
        audio_registry->device = nullptr;
    }

    AudioBufferHandle FnAudioRegistry::CreateBuffer(AudioFormat format, char* data, u32 size, u32 frec)
    {
        if (IsAudioRegistryInvalid()) return 0;
        
        u32 buffer_id = 0;
        alGenBuffers(1, &buffer_id);
        
        ALenum error_value = alGetError();
        
        if (error_value != AL_NO_ERROR)
        {
            switch (error_value)
            {
            case AL_INVALID_NAME       : NIT_CHECK_MSG(false, "AL error: AL_INVALID_NAME.");      break;                     
            case AL_INVALID_ENUM       : NIT_CHECK_MSG(false, "AL error: AL_INVALID_ENUM.");      break;                
            case AL_INVALID_VALUE      : NIT_CHECK_MSG(false, "AL error: AL_INVALID_VALUE.");     break;                
            case AL_INVALID_OPERATION  : NIT_CHECK_MSG(false, "AL error: AL_INVALID_OPERATION."); break;                
            case AL_OUT_OF_MEMORY      : NIT_CHECK_MSG(false, "AL error: AL_OUT_OF_MEMORY.");     break;
            default: NIT_CHECK_MSG(false, "AL unhandled error.");
            };
            return SparseSet::INVALID;
        }

        ALenum al_format;
        int    bytes_per_sample;

        switch (format)
        {
        case AudioFormat::Mono8:    al_format = AL_FORMAT_MONO8;    bytes_per_sample = 1; break;
        case AudioFormat::Mono16:   al_format = AL_FORMAT_MONO16;   bytes_per_sample = 2; break;
        case AudioFormat::Stereo8:  al_format = AL_FORMAT_STEREO8;  bytes_per_sample = 2; break;
        case AudioFormat::Stereo16: al_format = AL_FORMAT_STEREO16; bytes_per_sample = 4; break;
        case AudioFormat::None:
        default: NIT_CHECK_MSG(false, "Unsupported format"); return SparseSet::INVALID;
        }

        alBufferData(buffer_id, al_format, data, size, frec);
        
        f32 duration = (f32)size / ((f32)frec * (f32)bytes_per_sample);
        
        AudioBufferHandle handle = 0;
        
        FnPool::InsertData<AudioBufferData>(&audio_registry->audio_buffers, handle, {
            .buffer_id = buffer_id,
            .format    = format,
            .data      = data,
            .size      = size,
            .frec      = frec,
            .duration  = duration
        });
        
        return handle;
    }

    bool FnAudioRegistry::IsBufferValid(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        return FnPool::IsValid(&audio_registry->audio_buffers, buffer_handle);
    }

    void FnAudioRegistry::FreeBufferData(AudioBufferData* data)
    {
        if (!data)
        {
            NIT_CHECK(false);
            return;
        }

        for (AudioSourceHandle& source_handle : data->audio_sources)
        {
            AudioSourceData* audio_source_data = GetSourceData(source_handle);
            FreeSourceData(audio_source_data);
        }
        
        alDeleteBuffers(1, &data->buffer_id);
        
        delete[] data->data;
        data->data = nullptr;
    }

    void FnAudioRegistry::DestroyBuffer(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        
        if (!IsBufferValid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to destroy invalid buffer!");
            return;
        }

        AudioBufferData* data = GetBufferData(buffer_handle);
        FreeBufferData(data);

        for (AudioSourceHandle& source_handle : data->audio_sources)
        {
            FnPool::DeleteData(&audio_registry->audio_sources, source_handle);
        }
        
        FnPool::DeleteData(&audio_registry->audio_buffers, buffer_handle);
    }

    AudioBufferData* FnAudioRegistry::GetBufferData(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return nullptr;
        
        if (!IsBufferValid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid buffer!");
            return nullptr;
        }

        return FnPool::GetData<AudioBufferData>(&audio_registry->audio_buffers, buffer_handle);
    }

    AudioSourceHandle FnAudioRegistry::CreateSource(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return 0;

        if (!IsBufferValid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid buffer!");
            return SparseSet::INVALID;
        }

        AudioBufferData* buffer_data = GetBufferData(buffer_handle);
        
        u32 source_id = 0;
        alGenSources(1, &source_id);
        alSourcei(source_id, AL_BUFFER, buffer_data->buffer_id);
        
        AudioSourceHandle audio_source = 0;

        FnPool::InsertData<AudioSourceData>(&audio_registry->audio_sources, audio_source, {
            .source_id    = source_id,
            .audio_buffer = buffer_handle
        });

        buffer_data->audio_sources.push_back(audio_source);
        return audio_source;
    }

    bool FnAudioRegistry::IsSourceValid(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        return FnPool::IsValid(&audio_registry->audio_sources, source_handle);
    }

    void FnAudioRegistry::FreeSourceData(AudioSourceData* data)
    {
        if (!data)
        {
            NIT_CHECK(false);
            return;
        }
        
        alDeleteSources(1, &data->source_id);
    }

    void FnAudioRegistry::DestroySource(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        
        if (!IsSourceValid(source_handle))
        {
            NIT_CHECK_MSG(false, "Trying to destroy invalid source!");
            return;
        }

        AudioSourceData* source_data = GetSourceData(source_handle);
        FreeSourceData(source_data);
        FnPool::DeleteData(&audio_registry->audio_sources, source_handle);
    }

    AudioSourceData* FnAudioRegistry::GetSourceData(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return nullptr;
        
        if (!IsSourceValid(source_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid source!");
            return nullptr;
        }

        return FnPool::GetData<AudioSourceData>(&audio_registry->audio_sources, source_handle);
    }

    void FnAudioRegistry::Play(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSourcePlay(source_data->source_id);
    }

    void FnAudioRegistry::Stop(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSourceStop(source_data->source_id);
    }

    bool FnAudioRegistry::IsPlaying(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        ALint state;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alGetSourcei(source_data->source_id, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void FnAudioRegistry::SetPitch(AudioSourceHandle source_handle, const u32 pitch)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSourcei(source_data->source_id, AL_PITCH, static_cast<ALint>(pitch));
    }

    void FnAudioRegistry::SetGain(AudioSourceHandle source_handle, const u32 gain)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSourcei(source_data->source_id, AL_GAIN, static_cast<ALint>(gain));
    }

    void FnAudioRegistry::SetLoop(AudioSourceHandle source_handle, const bool loop)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSourcei(source_data->source_id, AL_LOOPING, loop);
    }

    void FnAudioRegistry::Translate(AudioSourceHandle source_handle, const Vector3& position)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSource3f(source_data->source_id, AL_POSITION, position.x, position.y, position.z);
    }

    void FnAudioRegistry::SetVelocity(AudioSourceHandle source_handle, const Vector3& velocity)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSource3f(source_data->source_id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void FnAudioRegistry::Rotate(AudioSourceHandle source_handle, const Vector3& orientation)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = GetSourceData(source_handle);
        alSource3f(source_data->source_id, AL_ORIENTATION, orientation.x, orientation.y, orientation.z);
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
