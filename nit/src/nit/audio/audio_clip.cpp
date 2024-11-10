#include "audio_clip.h"
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

        char* data = new char[size];
        
        in.read(data, size);

        if (!FnAudioRegistry::HasInstance() || !FnAudioRegistry::IsInitialized() || FnAudioRegistry::IsBufferValid(audio_clip->buffer_handle))
        {
            NIT_CHECK(false);
            return;
        }

        audio_clip->buffer_handle = FnAudioRegistry::CreateBuffer(format, data, size, frec);
    }

    void Free(AudioClip* audio_clip)
    {
        if (!audio_clip || !FnAudioRegistry::HasInstance() || !FnAudioRegistry::IsInitialized())
        {
            NIT_CHECK(false);
            return;
        }

#ifdef NIT_EDITOR_ENABLED
        audio_clip->editor_source = SparseSet::INVALID;
#endif

        if (FnAudioRegistry::IsBufferValid(audio_clip->buffer_handle))
        {
            FnAudioRegistry::DestroyBuffer(audio_clip->buffer_handle);
        }
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
        ImGui::ResourceCombo("Path", {".wav"}, audio_clip->audio_path);
        
        if (audio_clip->prev_path != audio_clip->audio_path || !IsAssetLoaded(this_asset))
        {
            const String path = "assets/" + audio_clip->audio_path;
            std::ifstream in(path, std::ios::binary);

            audio_clip->editor_source = SparseSet::INVALID;
            ReleaseAsset(this_asset);
            
            if (in)
            {
                RetainAsset(this_asset);
                audio_clip->editor_source = FnAudioRegistry::CreateSource(audio_clip->buffer_handle);
            }

            audio_clip->prev_path = audio_clip->audio_path;
        }
        
        if (audio_clip->editor_source != SparseSet::INVALID)
        {
            auto* data = FnAudioRegistry::GetBufferData(audio_clip->buffer_handle);
            ImGui::Text("Duration %f", data->duration);
            
            if (ImGui::Button("Play"))
            {
                FnAudioRegistry::Play(audio_clip->editor_source);
            }
        }
    }
#endif
}