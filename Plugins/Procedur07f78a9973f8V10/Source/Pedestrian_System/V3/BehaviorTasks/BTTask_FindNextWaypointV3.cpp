//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_FindNextWaypointV3.h"

#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
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


EBTNodeResult::Type UBTTask_FindNextWaypointV3::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GetWorld())
	{
		return EBTNodeResult::Failed;
	}

	// Get Subsystem safely
	UProceduralCrowdsSubsystem* ProceduralNPCSubsystem = GetWorld()->GetGameInstance()
		? GetWorld()->GetGameInstance()->GetSubsystem<UProceduralCrowdsSubsystem>()
		: nullptr;
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

	// Start and End vectors
	const FVector Start = ControlledPawn->GetActorLocation() + ControlledPawn->GetActorForwardVector();
	const FVector End   = Start + ControlledPawn->GetActorForwardVector() * TraceDistance;

	TArray<AActor*>& ActorsToIgnore = ProceduralNPCSubsystem->BTIgnoreArray;
	TArray<FHitResult> HitArray;

	// Sphere Trace
	bool bDidHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(), Start, End, SphereRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false, ActorsToIgnore, DebugDrawTypes,
		HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

	if (bDidHit && HitArray.Num() > 1)
	{
		// Filter ignored actors
		for (FHitResult& HitLoop : HitArray)
		{
			AActor* HitActorLoop = HitLoop.GetActor();
			if (!HitActorLoop) continue;

			if (!HitActorLoop->ActorHasTag(ObjectFilterTag))
			{
				ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActorLoop);
			}
		}

		// Random pick from array
		int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
		AActor* HitActor = HitArray[RandomIndex].GetActor();

		if (HitActor && HitActor->ActorHasTag(ObjectFilterTag))
		{
			FVector TargetLocation = UKismetMathLibrary::RandomPointInBoundingBox(HitArray[RandomIndex].ImpactPoint, PathfindingSeparation);

			if (OwnerComp.GetBlackboardComponent())
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextWaypointVector.SelectedKeyName, TargetLocation);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	// Re-find if failed first trace
	if (!bDidHit || HitArray.Num() <= 1)
	{
		const FVector StartRefind = ControlledPawn->GetActorLocation();
		const FVector EndRefind   = Start + ControlledPawn->GetActorLocation();

		TArray<AActor*>& ActorsToIgnoreRefind = ProceduralNPCSubsystem->BTIgnoreArray;
		TArray<FHitResult> HitArrayRefind;

		bool HitRefind = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(), StartRefind, EndRefind, RefindPathpointRadius,
			UEngineTypes::ConvertToTraceType(TraceChannel),
			false, ActorsToIgnoreRefind, DebugDrawTypes,
			HitArrayRefind, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

		if (HitRefind && HitArrayRefind.Num() > 0)
		{
			for (FHitResult& HitLoop : HitArrayRefind)
			{
				AActor* HitActorLoop = HitLoop.GetActor();
				if (!HitActorLoop) continue;

				if (!HitActorLoop->ActorHasTag(ObjectFilterTag))
				{
					ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActorLoop);
				}
			}

			int32 RandomIndex = FMath::RandRange(0, HitArrayRefind.Num() - 1);
			AActor* HitActor = HitArrayRefind[RandomIndex].GetActor();

			if (HitActor && HitActor->ActorHasTag(ObjectFilterTag))
			{
				if (OwnerComp.GetBlackboardComponent())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextWaypointVector.SelectedKeyName, HitArrayRefind[RandomIndex].ImpactPoint);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
