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
    
    void register_clip();
    void clip_load(AudioClip* audio_clip);
    void clip_free(AudioClip* audio_clip);
    void clip_serialize(const AudioClip* audio_clip, YAML::Emitter& emitter);
    void clip_deserialize(AudioClip* audio_clip, const YAML::Node& node);
#ifdef NIT_EDITOR_ENABLED
    void clip_draw_editor(AudioClip* audio_clip);
#endif
}