//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "FindPathPointForward_V3.h"

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

EBTNodeResult::Type UBTTask_FindPathPointForwardV3::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	// Primary trace
	const FVector Start = ControlledPawn->GetActorLocation() + ControlledPawn->GetActorForwardVector() * TraceStartLocation;
	const FVector End   = Start + ControlledPawn->GetActorForwardVector() * TraceDistance;

	const TArray<AActor*>& ActorsToIgnore = ProceduralNPCSubsystem->BTIgnoreArray;
	TArray<FHitResult> HitArray;

	bool bDidHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(), Start, End, SphereRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false, ActorsToIgnore, DebugDrawTypes,
		HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

	if (bDidHit && HitArray.Num() > 1)
	{
		for (const FHitResult& Hit : HitArray)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor) continue;

			if (!HitActor->ActorHasTag(ObjectFilterTag))
			{
				ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActor);
			}
		}

		const int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
		AActor* HitActor = HitArray[RandomIndex].GetActor();

		if (HitActor && HitActor->ActorHasTag(ObjectFilterTag))
		{
			if (OwnerComp.GetBlackboardComponent())
			{
				const FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(
					HitArray[RandomIndex].ImpactPoint,
					PathfindingSeparation);

				OwnerComp.GetBlackboardComponent()->SetValueAsVector(
					NextWaypointVector.SelectedKeyName, RandomPoint);

				return EBTNodeResult::Succeeded;
			}
		}

		return EBTNodeResult::Failed;
	}

	// Re-find trace
	if (!bDidHit || HitArray.Num() <= 1)
	{
		const FVector StartRefind = ControlledPawn->GetActorLocation();
		const FVector EndRefind   = StartRefind + ControlledPawn->GetActorForwardVector();

		const TArray<AActor*>& ActorsToIgnoreRefind = ProceduralNPCSubsystem->BTIgnoreArray;
		TArray<FHitResult> HitArrayRefind;

		bool bHitRefind = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(), StartRefind, EndRefind, RefindPathpointRadius,
			UEngineTypes::ConvertToTraceType(TraceChannel),
			false, ActorsToIgnoreRefind, DebugDrawTypes,
			HitArrayRefind, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

		if (bHitRefind && HitArrayRefind.Num() > 0)
		{
			for (const FHitResult& Hit : HitArrayRefind)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) continue;

				if (!HitActor->ActorHasTag(ObjectFilterTag))
				{
					ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActor);
				}
			}

			const int32 RandomIndex = FMath::RandRange(0, HitArrayRefind.Num() - 1);
			AActor* HitActor = HitArrayRefind[RandomIndex].GetActor();

			if (HitActor && HitActor->ActorHasTag(ObjectFilterTag))
			{
				if (OwnerComp.GetBlackboardComponent())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(
						NextWaypointVector.SelectedKeyName,
						HitArrayRefind[RandomIndex].ImpactPoint);

					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
