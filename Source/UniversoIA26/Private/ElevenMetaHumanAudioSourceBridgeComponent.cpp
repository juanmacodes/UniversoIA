#include "ElevenMetaHumanAudioSourceBridgeComponent.h"

#include "Engine/World.h"
#include "Engine/Engine.h"

UElevenMetaHumanAudioSourceBridgeComponent::UElevenMetaHumanAudioSourceBridgeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UElevenMetaHumanAudioSourceBridgeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UElevenMetaHumanAudioSourceBridgeComponent::InitializeBridge()
{
	RequestOutputDeviceSwap();
}

void UElevenMetaHumanAudioSourceBridgeComponent::EnsureBridgeAndPlay(USoundWave* SoundWave)
{
	if (!bOutputSwapped)
	{
		InitializeBridge();
	}

	PlayElevenSound(SoundWave);
}

void UElevenMetaHumanAudioSourceBridgeComponent::RequestOutputDeviceSwap()
{
	if (bRequestedDeviceSwap)
	{
		return;
	}

	FOnAudioOutputDevicesObtained DevicesObtained;
	DevicesObtained.BindDynamic(this, &UElevenMetaHumanAudioSourceBridgeComponent::OnAudioOutputDevicesObtained);

	UAudioMixerBlueprintLibrary::GetAvailableAudioOutputDevices(this, DevicesObtained);
	bRequestedDeviceSwap = true;
}

int32 UElevenMetaHumanAudioSourceBridgeComponent::FindBestOutputDeviceIndex(const TArray<FAudioOutputDeviceInfo>& AvailableDevices) const
{
	const FString Wanted = PreferredPlaybackDeviceContains.ToLower();

	for (int32 i = 0; i < AvailableDevices.Num(); ++i)
	{
		const FString Name = AvailableDevices[i].Name.ToLower();
		if (Name.Contains(Wanted))
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UElevenMetaHumanAudioSourceBridgeComponent::OnAudioOutputDevicesObtained(const TArray<FAudioOutputDeviceInfo>& AvailableDevices)
{
	const int32 DeviceIndex = FindBestOutputDeviceIndex(AvailableDevices);
	if (DeviceIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("[MHAudioBridge] No encontré un output device que contenga '%s'."), *PreferredPlaybackDeviceContains);
		return;
	}

	const FAudioOutputDeviceInfo& DeviceInfo = AvailableDevices[DeviceIndex];
	ChosenPlaybackDeviceName = DeviceInfo.Name;
	ChosenPlaybackDeviceId = DeviceInfo.DeviceId;

	UE_LOG(LogTemp, Warning, TEXT("[MHAudioBridge] Cambiando salida de Unreal a: %s"), *ChosenPlaybackDeviceName);

	FOnCompletedDeviceSwap DeviceSwapDelegate;
	DeviceSwapDelegate.BindDynamic(this, &UElevenMetaHumanAudioSourceBridgeComponent::OnCompletedDeviceSwap);

	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(this, ChosenPlaybackDeviceId, DeviceSwapDelegate);
}

void UElevenMetaHumanAudioSourceBridgeComponent::OnCompletedDeviceSwap(const FSwapAudioOutputResult& Result)
{
	bOutputSwapped = true;

	UE_LOG(LogTemp, Warning, TEXT("[MHAudioBridge] Output swap completado."));
}

void UElevenMetaHumanAudioSourceBridgeComponent::PlayElevenSound(USoundWave* SoundWave)
{
	if (!TTSAudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[MHAudioBridge] TTSAudioComponent no está asignado."));
		return;
	}

	if (!SoundWave)
	{
		UE_LOG(LogTemp, Error, TEXT("[MHAudioBridge] SoundWave es null."));
		return;
	}

	if (!bOutputSwapped)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MHAudioBridge] Aún no he cambiado el output device. El audio puede salir al endpoint equivocado."));
	}

	TTSAudioComponent->Stop();
	TTSAudioComponent->SetSound(SoundWave);
	TTSAudioComponent->Play();

	UE_LOG(LogTemp, Warning, TEXT("[MHAudioBridge] Reproduciendo Eleven SoundWave por '%s'."), *ChosenPlaybackDeviceName);
}