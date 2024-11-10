#pragma once
#include "nit/audio/audio_registry.h"

namespace Nit
{
    struct AudioClip
    {
        String audio_path;
        f32    duration = 0.f;
        
        char*  data      = nullptr;
        u32    buffer_id = 0;
        u32    format    = 0;
        u32    size      = 0;
        u32    frec      = 0;
        
#ifdef NIT_EDITOR_ENABLED
        AudioSourceHandle editor_source = 0;
        String prev_path;
        #endif
    };
    
    namespace FnAudioClip
    {
        void Register();
        void Load(AudioClip* audio_clip);
        void Free(AudioClip* audio_clip);
        void Serialize(const AudioClip* audio_clip, YAML::Emitter& emitter);
        void Deserialize(AudioClip* audio_clip, const YAML::Node& node);
#ifdef NIT_EDITOR_ENABLED
        void DrawEditor(AudioClip* audio_clip);
#endif
    }
}