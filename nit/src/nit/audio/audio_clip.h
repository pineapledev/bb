#pragma once
#include "nit/audio/audio.h"

namespace nit
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
    
    namespace audio::clip
    {
        void register_type();
        void load(AudioClip* audio_clip);
        void free(AudioClip* audio_clip);
        void serialize(const AudioClip* audio_clip, YAML::Emitter& emitter);
        void deserialize(AudioClip* audio_clip, const YAML::Node& node);
#ifdef NIT_EDITOR_ENABLED
        void draw_editor(AudioClip* audio_clip);
#endif
    }
}