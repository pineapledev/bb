#pragma once

namespace Nit
{
    using AudioSourceHandle = u32; 
    
    struct AudioRegistry
    {
        void* context = nullptr;
        void* device  = nullptr;
    };

    struct AudioClip;
    
    namespace FnAudioRegistry
    {
        void SetInstance(AudioRegistry* audio_registry_instance);
        bool HasInstance();
        AudioRegistry* GetInstance();
        bool IsInitialized();
        
        void Init();
        void Finish();
        
        AudioSourceHandle CreateSource(AudioClip* clip);
        void DestroySource(AudioSourceHandle audio_source_handle);
        void Play(AudioSourceHandle audio_source_handle);
        void Stop(AudioSourceHandle audio_source_handle);
        bool IsPlaying(AudioSourceHandle audio_source_handle);
        void SetPitch(AudioSourceHandle audio_source_handle, u32 pitch);
        void SetGain(AudioSourceHandle audio_source_handle, u32 gain);
        void SetLoop(AudioSourceHandle audio_source_handle, bool loop);
        void Translate(AudioSourceHandle audio_source_handle, const Vector3& position);
        void SetVelocity(AudioSourceHandle audio_source_handle, const Vector3& velocity);
        void Rotate(AudioSourceHandle audio_source_handle, const Vector3& orientation);
        void TranslateListener(const Vector3& position);
        void RotateListener(const Vector3& up, const Vector3& forward);
        void SetListenerVelocity(const Vector3& velocity); 
    }
}