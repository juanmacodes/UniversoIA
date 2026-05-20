#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Sound/SoundWaveProcedural.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "ElevenTTSAsync.generated.h"

USTRUCT(BlueprintType)
struct FElevenTTSResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Eleven|Audio")
	TArray<uint8> PCMBytes;

	UPROPERTY(BlueprintReadOnly, Category = "Eleven|Audio")
	TArray<float> FloatSamples;

	UPROPERTY(BlueprintReadOnly, Category = "Eleven|Audio")
	int32 SampleRate = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Eleven|Audio")
	int32 NumChannels = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Eleven|Audio")
	TObjectPtr<USoundWaveProcedural> SoundWave = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FElevenTTSSuccess, const FElevenTTSResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FElevenTTSError, const FString&, ErrorMessage);

UCLASS()
class UNIVERSOIA26_API UElevenTTSAsync : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FElevenTTSSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FElevenTTSError OnError;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "Eleven TTS Async"), Category = "Eleven|Audio")
	static UElevenTTSAsync* ElevenTTSAsync(
		UObject* WorldContextObject,
		const FString& ApiKey,
		const FString& VoiceId,
		const FString& Text,
		const FString& ModelId = TEXT("eleven_flash_v2_5"),
		const FString& OutputFormat = TEXT("pcm_44100"),
		int32 NumChannels = 1,
		float Stability = -1.0f,
		float SimilarityBoost = -1.0f,
		float Style = -1.0f,
		bool bUseSpeakerBoost = true,
		bool bEnableLogging = true
	);

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;

	UPROPERTY()
	TObjectPtr<USoundWaveProcedural> CreatedSoundWave = nullptr;

	FString ApiKey;
	FString VoiceId;
	FString Text;
	FString ModelId;
	FString OutputFormat;
	int32 NumChannels = 1;
	float Stability = -1.0f;
	float SimilarityBoost = -1.0f;
	float Style = -1.0f;
	bool bUseSpeakerBoost = true;
	bool bEnableLogging = true;

private:
	void HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void BroadcastError(const FString& Message);

	static int32 ParseSampleRateFromOutputFormat(const FString& InOutputFormat);
	static bool ConvertPCM16BytesToFloatSamples(const TArray<uint8>& InBytes, TArray<float>& OutFloatSamples);
	static USoundWaveProcedural* CreateProceduralSoundWaveFromPCM16(const TArray<uint8>& InBytes, int32 SampleRate, int32 InNumChannels);
};