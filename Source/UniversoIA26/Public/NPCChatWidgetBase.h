#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCDialogueTypes.h"
#include "NPCChatWidgetBase.generated.h"

UCLASS()
class UNIVERSOIA26_API UNPCChatWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddChatLine(const FChatLine& Line);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void ClearChatLines();

	UFUNCTION(BlueprintPure, Category = "Chat")
	const TArray<FChatLine>& GetDisplayedLines() const { return DisplayedLines; }

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FChatLine> DisplayedLines;

	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
	void BP_OnChatLineAdded(const FChatLine& Line);

	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
	void BP_OnChatCleared();
};