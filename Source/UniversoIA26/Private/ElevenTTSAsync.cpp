#include "ElevenTTSAsync.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Misc/Base64.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "GenericPlatform/GenericPlatformHttp.h"

UElevenTTSAsync* UElevenTTSAsync::ElevenTTSAsync(
	UObject* InWorldContextObject,
	const FString& InApiKey,
	const FString& InVoiceId,
	const FString& InText,
	const FString& InModelId,
	const FString& InOutputFormat,
	int32 InNumChannels,
	float InStability,
	float InSimilarityBoost,
	float InStyle,
	bool bInUseSpeakerBoost,
	bool bInEnableLogging
)
{
	UElevenTTSAsync* Node = NewObject<UElevenTTSAsync>();
	Node->WorldContextObject = InWorldContextObject;
	Node->ApiKey = InApiKey;
	Node->VoiceId = InVoiceId;
	Node->Text = InText;
	Node->ModelId = InModelId;
	Node->OutputFormat = InOutputFormat;
	Node->NumChannels = InNumChannels;
	Node->Stability = InStability;
	Node->SimilarityBoost = InSimilarityBoost;
	Node->Style = InStyle;
	Node->bUseSpeakerBoost = bInUseSpeakerBoost;
	Node->bEnableLogging = bInEnableLogging;
	return Node;
}

void UElevenTTSAsync::Activate()
{
	if (ApiKey.IsEmpty())
	{
		BroadcastError(TEXT("ElevenLabs API key vacía."));
		return;
	}

	if (VoiceId.IsEmpty())
	{
		BroadcastError(TEXT("VoiceId vacío."));
		return;
	}

	if (Text.IsEmpty())
	{
		BroadcastError(TEXT("Texto vacío."));
		return;
	}

	if (!OutputFormat.StartsWith(TEXT("pcm_")))
	{
		BroadcastError(TEXT("Este nodo espera output_format PCM, por ejemplo pcm_44100 o pcm_24000."));
		return;
	}

	const int32 SampleRate = ParseSampleRateFromOutputFormat(OutputFormat);
	if (SampleRate <= 0)
	{
		BroadcastError(FString::Printf(TEXT("No se pudo extraer SampleRate de output_format: %s"), *OutputFormat));
		return;
	}

	const FString EncodedVoiceId = FGenericPlatformHttp::UrlEncode(VoiceId);
	const FString EncodedOutputFormat = FGenericPlatformHttp::UrlEncode(OutputFormat);
	const FString LoggingValue = bEnableLogging ? TEXT("true") : TEXT("false");

	const FString Url = FString::Printf(
		TEXT("https://api.elevenlabs.io/v1/text-to-speech/%s?output_format=%s&enable_logging=%s"),
		*EncodedVoiceId,
		*EncodedOutputFormat,
		*LoggingValue
	);

	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetStringField(TEXT("text"), Text);

	if (!ModelId.IsEmpty())
	{
		RootObject->SetStringField(TEXT("model_id"), ModelId);
	}

	const bool bHasNumericVoiceOverrides =
		(Stability >= 0.0f) ||
		(SimilarityBoost >= 0.0f) ||
		(Style >= 0.0f);

	const bool bNeedsVoiceSettings = bHasNumericVoiceOverrides || !bUseSpeakerBoost;

	if (bNeedsVoiceSettings)
	{
		TSharedPtr<FJsonObject> VoiceSettings = MakeShared<FJsonObject>();

		if (Stability >= 0.0f)
		{
			VoiceSettings->SetNumberField(TEXT("stability"), Stability);
		}

		if (SimilarityBoost >= 0.0f)
		{
			VoiceSettings->SetNumberField(TEXT("similarity_boost"), SimilarityBoost);
		}

		if (Style >= 0.0f)
		{
			VoiceSettings->SetNumberField(TEXT("style"), Style);
		}

		VoiceSettings->SetBoolField(TEXT("use_speaker_boost"), bUseSpeakerBoost);
		RootObject->SetObjectField(TEXT("voice_settings"), VoiceSettings);
	}

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	if (!FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer))
	{
		BroadcastError(TEXT("No se pudo serializar el JSON de la peticion."));
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Url);
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("xi-api-key"), ApiKey);
	Request->SetHeader(TEXT("Accept"), TEXT("*/*"));
	Request->SetContentAsString(RequestBody);

	Request->OnProcessRequestComplete().BindUObject(this, &UElevenTTSAsync::HandleHttpResponse);

	if (!Request->ProcessRequest())
	{
		BroadcastError(TEXT("No se pudo lanzar la petición HTTP a ElevenLabs."));
	}
}

void UElevenTTSAsync::HandleHttpResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		BroadcastError(TEXT("La petición a ElevenLabs falló o no devolvió respuesta válida."));
		return;
	}

	const int32 ResponseCode = Response->GetResponseCode();
	if (ResponseCode < 200 || ResponseCode >= 300)
	{
		const FString ErrorBody = Response->GetContentAsString();
		BroadcastError(FString::Printf(TEXT("ElevenLabs devolvió HTTP %d: %s"), ResponseCode, *ErrorBody));
		return;
	}

	const TArray<uint8>& AudioBytes = Response->GetContent();
	if (AudioBytes.Num() == 0)
	{
		BroadcastError(TEXT("ElevenLabs devolvió audio vacío."));
		return;
	}

	const int32 SampleRate = ParseSampleRateFromOutputFormat(OutputFormat);
	if (SampleRate <= 0)
	{
		BroadcastError(TEXT("SampleRate inválido al procesar la respuesta."));
		return;
	}

	TArray<float> FloatSamples;
	if (!ConvertPCM16BytesToFloatSamples(AudioBytes, FloatSamples))
	{
		BroadcastError(TEXT("No se pudieron convertir los bytes PCM16 a float samples."));
		return;
	}

	CreatedSoundWave = CreateProceduralSoundWaveFromPCM16(AudioBytes, SampleRate, NumChannels);
	if (!CreatedSoundWave)
	{
		BroadcastError(TEXT("No se pudo crear USoundWaveProcedural."));
		return;
	}

	FElevenTTSResult Result;
	Result.PCMBytes = AudioBytes;
	Result.FloatSamples = MoveTemp(FloatSamples);
	Result.SampleRate = SampleRate;
	Result.NumChannels = NumChannels;
	Result.SoundWave = CreatedSoundWave;

	OnSuccess.Broadcast(Result);
	SetReadyToDestroy();
}

void UElevenTTSAsync::BroadcastError(const FString& Message)
{
	OnError.Broadcast(Message);
	SetReadyToDestroy();
}

int32 UElevenTTSAsync::ParseSampleRateFromOutputFormat(const FString& InOutputFormat)
{
	TArray<FString> Parts;
	InOutputFormat.ParseIntoArray(Parts, TEXT("_"), true);

	if (Parts.Num() < 2)
	{
		return 0;
	}

	return FCString::Atoi(*Parts[1]);
}

bool UElevenTTSAsync::ConvertPCM16BytesToFloatSamples(const TArray<uint8>& InBytes, TArray<float>& OutFloatSamples)
{
	OutFloatSamples.Reset();

	if (InBytes.Num() == 0 || (InBytes.Num() % 2) != 0)
	{
		return false;
	}

	const int32 NumSamples = InBytes.Num() / 2;
	OutFloatSamples.SetNumUninitialized(NumSamples);

	const int16* PCM16 = reinterpret_cast<const int16*>(InBytes.GetData());

	for (int32 i = 0; i < NumSamples; ++i)
	{
		OutFloatSamples[i] = FMath::Clamp(static_cast<float>(PCM16[i]) / 32768.0f, -1.0f, 1.0f);
	}

	return true;
}

USoundWaveProcedural* UElevenTTSAsync::CreateProceduralSoundWaveFromPCM16(const TArray<uint8>& InBytes, int32 SampleRate, int32 InNumChannels)
{
	if (InBytes.Num() == 0 || (InBytes.Num() % 2) != 0 || SampleRate <= 0 || InNumChannels <= 0)
	{
		return nullptr;
	}

	USoundWaveProcedural* Wave = NewObject<USoundWaveProcedural>(GetTransientPackage());
	if (!Wave)
	{
		return nullptr;
	}

	Wave->SetSampleRate(SampleRate);
	Wave->NumChannels = InNumChannels;
	Wave->Duration = static_cast<float>(InBytes.Num() / 2) / static_cast<float>(SampleRate * InNumChannels);
	Wave->SoundGroup = SOUNDGROUP_Voice;
	Wave->bLooping = false;
	Wave->QueueAudio(InBytes.GetData(), InBytes.Num());

	return Wave;
}