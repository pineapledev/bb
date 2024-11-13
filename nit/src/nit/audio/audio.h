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
    
    void              audio_set_instance(AudioRegistry* audio_registry_instance);
    bool              audio_has_instance();
    AudioRegistry*    audio_get_instance();
    bool              audio_is_initialized();
    void              audio_init();
    void              audio_finish();
    AudioBufferHandle audio_create_buffer(AudioFormat format, char* data, u32 size, u32 frec);
    bool              audio_is_buffer_valid(AudioBufferHandle buffer_handle);
    void              audio_free_buffer_data(AudioBufferData* data);
    void              audio_destroy_buffer(AudioBufferHandle buffer_handle);
    AudioBufferData*  audio_get_buffer_data(AudioBufferHandle buffer_handle);
    AudioSourceHandle audio_create_source(AudioBufferHandle buffer_handle);
    bool              audio_is_source_valid(AudioSourceHandle source_handle);
    void              audio_free_source_data(AudioSourceData* data);
    void              audio_destroy_source(AudioSourceHandle source_handle);
    AudioSourceData*  audio_get_source_data(AudioSourceHandle source_handle);
    void              audio_play(AudioSourceHandle source_handle);
    void              audio_stop(AudioSourceHandle source_handle);
    bool              audio_is_playing(AudioSourceHandle source_handle);
    void              audio_set_pitch(AudioSourceHandle source_handle, u32 pitch);
    void              audio_set_gain(AudioSourceHandle source_handle, u32 gain);
    void              audio_set_loop(AudioSourceHandle source_handle, bool loop);
    void              audio_translate(AudioSourceHandle source_handle, const Vector3& position);
    void              audio_set_velocity(AudioSourceHandle source_handle, const Vector3& velocity);
    void              audio_rotate(AudioSourceHandle source_handle, const Vector3& orientation);
    void              audio_translate_listener(const Vector3& position);
    void              audio_rotate_listener(const Vector3& up, const Vector3& forward);
    void              audio_set_listener_velocity(const Vector3& velocity); 
}
