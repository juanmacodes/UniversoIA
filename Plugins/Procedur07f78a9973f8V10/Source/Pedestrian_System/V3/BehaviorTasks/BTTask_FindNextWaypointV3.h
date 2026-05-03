//  Copyright (c) 2025 KomodoBit Games. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "Math/Color.h"
#include "BTTask_FindNextWaypointV3.generated.h"

/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_FindNextWaypointV3 : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How Far should the AI look to find the next pathpoint."))
	float TraceDistance = 500.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How wide should the trace used to find the next pathpoint be."))
	float SphereRadius = 320.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Whether to show the debug of the traces for pathfinding. Note: Will impact performance when enabled"))
	TEnumAsByte<EDrawDebugTrace::Type> DebugDrawTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tracing, meta=(ToolTip="trace collision channel to filter for, used to find pathpoints and not every object in the world."))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
	FBlackboardKeySelector NextWaypointVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Radius around AI to search for next pathpoint when lost/stuck."))
	float RefindPathpointRadius = 1800.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The variation in location when moving to the next pathpoint, NOTE: Only modify X and Y values, leave Z at 0."))
	FVector PathfindingSeparation = FVector(150.0f, 150.0f, 0.0f);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The variation in location when moving to the next pathpoint, NOTE: Only modify X and Y values, leave Z at 0."))
	FName ObjectFilterTag = FName("PathPoint");

	
	


private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
