#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NPCDialogueTypes.h"
#include "ConversationComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNIVERSOIA26_API UConversationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void AddUserMessage(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void AddAssistantMessage(const FString& Text, ENPCEmotion Emotion);

	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void AddSystemMessage(const FString& Text);

	UFUNCTION(BlueprintCallable, Category = "Conversation")
	void ClearConversation();

	UFUNCTION(BlueprintPure, Category = "Conversation")
	const TArray<FChatLine>& GetConversation() const { return Messages; }

	UFUNCTION(BlueprintCallable, Category = "Conversation")
	TArray<FChatLine> GetLastMessages(int32 MaxCount = 10) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FChatLine> Messages;
};