#include "ElevenLipSyncBPLibrary.h"
#include "Misc/AssertionMacros.h"

bool UElevenLipSyncBPLibrary::PCMBytesInt16ToFloatArray(
    const TArray<uint8>& InBytes,
    TArray<float>& OutSamples)
{
    OutSamples.Reset();

    if (InBytes.Num() == 0 || (InBytes.Num() % 2) != 0)
    {
        return false;
    }

    const int32 NumSamples = InBytes.Num() / 2;
    OutSamples.SetNumUninitialized(NumSamples);

    const int16* PCM16 = reinterpret_cast<const int16*>(InBytes.GetData());

    for (int32 i = 0; i < NumSamples; ++i)
    {
        OutSamples[i] = FMath::Clamp((float)PCM16[i] / 32768.0f, -1.0f, 1.0f);
    }

    return true;
}

USoundWaveProcedural* UElevenLipSyncBPLibrary::CreateProceduralSoundWaveFromPCM16(
    const TArray<uint8>& InBytes,
    int32 SampleRate,
    int32 NumChannels)
{
    if (InBytes.Num() == 0 || (InBytes.Num() % 2) != 0 || SampleRate <= 0 || NumChannels <= 0)
    {
        return nullptr;
    }

    USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>(GetTransientPackage());
    if (!Wave)
    {
        return nullptr;
    }

    Wave->SetSampleRate(SampleRate);
    Wave->NumChannels = NumChannels;
    Wave->Duration = (float)(InBytes.Num() / (2 * NumChannels)) / (float)SampleRate;
    Wave->SoundGroup = SOUNDGROUP_Voice;
    Wave->bLooping = false;
    Wave->QueueAudio(InBytes.GetData(), InBytes.Num());

    return Wave;
}