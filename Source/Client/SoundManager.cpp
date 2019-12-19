#include "SoundManager.h"
#include "FileUtils.h"
#include "Log.h"
#include "ResourceManager.h"
#include "StringUtils.h"
#include "Timer.h"

// SDL
#include "SDL_audio.h"

// ACM
#include "acmstrm.h"

// Vorbis
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"

class SoundManagerImpl : public ISoundManager
{
public:
    SoundManagerImpl();
    ~SoundManagerImpl() override;
    bool PlaySound(const StrMap& sound_names, const string& name) override;
    bool PlayMusic(const string& fname, uint repeat_time) override;
    void StopSounds() override;
    void StopMusic() override;

private:
    struct Sound
    {
        vector<uchar> BaseBuf {};
        size_t BaseBufLen {};
        unique_ptr<SDL_AudioCVT> Cvt {};
        vector<uchar> ConvertedBuf {};
        size_t ConvertedBufLen {};
        size_t ConvertedBufCur {};
        int OriginalFormat {};
        int OriginalChannels {};
        int OriginalRate {};
        bool IsMusic {};
        uint NextPlay {};
        uint RepeatTime {};
        unique_ptr<OggVorbis_File, std::function<void(OggVorbis_File*)>> OggStream {};
    };

    using SoundsFunc = std::function<void(uchar*)>;
    using SoundVec = vector<shared_ptr<Sound>>;

    void ProcessSounds(uchar* output);
    bool ProcessSound(shared_ptr<Sound> sound, uchar* output);
    shared_ptr<Sound> Load(const string& fname, bool is_music);
    bool LoadWAV(shared_ptr<Sound> sound, const string& fname);
    bool LoadACM(shared_ptr<Sound> sound, const string& fname, bool is_music);
    bool LoadOGG(shared_ptr<Sound> sound, const string& fname);
    bool StreamOGG(shared_ptr<Sound> sound);
    bool ConvertData(shared_ptr<Sound> sound);

    bool isActive;
    bool isAudioInited;
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec soundSpec;
    uint streamingPortion;
    SoundVec soundsActive;
    UCharVec outputBuf;
    SoundsFunc soundsFunc;
};

SoundManager ISoundManager::Create()
{
    return std::make_shared<SoundManagerImpl>();
}

SoundManagerImpl::SoundManagerImpl() :
    isActive {},
    isAudioInited {},
    deviceId {},
    soundSpec {},
    streamingPortion {},
    soundsActive {},
    outputBuf {},
    soundsFunc {}
{
    UNUSED_VARIABLE(OV_CALLBACKS_DEFAULT);
    UNUSED_VARIABLE(OV_CALLBACKS_NOCLOSE);
    UNUSED_VARIABLE(OV_CALLBACKS_STREAMONLY);
    UNUSED_VARIABLE(OV_CALLBACKS_STREAMONLY_NOCLOSE);

    // SDL
    isAudioInited = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (!isAudioInited)
    {
        WriteLog("SDL Audio initialization fail, error '{}'.\n", SDL_GetError());
        return;
    }

    // Create audio device
    SDL_AudioSpec desired;
    memzero(&desired, sizeof(desired));
#ifdef FO_WEB
    desired.format = AUDIO_F32;
    desired.freq = 48000;
    desired.channels = 2;
    streamingPortion = 0x20000; // 128kb
#else
    desired.format = AUDIO_S16;
    desired.freq = 44100;
    streamingPortion = 0x10000; // 64kb
#endif
    desired.callback = [](void* userdata, Uint8* stream, int len) {
        auto& func = *reinterpret_cast<SoundsFunc*>(userdata);
        func(stream);
    };
    soundsFunc = std::bind(&SoundManagerImpl::ProcessSounds, this, std::placeholders::_1);
    desired.userdata = &soundsFunc;

    deviceId = SDL_OpenAudioDevice(nullptr, 0, &desired, &soundSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (deviceId < 2)
    {
        WriteLog("SDL Open audio device fail, error '{}'.\n", SDL_GetError());
        return;
    }

    outputBuf.resize(soundSpec.size);
    isActive = true;

    // Start playing
    SDL_PauseAudioDevice(deviceId, 0);
}

SoundManagerImpl::~SoundManagerImpl()
{
    if (isActive)
    {
        StopSounds();
        StopMusic();
    }

    if (deviceId >= 2)
        SDL_CloseAudioDevice(deviceId);

    if (isAudioInited)
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SoundManagerImpl::ProcessSounds(uchar* output)
{
    memset(output, soundSpec.silence, soundSpec.size);

    for (auto it = soundsActive.begin(); it != soundsActive.end();)
    {
        shared_ptr<Sound> sound = *it;

        if (ProcessSound(sound, &outputBuf[0]))
        {
            int volume = (sound->IsMusic ? GameOpt.MusicVolume : GameOpt.SoundVolume);
            volume = std::clamp(volume, 0, 100) * SDL_MIX_MAXVOLUME / 100;
            SDL_MixAudioFormat(output, &outputBuf[0], soundSpec.format, soundSpec.size, volume);
            ++it;
        }
        else
        {
            it = soundsActive.erase(it);
        }
    }
}

bool SoundManagerImpl::ProcessSound(shared_ptr<Sound> sound, uchar* output)
{
    // Playing
    size_t whole = soundSpec.size;
    if (sound->ConvertedBufCur < sound->ConvertedBufLen)
    {
        if (whole > sound->ConvertedBufLen - sound->ConvertedBufCur)
        {
            // Flush last part of buffer
            size_t offset = sound->ConvertedBufLen - sound->ConvertedBufCur;
            memcpy(output, &sound->ConvertedBuf[sound->ConvertedBufCur], offset);
            sound->ConvertedBufCur += offset;

            // Stream new parts
            while (offset < whole && sound->OggStream && StreamOGG(sound))
            {
                size_t write = sound->ConvertedBufLen - sound->ConvertedBufCur;
                if (offset + write > whole)
                    write = whole - offset;

                memcpy(output + offset, &sound->ConvertedBuf[sound->ConvertedBufCur], write);
                sound->ConvertedBufCur += write;
                offset += write;
            }

            // Cut off end
            if (offset < whole)
                memset(output + offset, soundSpec.silence, whole - offset);
        }
        else
        {
            // Copy
            memcpy(output, &sound->ConvertedBuf[sound->ConvertedBufCur], whole);
            sound->ConvertedBufCur += whole;
        }

        if (sound->OggStream && sound->ConvertedBufCur == sound->ConvertedBufLen)
            StreamOGG(sound);

        // Continue processing
        return true;
    }

    // Repeat
    if (sound->RepeatTime)
    {
        if (!sound->NextPlay)
        {
            // Set next playing time
            sound->NextPlay = Timer::GameTick() + (sound->RepeatTime > 1 ? sound->RepeatTime : 0);
        }

        if (Timer::GameTick() >= sound->NextPlay)
        {
            // Set buffer to beginning
            sound->ConvertedBufCur = 0;
            if (sound->OggStream)
                ov_raw_seek(sound->OggStream.get(), 0);

            // Drop timer
            sound->NextPlay = 0;

            // Process without silent
            return ProcessSound(sound, output);
        }

        // Give silent
        memset(output, soundSpec.silence, whole);
        return true;
    }

    // Give silent
    memset(output, soundSpec.silence, whole);
    return false;
}

shared_ptr<SoundManagerImpl::Sound> SoundManagerImpl::Load(const string& fname, bool is_music)
{
    string fixed_fname = fname;
    string ext = _str(fname).getFileExtension();

    // Default ext
    if (ext.empty())
    {
        ext = "acm";
        fixed_fname += "." + ext;
    }

    auto sound = std::make_shared<Sound>();
    if (ext == "wav" && !LoadWAV(sound, fixed_fname))
        return nullptr;
    if (ext == "acm" && !LoadACM(sound, fixed_fname, is_music))
        return nullptr;
    if (ext == "ogg" && !LoadOGG(sound, fixed_fname))
        return nullptr;

    SDL_LockAudioDevice(deviceId);
    soundsActive.push_back(sound);
    SDL_UnlockAudioDevice(deviceId);
    return sound;
}

bool SoundManagerImpl::LoadWAV(shared_ptr<Sound> sound, const string& fname)
{
    File fm;
    if (!fm.LoadFile(fname))
        return false;

    uint dw_buf = fm.GetLEUInt();
    if (dw_buf != MAKEUINT('R', 'I', 'F', 'F'))
    {
        WriteLog("'RIFF' not found.\n");
        return false;
    }

    fm.GoForward(4);

    dw_buf = fm.GetLEUInt();
    if (dw_buf != MAKEUINT('W', 'A', 'V', 'E'))
    {
        WriteLog("'WAVE' not found.\n");
        return false;
    }

    dw_buf = fm.GetLEUInt();
    if (dw_buf != MAKEUINT('f', 'm', 't', ' '))
    {
        WriteLog("'fmt ' not found.\n");
        return false;
    }

    dw_buf = fm.GetLEUInt();
    if (!dw_buf)
    {
        WriteLog("Unknown format.\n");
        return false;
    }

    struct // WAVEFORMATEX
    {
        ushort wFormatTag; // Integer identifier of the format
        ushort nChannels; // Number of audio channels
        uint nSamplesPerSec; // Audio sample rate
        uint nAvgBytesPerSec; // Bytes per second (possibly approximate)
        ushort nBlockAlign; // Size in bytes of a sample block (all channels)
        ushort wBitsPerSample; // Size in bits of a single per-channel sample
        ushort cbSize; // Bytes of extra data appended to this struct
    } waveformatex;

    fm.CopyMem(&waveformatex, 16);

    if (waveformatex.wFormatTag != 1)
    {
        WriteLog("Compressed files not supported.\n");
        return false;
    }

    fm.GoForward(dw_buf - 16);

    dw_buf = fm.GetLEUInt();
    if (dw_buf == MAKEUINT('f', 'a', 'c', 't'))
    {
        dw_buf = fm.GetLEUInt();
        fm.GoForward(dw_buf);
        dw_buf = fm.GetLEUInt();
    }

    if (dw_buf != MAKEUINT('d', 'a', 't', 'a'))
    {
        WriteLog("Unknown format2.\n");
        return false;
    }

    dw_buf = fm.GetLEUInt();
    sound->BaseBuf.resize(dw_buf);
    sound->BaseBufLen = dw_buf;

    // Check format
    sound->OriginalChannels = waveformatex.nChannels;
    sound->OriginalRate = waveformatex.nSamplesPerSec;
    switch (waveformatex.wBitsPerSample)
    {
    case 8:
        sound->OriginalFormat = AUDIO_U8;
        break;
    case 16:
        sound->OriginalFormat = AUDIO_S16;
        break;
    default:
        WriteLog("Unknown format.\n");
        return false;
    }

    // Convert
    if (!fm.CopyMem(&sound->BaseBuf[0], static_cast<uint>(sound->BaseBufLen)))
    {
        WriteLog("File truncated.\n");
        return false;
    }

    return ConvertData(sound);
}

bool SoundManagerImpl::LoadACM(shared_ptr<Sound> sound, const string& fname, bool is_music)
{
    File fm;
    if (!fm.LoadFile(fname))
        return false;

    int channels = 0;
    int freq = 0;
    int samples = 0;
    auto acm = std::make_unique<CACMUnpacker>(fm.GetBuf(), fm.GetFsize(), channels, freq, samples);
    int buf_size = samples * 2;

    sound->OriginalFormat = AUDIO_S16;
    sound->OriginalChannels = (is_music ? 2 : 1);
    sound->OriginalRate = 22050;
    sound->BaseBuf.resize(buf_size);
    sound->BaseBufLen = sound->BaseBuf.size();

    auto* buf = reinterpret_cast<unsigned short*>(&sound->BaseBuf[0]);
    int dec_data = acm->readAndDecompress(buf, buf_size);
    if (dec_data != buf_size)
    {
        WriteLog("Decode Acm error.\n");
        return false;
    }

    return ConvertData(sound);
}

bool SoundManagerImpl::LoadOGG(shared_ptr<Sound> sound, const string& fname)
{
    File* fm = new File();
    if (!fm->LoadFile(fname))
    {
        delete fm;
        return false;
    }

    ov_callbacks callbacks;
    callbacks.read_func = [](void* ptr, size_t size, size_t nmemb, void* datasource) -> size_t {
        File* fm = (File*)datasource;
        return fm->CopyMem(ptr, (uint)size) ? size : 0;
    };
    callbacks.seek_func = [](void* datasource, ogg_int64_t offset, int whence) -> int {
        File* fm = (File*)datasource;
        switch (whence)
        {
        case SEEK_SET:
            fm->SetCurPos((uint)offset);
            break;
        case SEEK_CUR:
            fm->GoForward((uint)offset);
            break;
        case SEEK_END:
            fm->SetCurPos(fm->GetFsize() - (uint)offset);
            break;
        default:
            return -1;
        }
        return 0;
    };
    callbacks.close_func = [](void* datasource) -> int {
        File* fm = (File*)datasource;
        delete fm;
        return 0;
    };
    callbacks.tell_func = [](void* datasource) -> long {
        File* fm = (File*)datasource;
        return fm->GetCurPos();
    };

    sound->OggStream =
        unique_ptr<OggVorbis_File, std::function<void(OggVorbis_File*)>>(new OggVorbis_File(), [](auto* vf) {
            ov_clear(vf);
            delete vf;
        });

    int error = ov_open_callbacks(fm, sound->OggStream.get(), nullptr, 0, callbacks);
    if (error)
    {
        WriteLog("Open OGG file '{}' fail, error:\n", fname);
        switch (error)
        {
        case OV_EREAD:
            WriteLog("A read from media returned an error.\n");
            break;
        case OV_ENOTVORBIS:
            WriteLog("Bitstream does not contain any Vorbis data.\n");
            break;
        case OV_EVERSION:
            WriteLog("Vorbis version mismatch.\n");
            break;
        case OV_EBADHEADER:
            WriteLog("Invalid Vorbis bitstream header.\n");
            break;
        case OV_EFAULT:
            WriteLog("Internal logic fault; indicates a bug or heap/stack corruption.\n");
            break;
        default:
            WriteLog("Unknown error code {}.\n", error);
            break;
        }
        return false;
    }

    vorbis_info* vi = ov_info(sound->OggStream.get(), -1);
    if (!vi)
    {
        WriteLog("Ogg info error.\n");
        return false;
    }

    sound->OriginalFormat = AUDIO_S16;
    sound->OriginalChannels = vi->channels;
    sound->OriginalRate = (int)vi->rate;
    sound->BaseBuf.resize(streamingPortion);
    sound->BaseBufLen = streamingPortion;

    int result = 0;
    uint decoded = 0;
    while (true)
    {
        char* buf = reinterpret_cast<char*>(&sound->BaseBuf[0]);
        result = (int)ov_read(sound->OggStream.get(), buf + decoded, streamingPortion - decoded, 0, 2, 1, nullptr);
        if (result <= 0)
            break;

        decoded += result;
        if (decoded >= streamingPortion)
            break;
    }
    if (result < 0)
        return false;

    sound->BaseBufLen = decoded;

    // No need streaming
    if (!result)
        sound->OggStream = nullptr;

    return ConvertData(sound);
}

bool SoundManagerImpl::StreamOGG(shared_ptr<Sound> sound)
{
    long result = 0;
    uint decoded = 0;
    while (true)
    {
        char* buf = reinterpret_cast<char*>(&sound->BaseBuf[decoded]);
        result = ov_read(sound->OggStream.get(), buf, streamingPortion - decoded, 0, 2, 1, nullptr);
        if (result <= 0)
            break;

        decoded += result;
        if (decoded >= streamingPortion)
            break;
    }
    if (result < 0 || !decoded)
        return false;

    sound->BaseBufLen = decoded;
    return ConvertData(sound);
}

bool SoundManagerImpl::ConvertData(shared_ptr<Sound> sound)
{
    if (!sound->Cvt)
    {
        sound->Cvt = std::make_unique<SDL_AudioCVT>();

        if (SDL_BuildAudioCVT(sound->Cvt.get(), sound->OriginalFormat, sound->OriginalChannels, sound->OriginalRate,
                soundSpec.format, soundSpec.channels, soundSpec.freq) == -1)
        {
            WriteLog("SDL_BuildAudioCVT fail, error '{}'.\n", SDL_GetError());
            return false;
        }
    }

    sound->Cvt->len = static_cast<int>(sound->BaseBufLen);
    sound->ConvertedBuf.resize(size_t(1) * sound->Cvt->len * sound->Cvt->len_mult);
    sound->Cvt->buf = reinterpret_cast<Uint8*>(&sound->ConvertedBuf[0]);
    memcpy(sound->Cvt->buf, &sound->BaseBuf[0], sound->BaseBufLen);

    if (SDL_ConvertAudio(sound->Cvt.get()))
    {
        WriteLog("SDL_ConvertAudio fail, error '{}'.\n", SDL_GetError());
        return false;
    }

    sound->ConvertedBufCur = 0;
    sound->ConvertedBufLen = sound->Cvt->len_cvt;
    return true;
}

bool SoundManagerImpl::PlaySound(const StrMap& sound_names, const string& name)
{
    if (!isActive || !GameOpt.SoundVolume)
        return true;

    // Make 'NAME'
    string sound_name = _str(name).eraseFileExtension().upper();

    // Find base
    auto it = sound_names.find(sound_name);
    if (it != sound_names.end())
        return Load(it->second, false) != nullptr;

    // Check random pattern 'NAME_X'
    uint count = 0;
    while (sound_names.find(_str("{}_{}", sound_name, count + 1)) != sound_names.end())
        count++;
    if (count)
        return Load(sound_names.find(_str("{}_{}", sound_name, Random(1, count)))->second, false) != nullptr;

    return false;
}

bool SoundManagerImpl::PlayMusic(const string& fname, uint repeat_time)
{
    if (!isActive)
        return true;

    StopMusic();

    shared_ptr<Sound> sound = Load(fname, true);
    if (!sound)
        return false;

    sound->IsMusic = true;
    sound->RepeatTime = repeat_time;
    return true;
}

void SoundManagerImpl::StopSounds()
{
    SDL_LockAudioDevice(deviceId);
    soundsActive.erase(std::remove_if(soundsActive.begin(), soundsActive.end(), [](auto s) { return !s->IsMusic; }),
        soundsActive.end());
    SDL_UnlockAudioDevice(deviceId);
}

void SoundManagerImpl::StopMusic()
{
    SDL_LockAudioDevice(deviceId);
    soundsActive.erase(std::remove_if(soundsActive.begin(), soundsActive.end(), [](auto s) { return s->IsMusic; }),
        soundsActive.end());
    SDL_UnlockAudioDevice(deviceId);
}
