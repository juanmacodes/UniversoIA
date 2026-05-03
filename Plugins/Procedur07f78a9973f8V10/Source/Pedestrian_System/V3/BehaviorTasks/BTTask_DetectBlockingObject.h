//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BTTask_DetectBlockingObject.generated.h"



/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBTTask_DetectBlockingObject : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How Far should the AI look to find the next pathpoint."))
    float TraceDistance = 75;
    	
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="How wide should the trace used to find the next pathpoint be."))
    float SphereRadius = 35.0;
    	
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Tracing, meta=(ToolTip="Whether to show the debug of the traces for pathfinding. Note: Will impact performance when enabled"))
    TEnumAsByte<EDrawDebugTrace::Type> DebugDrawTypes;
    	
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="Blackboard vector value used to send data from this node to something like the MoveTo Node."))
    FBlackboardKeySelector MoveToVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="This determines what objects can be traced against, usually set to pawns."))
	TArray<TEnumAsByte<ECollisionChannel>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PathFinding, meta=(ToolTip="This determines how far to move to the side when avoiding."))
	float AvoidanceAmount = 250.0;
    
    
    
    
    private:
    	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};




