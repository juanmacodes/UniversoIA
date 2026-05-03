//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BTTask_Wander.generated.h"

/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_Wander : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
		FBlackboardKeySelector RandomLocation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PathFinding, meta=(ToolTip="The size of the radius that the AI will move in."))
		float WanderRadius;

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
