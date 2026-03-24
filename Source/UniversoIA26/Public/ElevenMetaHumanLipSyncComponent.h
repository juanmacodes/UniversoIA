// ElevenMetaHumanLipSyncComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundWaveProcedural.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ElevenMetaHumanLipSyncComponent.generated.h"

class UMetaHumanFaceDriverAnimInstance;

UCLASS(ClassGroup = (Audio), meta = (BlueprintSpawnableComponent))
class UNIVERSOIA26_API UElevenMetaHumanLipSyncComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UElevenMetaHumanLipSyncComponent();

	UFUNCTION(BlueprintCallable, Category = "Eleven|LipSync")
	void InitializeLipSync(UAudioComponent* InAudioComponent, int32 InSampleRate = 24000, int32 InNumChannels = 1);

	UFUNCTION(BlueprintCallable, Category = "Eleven|LipSync")
	void InitializeLipSyncFromSound(UAudioComponent* InAudioComponent, USoundWave* InSoundWave);

	UFUNCTION(BlueprintCallable, Category = "Eleven|LipSync")
	void ConsumePCMBytes(const TArray<uint8>& InPCMBytes);

	UFUNCTION(BlueprintCallable, Category = "Eleven|LipSync")
	void ResetProceduralAudio();

	UFUNCTION(BlueprintCallable, Category = "Eleven|LipSync")
	bool ForceDetectFaceMesh();

	UFUNCTION(BlueprintPure, Category = "Eleven|LipSync")
	USkeletalMeshComponent* GetDetectedFaceMesh() const { return FaceMesh; }

	UFUNCTION(BlueprintPure, Category = "Eleven|LipSync")
	float GetTalkAmount() const { return TalkAmount; }

	UFUNCTION(BlueprintPure, Category = "Eleven|LipSync")
	bool IsTalking() const { return bIsTalking; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void EnsureProceduralWave();
	void BuildEnvelopeFromPCM16(const TArray<uint8>& InPCMBytes);
	float GetEnvelopeValueAtTime(float TimeSec) const;
	void HandleClipFinished();

	bool DetectFaceMeshInternal();
	int32 ScoreFaceMesh(USkeletalMeshComponent* Mesh) const;
	void CacheFaceAnim();
	void ApplyFaceDrive();

private:
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(Transient)
	TObjectPtr<USoundWaveProcedural> ProceduralWave;

	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> FaceMesh;

	UPROPERTY(Transient)
	TObjectPtr<UMetaHumanFaceDriverAnimInstance> FaceDriverAnim;

	UPROPERTY(EditAnywhere, Category = "Detection")
	FName PreferredFaceComponentName = TEXT("Face");

	UPROPERTY(EditAnywhere, Category = "Detection")
	TArray<FName> FaceTags = { TEXT("MetaHumanFace"), TEXT("Face") };

	UPROPERTY(EditAnywhere, Category = "Detection")
	TArray<FName> CandidateHeadBones = {
		TEXT("head"),
		TEXT("Head"),
		TEXT("FACIAL_C_FacialRoot"),
		TEXT("neck_01")
	};

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float SilenceThreshold = 0.02f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float AttackSpeed = 16.0f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float ReleaseSpeed = 14.0f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float EnvelopeWindowMs = 20.0f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float EnvelopeHopMs = 10.0f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float EnvelopeNoiseFloor = 0.004f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float EnvelopeRange = 0.08f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Audio")
	float EnvelopeGain = 1.25f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	bool bDriveAnimInstance = true;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	bool bUseMorphTargetFallback = false;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float JawCurveScale = 1.25f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float MouthCurveScale = 0.55f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float MouthNarrowScale = 0.18f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float HeadBobScale = 0.10f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	TArray<FName> JawMorphTargets = {
		TEXT("jawOpen"),
		TEXT("CTRL_expressions_jawOpen")
	};

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	TArray<FName> MouthMorphTargets = {
		TEXT("mouthOpen"),
		TEXT("CTRL_expressions_mouthOpen")
	};

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float JawMorphScale = 1.0f;

	UPROPERTY(EditAnywhere, Category = "LipSync|Face")
	float MouthMorphScale = 0.65f;

	int32 SampleRate = 24000;
	int32 NumChannels = 1;

	UPROPERTY(Transient)
	TArray<float> EnvelopeSamples;

	float PlaybackTimeSec = 0.0f;
	float ClipDurationSec = 0.0f;
	bool bClipActive = false;

	float TargetTalkAmount = 0.0f;
	float TalkAmount = 0.0f;
	float JawOpenAmount = 0.0f;
	float MouthOpenAmount = 0.0f;
	float MouthNarrowAmount = 0.0f;
	float HeadBobAmount = 0.0f;

	bool bIsTalking = false;
	float BlinkAlpha = 0.0f;
	bool bInitialized = false;

	FTimerHandle ClipFinishedTimerHandle;
};