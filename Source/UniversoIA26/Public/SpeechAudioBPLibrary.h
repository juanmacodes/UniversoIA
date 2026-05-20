#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "SpeechAudioBPLibrary.generated.h"

UCLASS()
class UNIVERSOIA26_API USpeechAudioBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Speech|Audio")
    static bool BuildSpeechAudioFromPCM16(
        const TArray<uint8>& InBytes,
        int32 SampleRate,
        int32 NumChannels,
        USoundWaveProcedural*& OutSoundWave,
        TArray<float>& OutFloatSamples
    );
};