//  Copyright (c) 2025 KomodoBit Games. All rights reserved.


#include "BTTask_DetectBlockingObject.h"
#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "Math/Color.h"
#include "Logging/LogMacros.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/HitResult.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"



EBTNodeResult::Type UBTTask_DetectBlockingObject::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GetWorld())
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	//Start FVector from Controlled Pawn (half height offset)
	const FVector Start = ControlledPawn->GetActorLocation() + FVector(0, 0, ControlledPawn->GetDefaultHalfHeight() / 2.0f);

	// End FVector in front of pawn
	const FVector End = Start + ControlledPawn->GetActorForwardVector() * TraceDistance;

	// Ignore self
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(ControlledPawn);

	// Hit result
	FHitResult Hit;

	// Convert collision channels to object types
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToQuery;
	for (ECollisionChannel Channel : ObjectTypes)
	{
		ObjectTypesToQuery.Add(UEngineTypes::ConvertToObjectType(Channel));
	}

	// Sphere trace
	bool bDidHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(), Start, End, SphereRadius, ObjectTypesToQuery,
		false, ActorsToIgnore, DebugDrawTypes, Hit, true,
		FLinearColor::Red, FLinearColor::Green, 1.0f);

	if (bDidHit)
	{
		// Pick left or right
		const bool bGoRight = FMath::RandBool();
		const FVector Direction = bGoRight
			? ControlledPawn->GetActorRightVector()
			: -ControlledPawn->GetActorRightVector();

		const FVector NewMovementLocation = ControlledPawn->GetActorLocation() + Direction * AvoidanceAmount;

		// Blackboard guard
		if (OwnerComp.GetBlackboardComponent())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(MoveToVector.SelectedKeyName, NewMovementLocation);
			return EBTNodeResult::Succeeded;
		}

		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
