//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "Math/Color.h"
#include "Logging/LogMacros.h"
#include "BTTask_MoveCharacterAlongSpline.generated.h"


UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_MoveCharacterAlongSpline : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How Far should the AI look to find the next pathpoint."))
	float TraceDistance = 600.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How far ahead of the AI character should the pathfinding trace happen, recommended values are between 225-300."))
	float TraceStartLocation = 225.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How wide should the trace used to find the next pathpoint be."))
	float SphereRadius = 300.0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Whether to show the debug of the traces for pathfinding. Note: Will impact performance when enabled"))
	TEnumAsByte<EDrawDebugTrace::Type> DebugDrawTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tracing, meta=(ToolTip="trace collision channel to filter for, used to find pathpoints and not every object in the world."))
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
	FBlackboardKeySelector MoveToVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Radius around AI to search for next pathpoint when lost/stuck."))
	float RefindPathpointRadius = 1800;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The variation in location when moving to the next pathpoint, NOTE: Only modify X and Y values, leave Z at 0."))
	FVector PathfindingSeparation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The forward offset for the move to vector along the spline."))
	float SplineForwardOffset = 600;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The forward offset for the move to vector along the spline."))
	FName ObjectFilterTag = FName("PathPoint");


private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
	
};
