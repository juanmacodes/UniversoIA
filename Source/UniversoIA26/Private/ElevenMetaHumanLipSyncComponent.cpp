// ElevenMetaHumanLipSyncComponent.cpp

#include "ElevenMetaHumanLipSyncComponent.h"
#include "MetaHumanFaceDriverAnimInstance.h"

#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "TimerManager.h"

UElevenMetaHumanLipSyncComponent::UElevenMetaHumanLipSyncComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UElevenMetaHumanLipSyncComponent::BeginPlay()
{
	Super::BeginPlay();
	DetectFaceMeshInternal();
}

void UElevenMetaHumanLipSyncComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bClipActive && EnvelopeSamples.Num() > 0)
	{
		PlaybackTimeSec += DeltaTime;
		PlaybackTimeSec = FMath::Min(PlaybackTimeSec, ClipDurationSec);
		TargetTalkAmount = GetEnvelopeValueAtTime(PlaybackTimeSec);
	}
	else
	{
		TargetTalkAmount = 0.0f;
	}

	const float InterpSpeed = (TargetTalkAmount > TalkAmount) ? AttackSpeed : ReleaseSpeed;
	TalkAmount = FMath::FInterpTo(TalkAmount, TargetTalkAmount, DeltaTime, InterpSpeed);
	TalkAmount = FMath::Clamp(TalkAmount, 0.0f, 1.0f);

	bIsTalking = (TalkAmount > SilenceThreshold);

	const float TimeSec = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float Noise01 = FMath::Clamp(FMath::PerlinNoise1D(TimeSec * 3.0f) * 0.5f + 0.5f, 0.0f, 1.0f);

	JawOpenAmount = FMath::Clamp(TalkAmount * JawCurveScale, 0.0f, 1.0f);
	MouthOpenAmount = FMath::Clamp(TalkAmount * MouthCurveScale + (Noise01 * 0.03f), 0.0f, 1.0f);
	MouthNarrowAmount = FMath::Clamp(TalkAmount * MouthNarrowScale + ((1.0f - TalkAmount) * 0.03f), 0.0f, 1.0f);
	HeadBobAmount = FMath::Clamp(TalkAmount * HeadBobScale, 0.0f, 1.0f);

	ApplyFaceDrive();
}

void UElevenMetaHumanLipSyncComponent::ResetProceduralAudio()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ClipFinishedTimerHandle);
	}

	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}

	ProceduralWave = nullptr;
	EnvelopeSamples.Reset();

	PlaybackTimeSec = 0.0f;
	ClipDurationSec = 0.0f;
	bClipActive = false;

	TargetTalkAmount = 0.0f;
	TalkAmount = 0.0f;
	JawOpenAmount = 0.0f;
	MouthOpenAmount = 0.0f;
	MouthNarrowAmount = 0.0f;
	HeadBobAmount = 0.0f;
	bIsTalking = false;

	bInitialized = false;
}

void UElevenMetaHumanLipSyncComponent::InitializeLipSync(UAudioComponent* InAudioComponent, int32 InSampleRate, int32 InNumChannels)
{
	AudioComponent = InAudioComponent;
	SampleRate = FMath::Max(8000, InSampleRate);
	NumChannels = FMath::Max(1, InNumChannels);

	ResetProceduralAudio();
	EnsureProceduralWave();
	DetectFaceMeshInternal();

	UE_LOG(LogTemp, Warning, TEXT("[LipSync] Init manual: %d Hz, %d ch"), SampleRate, NumChannels);

	bInitialized = (AudioComponent != nullptr && ProceduralWave != nullptr);
}

void UElevenMetaHumanLipSyncComponent::InitializeLipSyncFromSound(UAudioComponent* InAudioComponent, USoundWave* InSoundWave)
{
	AudioComponent = InAudioComponent;

	if (InSoundWave)
	{
		SampleRate = FMath::Max(8000, FMath::RoundToInt(InSoundWave->GetSampleRateForCurrentPlatform()));
		NumChannels = FMath::Max(1, InSoundWave->NumChannels);
	}
	else
	{
		SampleRate = 24000;
		NumChannels = 1;
	}

	ResetProceduralAudio();
	EnsureProceduralWave();
	DetectFaceMeshInternal();

	UE_LOG(LogTemp, Warning, TEXT("[LipSync] Init from SoundWave: %d Hz, %d ch"), SampleRate, NumChannels);

	bInitialized = (AudioComponent != nullptr && ProceduralWave != nullptr);
}

void UElevenMetaHumanLipSyncComponent::EnsureProceduralWave()
{
	if (ProceduralWave)
	{
		return;
	}

	ProceduralWave = NewObject<USoundWaveProcedural>(this);
	if (!ProceduralWave)
	{
		return;
	}

	ProceduralWave->SetSampleRate(SampleRate);
	ProceduralWave->NumChannels = NumChannels;
	ProceduralWave->Duration = INDEFINITELY_LOOPING_DURATION;
	ProceduralWave->bLooping = false;
	ProceduralWave->SoundGroup = SOUNDGROUP_Voice;

	if (AudioComponent)
	{
		AudioComponent->SetSound(ProceduralWave);
	}
}

void UElevenMetaHumanLipSyncComponent::ConsumePCMBytes(const TArray<uint8>& InPCMBytes)
{
	if (InPCMBytes.IsEmpty())
	{
		return;
	}

	// Cada llamada representa un clip nuevo.
	ResetProceduralAudio();
	EnsureProceduralWave();
	DetectFaceMeshInternal();

	bInitialized = (AudioComponent != nullptr && ProceduralWave != nullptr);
	if (!bInitialized)
	{
		return;
	}

	ProceduralWave->QueueAudio(InPCMBytes.GetData(), InPCMBytes.Num());

	BuildEnvelopeFromPCM16(InPCMBytes);

	const int32 BytesPerSample = 2; // PCM16
	const int32 BytesPerFrame = FMath::Max(1, NumChannels * BytesPerSample);
	ClipDurationSec = (SampleRate > 0 && BytesPerFrame > 0)
		? static_cast<float>(InPCMBytes.Num()) / static_cast<float>(SampleRate * BytesPerFrame)
		: 0.0f;

	PlaybackTimeSec = 0.0f;
	bClipActive = true;

	if (AudioComponent)
	{
		AudioComponent->Play();
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ClipFinishedTimerHandle,
			this,
			&UElevenMetaHumanLipSyncComponent::HandleClipFinished,
			ClipDurationSec + 0.05f,
			false
		);
	}

	UE_LOG(LogTemp, Warning, TEXT("[LipSync] Clip queued: duration=%.3f sec, envelopeSamples=%d"), ClipDurationSec, EnvelopeSamples.Num());
}

void UElevenMetaHumanLipSyncComponent::BuildEnvelopeFromPCM16(const TArray<uint8>& InPCMBytes)
{
	EnvelopeSamples.Reset();

	if (SampleRate <= 0 || NumChannels <= 0)
	{
		return;
	}

	const int32 TotalSampleCount = InPCMBytes.Num() / sizeof(int16);
	if (TotalSampleCount <= 0)
	{
		return;
	}

	const int32 TotalFrameCount = TotalSampleCount / NumChannels;
	if (TotalFrameCount <= 0)
	{
		return;
	}

	const int16* Samples = reinterpret_cast<const int16*>(InPCMBytes.GetData());

	const int32 WindowFrames = FMath::Max(1, FMath::RoundToInt((EnvelopeWindowMs / 1000.0f) * SampleRate));
	const int32 HopFrames = FMath::Max(1, FMath::RoundToInt((EnvelopeHopMs / 1000.0f) * SampleRate));

	for (int32 StartFrame = 0; StartFrame < TotalFrameCount; StartFrame += HopFrames)
	{
		const int32 EndFrame = FMath::Min(StartFrame + WindowFrames, TotalFrameCount);

		double SumSquares = 0.0;
		int32 Count = 0;

		for (int32 FrameIndex = StartFrame; FrameIndex < EndFrame; ++FrameIndex)
		{
			for (int32 ChannelIndex = 0; ChannelIndex < NumChannels; ++ChannelIndex)
			{
				const int32 SampleIndex = FrameIndex * NumChannels + ChannelIndex;
				const float Normalized = static_cast<float>(Samples[SampleIndex]) / 32768.0f;
				SumSquares += static_cast<double>(Normalized * Normalized);
				++Count;
			}
		}

		float Rms = 0.0f;
		if (Count > 0)
		{
			Rms = FMath::Sqrt(static_cast<float>(SumSquares / Count));
		}

		float Env = (Rms - EnvelopeNoiseFloor) / FMath::Max(EnvelopeRange, KINDA_SMALL_NUMBER);
		Env = FMath::Clamp(Env * EnvelopeGain, 0.0f, 1.0f);

		EnvelopeSamples.Add(Env);
	}
}

float UElevenMetaHumanLipSyncComponent::GetEnvelopeValueAtTime(float TimeSec) const
{
	if (EnvelopeSamples.Num() == 0)
	{
		return 0.0f;
	}

	const float HopSec = EnvelopeHopMs / 1000.0f;
	if (HopSec <= 0.0f)
	{
		return EnvelopeSamples.Last();
	}

	const float SamplePosition = TimeSec / HopSec;
	const int32 IndexA = FMath::Clamp(FMath::FloorToInt(SamplePosition), 0, EnvelopeSamples.Num() - 1);
	const int32 IndexB = FMath::Clamp(IndexA + 1, 0, EnvelopeSamples.Num() - 1);

	const float Alpha = FMath::Frac(SamplePosition);
	return FMath::Lerp(EnvelopeSamples[IndexA], EnvelopeSamples[IndexB], Alpha);
}

void UElevenMetaHumanLipSyncComponent::HandleClipFinished()
{
	bClipActive = false;
	ClipDurationSec = 0.0f;
	TargetTalkAmount = 0.0f;

	if (AudioComponent && AudioComponent->IsPlaying())
	{
		AudioComponent->Stop();
	}
}

bool UElevenMetaHumanLipSyncComponent::ForceDetectFaceMesh()
{
	return DetectFaceMeshInternal();
}

bool UElevenMetaHumanLipSyncComponent::DetectFaceMeshInternal()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	TArray<USkeletalMeshComponent*> Meshes;
	Owner->GetComponents<USkeletalMeshComponent>(Meshes);

	int32 BestScore = INDEX_NONE;
	USkeletalMeshComponent* BestMesh = nullptr;

	for (USkeletalMeshComponent* Mesh : Meshes)
	{
		if (!Mesh)
		{
			continue;
		}

		const int32 Score = ScoreFaceMesh(Mesh);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestMesh = Mesh;
		}
	}

	FaceMesh = BestMesh;
	CacheFaceAnim();

	if (FaceMesh)
	{
		UE_LOG(LogTemp, Log, TEXT("[LipSync] Face mesh detectada: %s"), *FaceMesh->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[LipSync] No se detectó Face mesh en %s"), *Owner->GetName());
	return false;
}

int32 UElevenMetaHumanLipSyncComponent::ScoreFaceMesh(USkeletalMeshComponent* Mesh) const
{
	if (!Mesh)
	{
		return INDEX_NONE;
	}

	int32 Score = 0;
	const FString Name = Mesh->GetName().ToLower();

	if (Mesh->GetFName().IsEqual(PreferredFaceComponentName))
	{
		Score += 100;
	}

	for (const FName& Tag : FaceTags)
	{
		if (Mesh->ComponentHasTag(Tag))
		{
			Score += 80;
		}
	}

	if (Name.Contains(TEXT("face")))
	{
		Score += 60;
	}
	if (Name.Contains(TEXT("head")))
	{
		Score += 30;
	}

	for (const FName& BoneName : CandidateHeadBones)
	{
		if (Mesh->GetBoneIndex(BoneName) != INDEX_NONE)
		{
			Score += 10;
			break;
		}
	}

	return Score;
}

void UElevenMetaHumanLipSyncComponent::CacheFaceAnim()
{
	FaceDriverAnim = nullptr;

	if (!FaceMesh)
	{
		return;
	}

	if (UAnimInstance* Anim = FaceMesh->GetAnimInstance())
	{
		FaceDriverAnim = Cast<UMetaHumanFaceDriverAnimInstance>(Anim);
	}
}

void UElevenMetaHumanLipSyncComponent::ApplyFaceDrive()
{
	if (!FaceMesh)
	{
		DetectFaceMeshInternal();
		if (!FaceMesh)
		{
			return;
		}
	}

	if (!FaceDriverAnim)
	{
		CacheFaceAnim();
	}

	if (bDriveAnimInstance && FaceDriverAnim)
	{
		FaceDriverAnim->TalkAmount = TalkAmount;
		FaceDriverAnim->JawOpenAmount = JawOpenAmount;
		FaceDriverAnim->MouthOpenAmount = MouthOpenAmount;
		FaceDriverAnim->MouthNarrowAmount = MouthNarrowAmount;
		FaceDriverAnim->HeadBobAmount = HeadBobAmount;
		FaceDriverAnim->bIsTalking = bIsTalking;
		FaceDriverAnim->BlinkAlpha = BlinkAlpha;
	}

	// Solo como fallback. Si ya mueves curvas en el AnimBP, déjalo en false.
	if (bUseMorphTargetFallback)
	{
		for (const FName& Morph : JawMorphTargets)
		{
			FaceMesh->SetMorphTarget(Morph, JawOpenAmount * JawMorphScale, false);
		}

		for (const FName& Morph : MouthMorphTargets)
		{
			FaceMesh->SetMorphTarget(Morph, MouthOpenAmount * MouthMorphScale, false);
		}
	}
}