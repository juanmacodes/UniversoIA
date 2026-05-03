//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_FindRandomPathPointV3.h"
#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pedestrian_System/V3/ProceduralCrowdsSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/EngineTypes.h"
#include "Math/Color.h"
#include "Engine/World.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/HitResult.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_FindRandomPathPointV3::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GetWorld())
	{
		return EBTNodeResult::Failed;
	}

	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!GameInstance)
	{
		return EBTNodeResult::Failed;
	}

	UProceduralCrowdsSubsystem* ProceduralNPCSubsystem = GameInstance->GetSubsystem<UProceduralCrowdsSubsystem>();
	if (!ProceduralNPCSubsystem)
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

	// Start / End vectors
	const FVector Start = ControlledPawn->GetActorLocation();
	const FVector End   = Start + ControlledPawn->GetActorLocation();

	TArray<AActor*>& ActorsToIgnore = ProceduralNPCSubsystem->BTIgnoreArray;
	TArray<FHitResult> HitArray;

	// Sphere trace
	const bool bDidHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(), Start, End, PathpointRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false, ActorsToIgnore, DebugDrawTypes,
		HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

	if (bDidHit && HitArray.Num() > 0)
	{
		// Filter actors
		for (FHitResult& HitLoop : HitArray)
		{
			AActor* HitActorLoop = HitLoop.GetActor();
			if (!HitActorLoop) continue;

			if (!HitActorLoop->ActorHasTag(ObjectFilterTag))
			{
				ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActorLoop);
			}
		}

		// Random pick
		int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
		AActor* HitActor = HitArray[RandomIndex].GetActor();

		if (HitActor && HitActor->ActorHasTag(ObjectFilterTag))
		{
			if (OwnerComp.GetBlackboardComponent())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(
					NextWaypointVector.SelectedKeyName,
					HitArray[RandomIndex].ImpactPoint);

				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
}
