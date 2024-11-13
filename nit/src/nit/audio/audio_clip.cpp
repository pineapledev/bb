#include "audio_clip.h"
#include "core/asset.h"

#ifdef NIT_EDITOR_ENABLED
#include "core/engine.h"
#include "editor/editor.h"
#include "editor/editor_utils.h"
#endif

namespace nit::audio::clip
{
    void type_register()
    {
        register_asset_type<AudioClip>(
        {
              load
            , free
            , serialize
            , deserialize
#ifdef NIT_EDITOR_ENABLED
            , draw_editor
#endif
        });
    }
    
    void load(AudioClip* audio_clip)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }
        
        if (audio_clip->audio_path.empty())
        {
            NIT_CHECK(false);
            return;
        }
        
        char buffer[4];
        const String path = "assets/" + audio_clip->audio_path;
        std::ifstream in(path, std::ios::binary);
        
        if (!in)
        {
            NIT_CHECK(false);
            return;
        }
        
        // ChunkID (“RIFF”)
        u32 chunk_id = 0;
        in.read(buffer, 4);
        if (strncmp(buffer, "RIFF", 4) != 0)
        {
            NIT_CHECK_MSG(false, "This is not a valid WAV file");
        }

        memcpy(&chunk_id, buffer, 4);

        // RiffChunkSize
        in.read(buffer, 4);
    
        // Format "WAVE"
        in.read(buffer, 4);
        if (strncmp(buffer, "WAVE", 4) != 0)
        {
            NIT_CHECK_MSG(false, "This is not a valid WAV file");
        }
    
        // SubChunkId "fmt"
        in.read(buffer, 4);
        if (strncmp(buffer, "fmt", 3) != 0)
        {
            NIT_CHECK_MSG(false, "This is not a valid WAV file");
        }

        // FmtChunkSize
        u32 chunk_size{0};
        in.read(buffer, 4);
        memcpy(&chunk_size, buffer, 4);

        const bool read_extra_params = chunk_size > 16;

        // AudioFormat
        in.read(buffer, 2);
    
        // Channels
        in.read(buffer, 2);
        u32 channel{0};
        memcpy(&channel, buffer, 4);
        const bool is_stereo = channel != 1;

        // SampleRate
        in.read(buffer, 4);
        u32 frec = 0;
        memcpy(&frec, buffer, 4);

        // ByteRate
        in.read(buffer, 4);

        // BlockAlign
        in.read(buffer, 2);
    
        // BitsPerSample
        in.read(buffer, 2);
        u32 bits_per_sample = 0;
        memcpy(&bits_per_sample, buffer, 2);

        AudioFormat format = AudioFormat::None;
        
        if (bits_per_sample == 8)
            format = is_stereo ? AudioFormat::Stereo8 : AudioFormat::Mono8;
        else if (bits_per_sample == 16)
            format = is_stereo ? AudioFormat::Stereo16 : AudioFormat::Mono16;

        // ExtraParamsSize
        if (read_extra_params)
        {
            in.read(buffer, 2);
            u32 extra_param_size = 0;
            memcpy(&extra_param_size, buffer, 2);
            // ExtraParams
            in.read(buffer, extra_param_size);
        }

        // Data
        while(strncmp(buffer, "data", 4) != 0)
            in.read(buffer, 4);

        // DataSize
        in.read(buffer, 4);

        u32 size = 0;
        memcpy(&size, buffer, 4);

        //TODO: Not multi-thread friendly
        static u32 max_buffer_size = 10000;
        static char* data = new char[max_buffer_size]; 
        
        if (size > max_buffer_size)
        {
            max_buffer_size = size;
            delete [] data;
            data = new char[max_buffer_size];
        }

        std::fill_n(data, max_buffer_size, 0);
        
        in.read(data, size);

        if (!audio_has_instance() || !audio::audio_is_initialized() || audio::audio_is_buffer_valid(audio_clip->buffer_handle))
        {
            NIT_CHECK(false);
            return;
        }

        audio_clip->buffer_handle = audio::audio_create_buffer(format, data, size, frec);
    }

    void free(AudioClip* audio_clip)
    {
        if (!audio_clip || !audio_has_instance() || !audio::audio_is_initialized())
        {
            NIT_CHECK(false);
            return;
        }

#ifdef NIT_EDITOR_ENABLED
        audio_clip->editor_source = SparseSet::INVALID;
#endif

        if (audio::audio_is_buffer_valid(audio_clip->buffer_handle))
        {
            audio::audio_destroy_buffer(audio_clip->buffer_handle);
        }
    }

    void serialize(const AudioClip* audio_clip, YAML::Emitter& emitter)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }
        
        emitter << YAML::Key << "audio_path" << audio_clip->audio_path;
    }

    void deserialize(AudioClip* audio_clip, const YAML::Node& node)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }

        audio_clip->audio_path = node["audio_path"].as<String>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(AudioClip* audio_clip)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }

        AssetHandle this_asset = engine_get_instance()->editor.selected_asset;
        editor::draw_resource_combo("Path", {".wav"}, audio_clip->audio_path);
        
        if (audio_clip->prev_path != audio_clip->audio_path || !is_asset_loaded(this_asset))
        {
            const String path = "assets/" + audio_clip->audio_path;
            std::ifstream in(path, std::ios::binary);

            audio_clip->editor_source = SparseSet::INVALID;
            release_asset(this_asset);
            
            if (in)
            {
                retain_asset(this_asset);
                audio_clip->editor_source = audio::audio_create_source(audio_clip->buffer_handle);
            }

            audio_clip->prev_path = audio_clip->audio_path;
        }
        
        if (audio_clip->editor_source != SparseSet::INVALID)
        {
            auto* data = audio::audio_get_buffer_data(audio_clip->buffer_handle);
            ImGui::Text("Duration %f", data->duration);
            
            if (ImGui::Button("Play"))
            {
                audio::audio_play(audio_clip->editor_source);
            }
        }
    }
#endif
}