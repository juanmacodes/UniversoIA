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

	float SampledMBP = 0.0f;
	float SampledFV = 0.0f;
	float SampledOO = 0.0f;
	float SampledEE = 0.0f;
	float SampledAA = 0.0f;
	float SampledSH = 0.0f;

	SampleVisemesAtTime(
		PlaybackTimeSec,
		SampledMBP,
		SampledFV,
		SampledOO,
		SampledEE,
		SampledAA,
		SampledSH);

	VisemeMBP = FMath::FInterpTo(VisemeMBP, SampledMBP, DeltaTime, 18.0f);
	VisemeFV = FMath::FInterpTo(VisemeFV, SampledFV, DeltaTime, 18.0f);
	VisemeOO = FMath::FInterpTo(VisemeOO, SampledOO, DeltaTime, 18.0f);
	VisemeEE = FMath::FInterpTo(VisemeEE, SampledEE, DeltaTime, 18.0f);
	VisemeAA = FMath::FInterpTo(VisemeAA, SampledAA, DeltaTime, 18.0f);
	VisemeSH = FMath::FInterpTo(VisemeSH, SampledSH, DeltaTime, 18.0f);

	VisemeMBP = FMath::Clamp(VisemeMBP, 0.0f, 1.0f);
	VisemeFV = FMath::Clamp(VisemeFV, 0.0f, 1.0f);
	VisemeOO = FMath::Clamp(VisemeOO, 0.0f, 1.0f);
	VisemeEE = FMath::Clamp(VisemeEE, 0.0f, 1.0f);
	VisemeAA = FMath::Clamp(VisemeAA, 0.0f, 1.0f);
	VisemeSH = FMath::Clamp(VisemeSH, 0.0f, 1.0f);

	bIsTalking = (TalkAmount > SilenceThreshold);

	const float TimeSec = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float Noise01 = FMath::Clamp(FMath::PerlinNoise1D(TimeSec * 3.0f) * 0.5f + 0.5f, 0.0f, 1.0f);

	// Base por envelope + capa de visemas.
	JawOpenAmount =
		FMath::Clamp(
			(TalkAmount * JawCurveScale) +
			(VisemeAA * VisemeAAScale) +
			(VisemeOO * 0.08f) -
			(VisemeMBP * 0.45f),
			0.0f,
			1.0f);

	MouthOpenAmount =
		FMath::Clamp(
			(TalkAmount * MouthOpenCurveScale) +
			(VisemeAA * 0.20f) +
			(VisemeEE * 0.05f) -
			(VisemeMBP * 0.15f),
			0.0f,
			1.0f);

	MouthNarrowAmount =
		FMath::Clamp(
			(VisemeOO * VisemeOOScale) +
			(VisemeSH * VisemeSHScale) +
			(VisemeFV * 0.05f),
			0.0f,
			1.0f);

	MouthWideAmount =
		FMath::Clamp(
			(VisemeEE * VisemeEEScale) +
			(TalkAmount * 0.04f),
			0.0f,
			1.0f);

	HeadBobAmount =
		FMath::Clamp(
			(TalkAmount * HeadBobScale) + (Noise01 * 0.015f),
			0.0f,
			1.0f);

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
	VisemeTimeline.Reset();

	PlaybackTimeSec = 0.0f;
	ClipDurationSec = 0.0f;
	bClipActive = false;

	TargetTalkAmount = 0.0f;
	TalkAmount = 0.0f;
	JawOpenAmount = 0.0f;
	MouthOpenAmount = 0.0f;
	MouthNarrowAmount = 0.0f;
	MouthWideAmount = 0.0f;
	HeadBobAmount = 0.0f;

	VisemeMBP = 0.0f;
	VisemeFV = 0.0f;
	VisemeOO = 0.0f;
	VisemeEE = 0.0f;
	VisemeAA = 0.0f;
	VisemeSH = 0.0f;

	bIsTalking = false;
	BlinkAlpha = 0.0f;
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

void UElevenMetaHumanLipSyncComponent::SetPendingSpeechText(const FString& InText)
{
	PendingSpeechText = InText;
}

void UElevenMetaHumanLipSyncComponent::ConsumePCMBytesWithText(const TArray<uint8>& InPCMBytes, const FString& InText)
{
	PendingSpeechText = InText;
	ConsumePCMBytes(InPCMBytes);
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

	BuildApproxVisemeTimelineFromText(PendingSpeechText, ClipDurationSec);

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

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[LipSync] Clip queued: duration=%.3f sec, envelopeSamples=%d, visemes=%d"),
		ClipDurationSec,
		EnvelopeSamples.Num(),
		VisemeTimeline.Num());
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

void UElevenMetaHumanLipSyncComponent::BuildApproxVisemeTimelineFromText(const FString& InText, float InDurationSec)
{
	VisemeTimeline.Reset();

	if (InText.IsEmpty() || InDurationSec <= 0.0f)
	{
		return;
	}

	FString Upper = InText.ToUpper();

	TArray<FString> Tokens;
	TArray<float> TokenWeights;

	auto AddToken = [&](const FString& Token, float Weight)
		{
			if (!Token.IsEmpty())
			{
				Tokens.Add(Token);
				TokenWeights.Add(Weight);
			}
		};

	for (int32 i = 0; i < Upper.Len();)
	{
		const TCHAR C = Upper[i];

		if (FChar::IsWhitespace(C))
		{
			AddToken(TEXT(" "), 0.35f);
			++i;
			continue;
		}

		if (FChar::IsPunct(C))
		{
			AddToken(TEXT("|"), 0.30f);
			++i;
			continue;
		}

		const FString Two = (i + 1 < Upper.Len()) ? Upper.Mid(i, 2) : FString();
		if (Two == TEXT("CH") || Two == TEXT("SH") || Two == TEXT("LL") || Two == TEXT("RR") ||
			Two == TEXT("PH") || Two == TEXT("TH") || Two == TEXT("QU") || Two == TEXT("GU"))
		{
			float Weight = 1.0f;
			if (Two == TEXT("CH") || Two == TEXT("SH"))
			{
				Weight = 1.15f;
			}
			else if (Two == TEXT("QU") || Two == TEXT("GU"))
			{
				Weight = 1.20f;
			}

			AddToken(Two, Weight);
			i += 2;
			continue;
		}

		FString OneChar;
		OneChar.AppendChar(C);

		float Weight = 1.0f;
		if (OneChar == TEXT("A") || OneChar == TEXT("E") || OneChar == TEXT("I") || OneChar == TEXT("O") || OneChar == TEXT("U"))
		{
			Weight = 1.25f;
		}
		else if (OneChar == TEXT("M") || OneChar == TEXT("B") || OneChar == TEXT("P"))
		{
			Weight = 1.05f;
		}

		AddToken(OneChar, Weight);
		++i;
	}

	if (Tokens.Num() == 0)
	{
		return;
	}

	float TotalWeight = 0.0f;
	for (float W : TokenWeights)
	{
		TotalWeight += W;
	}

	if (TotalWeight <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	float Cursor = 0.0f;

	for (int32 Index = 0; Index < Tokens.Num(); ++Index)
	{
		const FString& Token = Tokens[Index];
		const float Duration = InDurationSec * (TokenWeights[Index] / TotalWeight);

		FApproxVisemeKey Key;
		Key.Type = DetectVisemeForToken(Token);
		Key.StartTime = Cursor;
		Key.EndTime = Cursor + Duration;

		VisemeTimeline.Add(Key);
		Cursor = Key.EndTime;
	}

	if (VisemeTimeline.Num() > 0)
	{
		VisemeTimeline.Last().EndTime = InDurationSec;
	}
}

UElevenMetaHumanLipSyncComponent::EApproxViseme UElevenMetaHumanLipSyncComponent::DetectVisemeForToken(const FString& Token) const
{
	if (Token.IsEmpty() || Token == TEXT(" ") || Token == TEXT("|"))
	{
		return EApproxViseme::Silence;
	}

	if (Token == TEXT("M") || Token == TEXT("B") || Token == TEXT("P"))
	{
		return EApproxViseme::MBP;
	}

	if (Token == TEXT("F") || Token == TEXT("V") || Token == TEXT("PH"))
	{
		return EApproxViseme::FV;
	}

	if (Token == TEXT("O") || Token == TEXT("U") || Token == TEXT("W") || Token == TEXT("Q") || Token == TEXT("QU") || Token == TEXT("GU"))
	{
		return EApproxViseme::OO;
	}

	if (Token == TEXT("E") || Token == TEXT("I") || Token == TEXT("Y"))
	{
		return EApproxViseme::EE;
	}

	if (Token == TEXT("A"))
	{
		return EApproxViseme::AA;
	}

	if (Token == TEXT("CH") || Token == TEXT("SH") || Token == TEXT("J") || Token == TEXT("X"))
	{
		return EApproxViseme::SH;
	}

	return EApproxViseme::Rest;
}

void UElevenMetaHumanLipSyncComponent::SampleVisemesAtTime(
	float TimeSec,
	float& OutMBP,
	float& OutFV,
	float& OutOO,
	float& OutEE,
	float& OutAA,
	float& OutSH) const
{
	OutMBP = 0.0f;
	OutFV = 0.0f;
	OutOO = 0.0f;
	OutEE = 0.0f;
	OutAA = 0.0f;
	OutSH = 0.0f;

	if (VisemeTimeline.Num() == 0)
	{
		return;
	}

	const float BlendPaddingSec = VisemeBlendPaddingMs / 1000.0f;

	for (const FApproxVisemeKey& Key : VisemeTimeline)
	{
		const float Center = 0.5f * (Key.StartTime + Key.EndTime);
		const float Half = 0.5f * (Key.EndTime - Key.StartTime) + BlendPaddingSec;

		if (Half <= KINDA_SMALL_NUMBER)
		{
			continue;
		}

		const float Distance = FMath::Abs(TimeSec - Center);
		if (Distance > Half)
		{
			continue;
		}

		float Weight = 1.0f - (Distance / Half);
		Weight = FMath::Clamp(Weight, 0.0f, 1.0f);
		Weight = FMath::InterpEaseInOut(0.0f, 1.0f, Weight, 2.0f);

		switch (Key.Type)
		{
		case EApproxViseme::MBP:
			OutMBP = FMath::Max(OutMBP, Weight * VisemeMBPScale);
			break;
		case EApproxViseme::FV:
			OutFV = FMath::Max(OutFV, Weight * VisemeFVScale);
			break;
		case EApproxViseme::OO:
			OutOO = FMath::Max(OutOO, Weight * VisemeOOScale);
			break;
		case EApproxViseme::EE:
			OutEE = FMath::Max(OutEE, Weight * VisemeEEScale);
			break;
		case EApproxViseme::AA:
			OutAA = FMath::Max(OutAA, Weight * VisemeAAScale);
			break;
		case EApproxViseme::SH:
			OutSH = FMath::Max(OutSH, Weight * VisemeSHScale);
			break;
		default:
			break;
		}
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
		FaceDriverAnim->MouthWideAmount = MouthWideAmount;
		FaceDriverAnim->HeadBobAmount = HeadBobAmount;
		FaceDriverAnim->VisemeMBP = VisemeMBP;
		FaceDriverAnim->VisemeFV = VisemeFV;
		FaceDriverAnim->VisemeOO = VisemeOO;
		FaceDriverAnim->VisemeEE = VisemeEE;
		FaceDriverAnim->VisemeAA = VisemeAA;
		FaceDriverAnim->VisemeSH = VisemeSH;
		FaceDriverAnim->bIsTalking = bIsTalking;
		FaceDriverAnim->BlinkAlpha = BlinkAlpha;
	}

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