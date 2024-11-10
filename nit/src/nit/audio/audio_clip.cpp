#include "audio_clip.h"
#include <AL/al.h>
#include "audio_registry.h"
#include "core/asset.h"

#ifdef NIT_EDITOR_ENABLED
#include "core/engine.h"
#include "editor/editor.h"
#include "editor/editor_utils.h"
#endif

namespace Nit::FnAudioClip
{
    void Register()
    {
        RegisterAssetType<AudioClip>(
        {
              Load
            , Free
            , Serialize
            , Deserialize
#ifdef NIT_EDITOR_ENABLED
            , DrawEditor
#endif
        });
    }
    
    void Load(AudioClip* audio_clip)
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
        memcpy(&audio_clip->frec, buffer, 4);

        // ByteRate
        in.read(buffer, 4);

        // BlockAlign
        in.read(buffer, 2);
    
        // BitsPerSample
        in.read(buffer, 2);
        u32 bits_per_sample = 0;
        memcpy(&bits_per_sample, buffer, 2);

        if (bits_per_sample == 8)
            audio_clip->format = is_stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
        else if (bits_per_sample == 16)
            audio_clip->format = is_stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

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
        memcpy(&audio_clip->size, buffer, 4);

        audio_clip->data = new char[audio_clip->size];
        in.read(audio_clip->data, audio_clip->size);

        if (!FnAudioRegistry::HasInstance() || !FnAudioRegistry::IsInitialized())
        {
            NIT_CHECK(false);
            return;
        }
        
        alGenBuffers(1, &audio_clip->buffer_id);

        if (alGetError() != AL_NO_ERROR)
        {
            NIT_CHECK_MSG(false, "AL error.");
        }

        alBufferData(audio_clip->buffer_id, audio_clip->format, audio_clip->data, audio_clip->size, audio_clip->frec);

        // Retrieve duration
        
        int bytes_per_sample = 0;
    
        switch (audio_clip->format)
        {
        case AL_FORMAT_MONO8:     bytes_per_sample = 1; break;
        case AL_FORMAT_MONO16:    bytes_per_sample = 2; break;
        case AL_FORMAT_STEREO8:   bytes_per_sample = 2; break;
        case AL_FORMAT_STEREO16:  bytes_per_sample = 4; break;
        default: NIT_CHECK_MSG(false, "Unsupported format");
        }

        if (bytes_per_sample != 0)
        {
            audio_clip->duration = (f32) audio_clip->size / ((f32) audio_clip->frec * (f32) bytes_per_sample);
        }
    }

    void Free(AudioClip* audio_clip)
    {
        if (!audio_clip || !FnAudioRegistry::HasInstance() || !FnAudioRegistry::IsInitialized())
        {
            NIT_CHECK(false);
            return;
        }

        alDeleteBuffers(1, &audio_clip->buffer_id);
        delete[] audio_clip->data;
        audio_clip->data = nullptr;

        audio_clip->duration = 0.f;
    }

    void Serialize(const AudioClip* audio_clip, YAML::Emitter& emitter)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }
        
        emitter << YAML::Key << "audio_path" << audio_clip->audio_path;
    }

    void Deserialize(AudioClip* audio_clip, const YAML::Node& node)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }

        audio_clip->audio_path = node["audio_path"].as<String>();
    }

#ifdef NIT_EDITOR_ENABLED
    void DrawEditor(AudioClip* audio_clip)
    {
        if (!audio_clip)
        {
            NIT_CHECK(false);
            return;
        }

        AssetHandle this_asset = engine->editor.selected_asset;
        
        if (ImGui::InputText("Path", audio_clip->audio_path) || !IsAssetLoaded(this_asset))
        {
            const String path = "assets/" + audio_clip->audio_path;
            std::ifstream in(path, std::ios::binary);
        
            if (in)
            {
                ReleaseAsset(this_asset);
                RetainAsset(this_asset);
                audio_clip->source = FnAudioRegistry::CreateSource(audio_clip);
            }
            else
            {
                ReleaseAsset(this_asset);
                
                if (audio_clip->source != 0)
                {
                    FnAudioRegistry::DestroySource(audio_clip->source);
                    audio_clip->source = 0;
                }
            }
        }

        if (audio_clip->source != 0)
        {
            ImGui::Text("Duration %f", audio_clip->duration);

            if (ImGui::Button("Play"))
            {
                FnAudioRegistry::Play(audio_clip->source);
            }
        }
    }
#endif
}
