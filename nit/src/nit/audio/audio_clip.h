#pragma once
#include "nit/audio/audio_registry.h"

namespace Nit
{
    struct AudioClip
    {
        String audio_path;
        AudioBufferHandle buffer_handle = SparseSet::INVALID;
        
#ifdef NIT_EDITOR_ENABLED
        AudioSourceHandle editor_source = SparseSet::INVALID;
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