//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "Math/Color.h"
#include "BTTask_FindRandomPathPointV3.generated.h"



/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_FindRandomPathPointV3 : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
    	
    	
    	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Whether to show the debug of the traces for pathfinding. Note: Will impact performance when enabled"))
    	TEnumAsByte<EDrawDebugTrace::Type> DebugDrawTypes;
    
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tracing, meta=(ToolTip="trace collision channel to filter for, used to find pathpoints and not every object in the world."))
		TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;
    	
    	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
    	FBlackboardKeySelector NextWaypointVector;
    
    	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The size of the radius that the AI will look for a pathpoint in."))
    	float PathpointRadius = 1800;
    	
    	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="The tag of the waypoint to filter."))
		FName ObjectFilterTag = FName("PathPoint");
    
    
    private:
    	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
