#include "SpeechAudioBPLibrary.h"

bool USpeechAudioBPLibrary::BuildSpeechAudioFromPCM16(
    const TArray<uint8>& InBytes,
    int32 SampleRate,
    int32 NumChannels,
    USoundWaveProcedural*& OutSoundWave,
    TArray<float>& OutFloatSamples)
{
    OutSoundWave = nullptr;
    OutFloatSamples.Reset();

    if (InBytes.Num() == 0 || (InBytes.Num() % 2) != 0)
    {
        return false;
    }

    if (SampleRate <= 0 || NumChannels <= 0)
    {
        return false;
    }

    const int32 NumSamples = InBytes.Num() / 2;
    OutFloatSamples.SetNumUninitialized(NumSamples);

    const int16* PCM16 = reinterpret_cast<const int16*>(InBytes.GetData());

    for (int32 i = 0; i < NumSamples; ++i)
    {
        OutFloatSamples[i] = FMath::Clamp(static_cast<float>(PCM16[i]) / 32768.0f, -1.0f, 1.0f);
    }

    USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>(GetTransientPackage());
    if (!Wave)
    {
        OutFloatSamples.Reset();
        return false;
    }

    Wave->SetSampleRate(SampleRate);
    Wave->NumChannels = NumChannels;
    Wave->Duration = static_cast<float>(NumSamples) / static_cast<float>(SampleRate * NumChannels);
    Wave->SoundGroup = SOUNDGROUP_Voice;
    Wave->bLooping = false;
    Wave->QueueAudio(InBytes.GetData(), InBytes.Num());

    OutSoundWave = Wave;
    return true;
}