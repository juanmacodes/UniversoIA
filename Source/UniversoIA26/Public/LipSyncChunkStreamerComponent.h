#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LipSyncChunkTarget.h"
#include "LipSyncChunkStreamerComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNIVERSOIA26_API ULipSyncChunkStreamerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULipSyncChunkStreamerComponent();

	UFUNCTION(BlueprintCallable, Category = "LipSync")
	bool StartLipSyncStreaming(
		UObject* TargetObject,
		const TArray<float>& InSamples,
		int32 InSampleRate,
		int32 InNumChannels,
		float ChunkMilliseconds = 10.0f,
		bool bRecreateGeneratorFirst = true
	);

	UFUNCTION(BlueprintCallable, Category = "LipSync")
	void StopLipSyncStreaming();

	UFUNCTION(BlueprintPure, Category = "LipSync")
	bool IsStreaming() const { return bIsStreaming; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UObject> CurrentTarget = nullptr;

	UPROPERTY()
	TArray<float> PendingSamples;

	UPROPERTY()
	int32 PendingSampleRate = 0;

	UPROPERTY()
	int32 PendingNumChannels = 1;

	UPROPERTY()
	int32 CurrentSampleIndex = 0;

	UPROPERTY()
	int32 SamplesPerChunk = 0;

	UPROPERTY()
	bool bIsStreaming = false;

	FTimerHandle ChunkTimerHandle;

	void TickLipSyncChunk();
};