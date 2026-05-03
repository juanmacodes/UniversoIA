//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BTTask_FindRandomPathPoint.generated.h"

/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_FindRandomPathPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Whether to show the debug of the traces for pathfinding. Note: Will impact performance when enabled"))
	TEnumAsByte<EDrawDebugTrace::Type> DebugDrawTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tracing, meta=(ToolTip="Object collision channel to filter for, used to find pathpoints and not every object in the world."))
	TEnumAsByte<EObjectTypeQuery> ObjectToUse;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
	FBlackboardKeySelector NextWaypointVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The size of the radius that the AI will look for a pathpoint in."))
	float PathpointRadius;



private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
