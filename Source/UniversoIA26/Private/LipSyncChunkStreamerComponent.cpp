#include "LipSyncChunkStreamerComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ULipSyncChunkStreamerComponent::ULipSyncChunkStreamerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULipSyncChunkStreamerComponent::StartLipSyncStreaming(
	UObject* TargetObject,
	const TArray<float>& InSamples,
	int32 InSampleRate,
	int32 InNumChannels,
	float ChunkMilliseconds,
	bool bRecreateGeneratorFirst
)
{
	StopLipSyncStreaming();

	if (!TargetObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartLipSyncStreaming: TargetObject es null."));
		return false;
	}

	if (!TargetObject->GetClass()->ImplementsInterface(ULipSyncChunkTarget::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartLipSyncStreaming: TargetObject no implementa LipSyncChunkTarget."));
		return false;
	}

	if (InSamples.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartLipSyncStreaming: no hay samples."));
		return false;
	}

	if (InSampleRate <= 0 || InNumChannels <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartLipSyncStreaming: SampleRate o NumChannels inválidos."));
		return false;
	}

	if (ChunkMilliseconds <= 0.0f)
	{
		ChunkMilliseconds = 10.0f;
	}

	CurrentTarget = TargetObject;
	PendingSamples = InSamples;
	PendingSampleRate = InSampleRate;
	PendingNumChannels = InNumChannels;
	CurrentSampleIndex = 0;

	const int32 SamplesPerChannelPerChunk = FMath::Max(1, FMath::RoundToInt((PendingSampleRate * ChunkMilliseconds) / 1000.0f));
	SamplesPerChunk = SamplesPerChannelPerChunk * PendingNumChannels;

	if (bRecreateGeneratorFirst)
	{
		ILipSyncChunkTarget::Execute_RecreateLipSyncGenerator(CurrentTarget);
	}

	bIsStreaming = true;

	// Primer chunk inmediato
	TickLipSyncChunk();

	if (bIsStreaming)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ChunkTimerHandle,
			this,
			&ULipSyncChunkStreamerComponent::TickLipSyncChunk,
			ChunkMilliseconds / 1000.0f,
			true
		);
	}

	return true;
}

void ULipSyncChunkStreamerComponent::TickLipSyncChunk()
{
	if (!bIsStreaming || !CurrentTarget)
	{
		StopLipSyncStreaming();
		return;
	}

	if (CurrentSampleIndex >= PendingSamples.Num())
	{
		StopLipSyncStreaming();
		return;
	}

	const int32 EndIndex = FMath::Min(CurrentSampleIndex + SamplesPerChunk, PendingSamples.Num());
	const int32 Count = EndIndex - CurrentSampleIndex;

	TArray<float> Chunk;
	Chunk.Reserve(Count);
	for (int32 i = CurrentSampleIndex; i < EndIndex; ++i)
	{
		Chunk.Add(PendingSamples[i]);
	}

	ILipSyncChunkTarget::Execute_FeedLipSyncChunk(CurrentTarget, Chunk, PendingSampleRate, PendingNumChannels);

	CurrentSampleIndex = EndIndex;

	if (CurrentSampleIndex >= PendingSamples.Num())
	{
		StopLipSyncStreaming();
	}
}

void ULipSyncChunkStreamerComponent::StopLipSyncStreaming()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ChunkTimerHandle);
	}

	bIsStreaming = false;
	CurrentTarget = nullptr;
	PendingSamples.Reset();
	PendingSampleRate = 0;
	PendingNumChannels = 1;
	CurrentSampleIndex = 0;
	SamplesPerChunk = 0;
}

void ULipSyncChunkStreamerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopLipSyncStreaming();
	Super::EndPlay(EndPlayReason);
}