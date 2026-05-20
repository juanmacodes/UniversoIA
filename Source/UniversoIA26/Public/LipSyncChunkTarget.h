#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LipSyncChunkTarget.generated.h"

UINTERFACE(BlueprintType)
class UNIVERSOIA26_API ULipSyncChunkTarget : public UInterface
{
	GENERATED_BODY()
};

class UNIVERSOIA26_API ILipSyncChunkTarget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LipSync")
	void RecreateLipSyncGenerator();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "LipSync")
	void FeedLipSyncChunk(const TArray<float>& PCMData, int32 SampleRate, int32 NumChannels);
};