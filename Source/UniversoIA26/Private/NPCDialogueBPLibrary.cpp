#include "NPCDialogueBPLibrary.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

bool UNPCDialogueBPLibrary::ParseAssistantJsonReply(
	const FString& JsonString,
	FString& OutReply,
	ENPCEmotion& OutEmotion)
{
	OutReply.Empty();
	OutEmotion = ENPCEmotion::Neutral;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}

	if (!JsonObject->TryGetStringField(TEXT("reply"), OutReply))
	{
		return false;
	}

	FString EmotionString = TEXT("Neutral");
	JsonObject->TryGetStringField(TEXT("emotion"), EmotionString);
	OutEmotion = EmotionFromString(EmotionString);

	return true;
}

ENPCEmotion UNPCDialogueBPLibrary::EmotionFromString(const FString& InEmotion)
{
	const FString Value = InEmotion.ToLower();

	if (Value == TEXT("happy")) return ENPCEmotion::Happy;
	if (Value == TEXT("concerned")) return ENPCEmotion::Concerned;
	if (Value == TEXT("angry")) return ENPCEmotion::Angry;
	if (Value == TEXT("stare")) return ENPCEmotion::Stare;

	return ENPCEmotion::Neutral;
}

FString UNPCDialogueBPLibrary::EmotionToString(ENPCEmotion InEmotion)
{
	switch (InEmotion)
	{
	case ENPCEmotion::Happy: return TEXT("Happy");
	case ENPCEmotion::Concerned: return TEXT("Concerned");
	case ENPCEmotion::Angry: return TEXT("Angry");
	case ENPCEmotion::Stare: return TEXT("Stare");
	default: return TEXT("Neutral");
	}
}