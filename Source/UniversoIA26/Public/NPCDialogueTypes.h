#pragma once

#include "CoreMinimal.h"
#include "NPCDialogueTypes.generated.h"

UENUM(BlueprintType)
enum class ENPCEmotion : uint8
{
	Neutral     UMETA(DisplayName = "Neutral"),
	Happy       UMETA(DisplayName = "Happy"),
	Concerned   UMETA(DisplayName = "Concerned"),
	Angry       UMETA(DisplayName = "Angry"),
	Stare       UMETA(DisplayName = "Stare")
};

UENUM(BlueprintType)
enum class EChatRole : uint8
{
	System      UMETA(DisplayName = "System"),
	User        UMETA(DisplayName = "User"),
	Assistant   UMETA(DisplayName = "Assistant")
};

USTRUCT(BlueprintType)
struct FChatLine
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EChatRole Role = EChatRole::User;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Text;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ENPCEmotion Emotion = ENPCEmotion::Neutral;
};