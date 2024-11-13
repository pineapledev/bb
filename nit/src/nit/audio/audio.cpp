#include "audio.h"
#include <AL/alc.h>
#include <alc/alcmain.h>

namespace nit
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

    void audio_set_instance(AudioRegistry* audio_registry_instance)
    {
        if (!audio_registry_instance)
        {
            NIT_CHECK(false);
            return;
        }

        audio_registry = audio_registry_instance;
    }

    bool audio_has_instance()
    {
        return audio_registry != nullptr;
    }

    AudioRegistry* engine_get_instance()
    {
        if (!audio_registry)
        {
            NIT_CHECK_MSG(false, "AudioRegistry not created!")
            return nullptr;
        }
        
        return audio_registry;
    }

    bool audio_is_initialized()
    {
        return audio_registry->context != nullptr && audio_registry->device != nullptr;
    }

    void audio_init()
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

            pool::load<AudioSourceData>(&audio_registry->audio_sources, 100);
            pool::load<AudioBufferData>(&audio_registry->audio_buffers, 100);
            
            return;
        }
    
        NIT_CHECK_MSG(false, "AudioRegistry initialization failed!\n");
    }

    void audio_finish()
    {
        if (IsAudioRegistryInvalid()) return;
        
        AudioBufferData* buffer_data = (AudioBufferData*) audio_registry->audio_buffers.elements;
        
        for (u32 i = 0; i < audio_registry->audio_buffers.sparse_set.count; ++i)
        {
            audio_free_buffer_data(buffer_data + i);
        }

        pool::release(&audio_registry->audio_sources);
        pool::release(&audio_registry->audio_buffers);
        
        alcDestroyContext(static_cast<ALCcontext*>(audio_registry->context));
        audio_registry->context = nullptr;

        alcCloseDevice(static_cast<ALCdevice*>(audio_registry->device));
        audio_registry->device = nullptr;
    }

    AudioBufferHandle audio_create_buffer(AudioFormat format, char* data, u32 size, u32 frec)
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
        
        pool::insert_data<AudioBufferData>(&audio_registry->audio_buffers, handle, {
            .buffer_id = buffer_id,
            .format    = format,
            .size      = size,
            .frec      = frec,
            .duration  = duration
        });
        
        return handle;
    }

    bool audio_is_buffer_valid(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        return pool::is_valid(&audio_registry->audio_buffers, buffer_handle);
    }

    void audio_free_buffer_data(AudioBufferData* data)
    {
        if (!data)
        {
            NIT_CHECK(false);
            return;
        }

        for (AudioSourceHandle& source_handle : data->audio_sources)
        {
            AudioSourceData* audio_source_data = audio_get_source_data(source_handle);
            audio_free_source_data(audio_source_data);
        }
        
        alDeleteBuffers(1, &data->buffer_id);
    }

    void audio_destroy_buffer(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        
        if (!audio_is_buffer_valid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to destroy invalid buffer!");
            return;
        }

        AudioBufferData* data = audio_get_buffer_data(buffer_handle);
        audio_free_buffer_data(data);

        for (AudioSourceHandle& source_handle : data->audio_sources)
        {
            pool::delete_data(&audio_registry->audio_sources, source_handle);
        }
        
        pool::delete_data(&audio_registry->audio_buffers, buffer_handle);
    }

    AudioBufferData* audio_get_buffer_data(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return nullptr;
        
        if (!audio_is_buffer_valid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid buffer!");
            return nullptr;
        }

        return pool::get_data<AudioBufferData>(&audio_registry->audio_buffers, buffer_handle);
    }

    AudioSourceHandle audio_create_source(AudioBufferHandle buffer_handle)
    {
        if (IsAudioRegistryInvalid()) return 0;

        if (!audio_is_buffer_valid(buffer_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid buffer!");
            return SparseSet::INVALID;
        }

        AudioBufferData* buffer_data = audio_get_buffer_data(buffer_handle);
        
        u32 source_id = 0;
        alGenSources(1, &source_id);
        alSourcei(source_id, AL_BUFFER, buffer_data->buffer_id);
        
        AudioSourceHandle audio_source = 0;

        pool::insert_data<AudioSourceData>(&audio_registry->audio_sources, audio_source, {
            .source_id    = source_id,
            .audio_buffer = buffer_handle
        });

        buffer_data->audio_sources.push_back(audio_source);
        return audio_source;
    }

    bool audio_is_source_valid(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        return pool::is_valid(&audio_registry->audio_sources, source_handle);
    }

    void audio_free_source_data(AudioSourceData* data)
    {
        if (!data)
        {
            NIT_CHECK(false);
            return;
        }
        
        alDeleteSources(1, &data->source_id);
    }

    void audio_destroy_source(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        
        if (!audio_is_source_valid(source_handle))
        {
            NIT_CHECK_MSG(false, "Trying to destroy invalid source!");
            return;
        }

        AudioSourceData* source_data = audio_get_source_data(source_handle);
        audio_free_source_data(source_data);

        AudioBufferData* buffer_data = audio_get_buffer_data(source_data->audio_buffer);
        buffer_data->audio_sources.erase(std::ranges::find(buffer_data->audio_sources, source_handle));
        
        pool::delete_data(&audio_registry->audio_sources, source_handle);
    }

    AudioSourceData* audio_get_source_data(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return nullptr;
        
        if (!audio_is_source_valid(source_handle))
        {
            NIT_CHECK_MSG(false, "Trying to use invalid source!");
            return nullptr;
        }

        return pool::get_data<AudioSourceData>(&audio_registry->audio_sources, source_handle);
    }

    void audio_play(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSourcePlay(source_data->source_id);
    }

    void audio_stop(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSourceStop(source_data->source_id);
    }

    bool audio_is_playing(AudioSourceHandle source_handle)
    {
        if (IsAudioRegistryInvalid()) return false;
        ALint state;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alGetSourcei(source_data->source_id, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    void audio_set_pitch(AudioSourceHandle source_handle, const u32 pitch)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSourcei(source_data->source_id, AL_PITCH, static_cast<ALint>(pitch));
    }

    void audio_set_gain(AudioSourceHandle source_handle, const u32 gain)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSourcei(source_data->source_id, AL_GAIN, static_cast<ALint>(gain));
    }

    void audio_set_loop(AudioSourceHandle source_handle, const bool loop)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSourcei(source_data->source_id, AL_LOOPING, loop);
    }

    void audio_translate(AudioSourceHandle source_handle, const Vector3& position)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSource3f(source_data->source_id, AL_POSITION, position.x, position.y, position.z);
    }

    void audio_set_velocity(AudioSourceHandle source_handle, const Vector3& velocity)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSource3f(source_data->source_id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }

    void audio_rotate(AudioSourceHandle source_handle, const Vector3& orientation)
    {
        if (IsAudioRegistryInvalid()) return;
        AudioSourceData* source_data = audio_get_source_data(source_handle);
        alSource3f(source_data->source_id, AL_ORIENTATION, orientation.x, orientation.y, orientation.z);
    }

    void audio_translate_listener(const Vector3& position)
    {
        if (IsAudioRegistryInvalid()) return;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
    }
  
    void audio_rotate_listener(const Vector3& up, const Vector3& forward)
    {
        if (IsAudioRegistryInvalid()) return;
        const float values[] = {up.x, up.y, up.z, forward.x, forward.y, forward.z};
        alListenerfv(AL_ORIENTATION, values);
    }

    void audio_set_listener_velocity(const Vector3& velocity)
    {
        if (IsAudioRegistryInvalid()) return;
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
}
