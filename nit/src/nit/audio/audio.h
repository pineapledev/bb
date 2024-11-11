#pragma once

namespace nit
{
    enum class AudioFormat : u8
    {
        None,
        Mono8,
        Mono16,
        Stereo8,
        Stereo16
    };
    
    using AudioSourceHandle = u32; 
    using AudioBufferHandle = u32;
    
    struct AudioSourceData
    {
        u32 source_id    = 0;
        u32 audio_buffer = SparseSet::INVALID;
    };
    
    struct AudioBufferData
    {
        u32         buffer_id     = 0;
        AudioFormat format        = AudioFormat::None;
        u32         size          = 0;
        u32         frec          = 0;
        f32         duration      = 0.f;
        Array<u32>  audio_sources = {};
    };
    
    struct AudioRegistry
    {
        void* context = nullptr;
        void* device  = nullptr;
        
        Pool audio_buffers;
        Pool audio_sources;
    };
    
    namespace audio
    {
        void set_instance(AudioRegistry* audio_registry_instance);
        bool has_instance();
        AudioRegistry* get_instance();
        bool is_initialized();
        
        void init();
        void finish();

        AudioBufferHandle create_buffer(AudioFormat format, char* data, u32 size, u32 frec);
        bool is_buffer_valid(AudioBufferHandle buffer_handle);
        void free_buffer_data(AudioBufferData* data);
        void destroy_buffer(AudioBufferHandle buffer_handle);
        AudioBufferData* get_buffer_data(AudioBufferHandle buffer_handle);
        
        AudioSourceHandle create_source(AudioBufferHandle buffer_handle);
        bool is_source_valid(AudioSourceHandle source_handle);
        void free_source_data(AudioSourceData* data);
        void destroy_source(AudioSourceHandle source_handle);
        AudioSourceData* get_source_data(AudioSourceHandle source_handle);
        
        void play(AudioSourceHandle source_handle);
        void stop(AudioSourceHandle source_handle);
        bool is_playing(AudioSourceHandle source_handle);
        void set_pitch(AudioSourceHandle source_handle, u32 pitch);
        void set_gain(AudioSourceHandle source_handle, u32 gain);
        void set_loop(AudioSourceHandle source_handle, bool loop);
        void translate(AudioSourceHandle source_handle, const Vector3& position);
        void set_velocity(AudioSourceHandle source_handle, const Vector3& velocity);
        void rotate(AudioSourceHandle source_handle, const Vector3& orientation);
        void translate_listener(const Vector3& position);
        void rotate_listener(const Vector3& up, const Vector3& forward);
        void set_listener_velocity(const Vector3& velocity); 
    }
}
