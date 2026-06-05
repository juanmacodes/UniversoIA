#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NPCDialogueTypes.h"
#include "NPCDialogueBPLibrary.generated.h"

UCLASS()
class UNIVERSOIA26_API UNPCDialogueBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "NPC|Dialogue")
	static bool ParseAssistantJsonReply(
		const FString& JsonString,
		FString& OutReply,
		ENPCEmotion& OutEmotion
	);

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	static ENPCEmotion EmotionFromString(const FString& InEmotion);

	UFUNCTION(BlueprintPure, Category = "NPC|Dialogue")
	static FString EmotionToString(ENPCEmotion InEmotion);
};