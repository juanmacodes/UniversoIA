#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "ElevenLipSyncBPLibrary.generated.h"

UCLASS()
class UNIVERSOIA26_API UElevenLipSyncBPLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Eleven|Audio")
    static bool PCMBytesInt16ToFloatArray(
        const TArray<uint8>& InBytes,
        TArray<float>& OutSamples
    );

    UFUNCTION(BlueprintCallable, Category = "Eleven|Audio")
    static USoundWaveProcedural* CreateProceduralSoundWaveFromPCM16(
        const TArray<uint8>& InBytes,
        int32 SampleRate = 44100,
        int32 NumChannels = 1
    );
};