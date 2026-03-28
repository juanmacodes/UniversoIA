#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AudioMixerBlueprintLibrary.h"
#include "Sound/SoundWave.h"
#include "Components/AudioComponent.h"
#include "ElevenMetaHumanAudioSourceBridgeComponent.generated.h"

UCLASS(ClassGroup = (MetaHuman), meta = (BlueprintSpawnableComponent))
class UNIVERSOIA26_API UElevenMetaHumanAudioSourceBridgeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UElevenMetaHumanAudioSourceBridgeComponent();

	/** Nombre parcial del endpoint de salida de Unreal.
		Para VB-CABLE suele ser "CABLE Input". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Devices")
	FString PreferredPlaybackDeviceContains = TEXT("CABLE Input");

	/** Subject name que luego pondrás en el MetaHuman. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|MetaHuman")
	FString SubjectName = TEXT("Eleven_Subject");

	/** Audio component que reproducirá el SoundWave de Eleven. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bridge|Playback")
	TObjectPtr<UAudioComponent> TTSAudioComponent = nullptr;

	/** Lanza la búsqueda de outputs y cambia la salida de Unreal al cable virtual. */
	UFUNCTION(BlueprintCallable, Category = "Bridge")
	void InitializeBridge();

	/** Reproduce el SoundWave de Eleven por el output virtual ya seleccionado. */
	UFUNCTION(BlueprintCallable, Category = "Bridge")
	void PlayElevenSound(USoundWave* SoundWave);

	/** Reproduce y, si no está listo, intenta inicializar antes. */
	UFUNCTION(BlueprintCallable, Category = "Bridge")
	void EnsureBridgeAndPlay(USoundWave* SoundWave);

	UFUNCTION(BlueprintPure, Category = "Bridge")
	bool IsOutputReady() const { return bOutputSwapped; }

	UFUNCTION(BlueprintPure, Category = "Bridge")
	FString GetChosenPlaybackDeviceName() const { return ChosenPlaybackDeviceName; }

	UFUNCTION(BlueprintPure, Category = "Bridge")
	FName GetSubjectName() const { return FName(*SubjectName); }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnAudioOutputDevicesObtained(const TArray<FAudioOutputDeviceInfo>& AvailableDevices);

	UFUNCTION()
	void OnCompletedDeviceSwap(const FSwapAudioOutputResult& Result);

	void RequestOutputDeviceSwap();
	int32 FindBestOutputDeviceIndex(const TArray<FAudioOutputDeviceInfo>& AvailableDevices) const;

private:
	UPROPERTY(Transient)
	bool bRequestedDeviceSwap = false;

	UPROPERTY(Transient)
	bool bOutputSwapped = false;

	UPROPERTY(Transient)
	FString ChosenPlaybackDeviceName;

	UPROPERTY(Transient)
	FString ChosenPlaybackDeviceId;
};