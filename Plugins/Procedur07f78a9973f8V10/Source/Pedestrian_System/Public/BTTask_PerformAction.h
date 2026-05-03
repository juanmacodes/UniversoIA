//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PerformAction.generated.h"

UENUM(BlueprintType)
enum ActionTypeNPC
{
	NPCAction1 UMETA(DisplayName = "Action 1"),
	NPCAction2 UMETA(DisplayName = "Action 2"),
	NPCAction3 UMETA(DisplayName = "Action 3"),
	NPCAction4 UMETA(DisplayName = "Action 4"),
	NPCAction5 UMETA(DisplayName = "Action 5"),
	NPCAction6 UMETA(DisplayName = "Action 6"),
	NPCAction7 UMETA(DisplayName = "Action 7"),
	NPCAction8 UMETA(DisplayName = "Action 8"),
	NPCAction9 UMETA(DisplayName = "Action 9"),
	NPCAction10 UMETA(DisplayName = "Action 10"),
	NPCAction11 UMETA(DisplayName = "Action 11"),
	NPCAction12 UMETA(DisplayName = "Action 12"),
	NPCAction13 UMETA(DisplayName = "Action 13"),
	NPCAction14 UMETA(DisplayName = "Action 14"),
	NPCAction15 UMETA(DisplayName = "Action 15"),
	NPCAction16 UMETA(DisplayName = "Action 16"),
	NPCAction17 UMETA(DisplayName = "Action 17"),
	NPCAction18 UMETA(DisplayName = "Action 18"),
	NPCAction19 UMETA(DisplayName = "Action 19"),
	NPCAction20 UMETA(DisplayName = "Action 20"),
	NPCAction21 UMETA(DisplayName = "Action 21"),
	NPCAction22 UMETA(DisplayName = "Action 22"),
	NPCAction23 UMETA(DisplayName = "Action 23"),
	NPCAction24 UMETA(DisplayName = "Action 24"),
	NPCAction25 UMETA(DisplayName = "Action 25"),
	NPCAction26 UMETA(DisplayName = "Action 26"),
	NPCAction27 UMETA(DisplayName = "Action 27"),
	NPCAction28 UMETA(DisplayName = "Action 28"),
	NPCAction29 UMETA(DisplayName = "Action 29"),
	NPCAction30 UMETA(DisplayName = "Action 30"),
	NPCAction31 UMETA(DisplayName = "Action 31"),
	NPCAction32 UMETA(DisplayName = "Action 32")
};





UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_PerformAction : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = General, meta=(ToolTip="The event to trigger in the pawn/character BP of the AI."))
	TEnumAsByte<ActionTypeNPC> ActionTypeNPC;
	

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
