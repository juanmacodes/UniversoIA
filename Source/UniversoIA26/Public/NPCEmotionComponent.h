#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCDialogueTypes.h"
#include "NPCEmotionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEmotionChanged, ENPCEmotion, NewEmotion);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNIVERSOIA26_API UNPCEmotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Emotion")
	void SetEmotion(ENPCEmotion NewEmotion);

	UFUNCTION(BlueprintPure, Category = "Emotion")
	ENPCEmotion GetEmotion() const { return CurrentEmotion; }

	UPROPERTY(BlueprintAssignable)
	FOnEmotionChanged OnEmotionChanged;

protected:
	UPROPERTY(BlueprintReadOnly)
	ENPCEmotion CurrentEmotion = ENPCEmotion::Neutral;
};