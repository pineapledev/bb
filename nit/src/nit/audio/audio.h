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
        void SetInstance(AudioRegistry* audio_registry_instance);
        bool HasInstance();
        AudioRegistry* GetInstance();
        bool IsInitialized();
        
        void Init();
        void Finish();

        AudioBufferHandle CreateBuffer(AudioFormat format, char* data, u32 size, u32 frec);
        bool IsBufferValid(AudioBufferHandle buffer_handle);
        void FreeBufferData(AudioBufferData* data);
        void DestroyBuffer(AudioBufferHandle buffer_handle);
        AudioBufferData* GetBufferData(AudioBufferHandle buffer_handle);
        
        AudioSourceHandle CreateSource(AudioBufferHandle buffer_handle);
        bool IsSourceValid(AudioSourceHandle source_handle);
        void FreeSourceData(AudioSourceData* data);
        void DestroySource(AudioSourceHandle source_handle);
        AudioSourceData* GetSourceData(AudioSourceHandle source_handle);
        
        void Play(AudioSourceHandle source_handle);
        void Stop(AudioSourceHandle source_handle);
        bool IsPlaying(AudioSourceHandle source_handle);
        void SetPitch(AudioSourceHandle source_handle, u32 pitch);
        void SetGain(AudioSourceHandle source_handle, u32 gain);
        void SetLoop(AudioSourceHandle source_handle, bool loop);
        void Translate(AudioSourceHandle source_handle, const Vector3& position);
        void SetVelocity(AudioSourceHandle source_handle, const Vector3& velocity);
        void Rotate(AudioSourceHandle source_handle, const Vector3& orientation);
        void TranslateListener(const Vector3& position);
        void RotateListener(const Vector3& up, const Vector3& forward);
        void SetListenerVelocity(const Vector3& velocity); 
    }
}
