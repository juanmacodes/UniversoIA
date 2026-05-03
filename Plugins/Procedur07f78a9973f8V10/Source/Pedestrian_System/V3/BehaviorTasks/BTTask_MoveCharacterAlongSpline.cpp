//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_MoveCharacterAlongSpline.h"

#include "AIController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Pedestrian_System/V3/ProceduralCrowdsSubsystem.h"
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

EBTNodeResult::Type UBTTask_MoveCharacterAlongSpline::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GetWorld())
	{
		return EBTNodeResult::Failed;
	}

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

	//Start FVector from Controlled Pawn
	const FVector Start = ControlledPawn->GetActorLocation() + ControlledPawn->GetActorForwardVector() * TraceStartLocation;
	const FVector End   = ControlledPawn->GetActorForwardVector() * TraceDistance + Start;

	// Sphere Trace
	TArray<AActor*>& ActorsToIgnore = ProceduralNPCSubsystem->BTIgnoreArray;
	TArray<FHitResult> HitArray;

	bool bDidHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(), Start, End, SphereRadius,
		UEngineTypes::ConvertToTraceType(TraceChannel),
		false, ActorsToIgnore, DebugDrawTypes,
		HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

	if (bDidHit && HitArray.Num() > 1)
	{
		for (FHitResult& HitLoop : HitArray)
		{
			AActor* HitActorLoop = HitLoop.GetActor();
			if (!HitActorLoop) continue;

			if (!HitActorLoop->ActorHasTag(ObjectFilterTag))
			{
				ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActorLoop);
			}
		}

		if (HitArray.Num() > 0)
		{
			const int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
			const FHitResult& Hit = HitArray[RandomIndex];

			AActor* HitActor = Hit.GetActor();
			if (!HitActor) return EBTNodeResult::Failed;

			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

			AActor* OwnerActor = HitActor->GetOwner();
			if (!OwnerActor) return EBTNodeResult::Failed;

			USplineComponent* SplineComponent = OwnerActor->FindComponentByClass<USplineComponent>();
			if (!SplineComponent) return EBTNodeResult::Failed;

			if (HitActor->ActorHasTag(ObjectFilterTag))
			{
				float ClosestInputKey = SplineComponent->FindInputKeyClosestToWorldLocation(Hit.ImpactPoint);
				float ClosestDistance = SplineComponent->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);
				float TargetDistance  = ClosestDistance + SplineForwardOffset;

				TargetDistance = FMath::Clamp(TargetDistance, 0.0f, SplineComponent->GetSplineLength());

				FVector SplineTargetLocation = SplineComponent->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
				FVector FinalTargetLocation  = UKismetMathLibrary::RandomPointInBoundingBox(SplineTargetLocation, PathfindingSeparation);

				UE_LOG(LogTemp, Warning, TEXT("Setting MoveToVector to: %s"), *FinalTargetLocation.ToString());

				if (OwnerComp.GetBlackboardComponent())
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(MoveToVector.SelectedKeyName, FinalTargetLocation);
					return EBTNodeResult::Succeeded;
				}
			}
		}
		return EBTNodeResult::Failed;
	}

	// Re-find path if no hit or too few
	if (!bDidHit || HitArray.Num() <= 1)
	{
		const FVector StartRefind = ControlledPawn->GetActorLocation();
		const FVector EndRefind   = ControlledPawn->GetActorForwardVector() + StartRefind;

		TArray<FHitResult> HitArrayRefind;
		const TArray<AActor*>& ActorsToIgnoreRefind = ProceduralNPCSubsystem->BTIgnoreArray;

		bool HitRefind = UKismetSystemLibrary::SphereTraceMulti(
			GetWorld(), StartRefind, EndRefind, RefindPathpointRadius,
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false, ActorsToIgnoreRefind, DebugDrawTypes,
			HitArrayRefind, true, FLinearColor::Red, FLinearColor::Green, 1.0f);

		if (HitRefind)
		{
			for (const FHitResult& Hit : HitArrayRefind)
			{
				AActor* HitActor = Hit.GetActor();
				if (!HitActor) continue;

				AActor* OwnerActor = HitActor->GetOwner();
				if (!OwnerActor) continue;

				USplineComponent* SplineComponent = OwnerActor->FindComponentByClass<USplineComponent>();
				if (!SplineComponent) continue;

				if (HitActor->ActorHasTag(ObjectFilterTag))
				{
					float DistanceOnSpline = SplineComponent->FindInputKeyClosestToWorldLocation(Hit.ImpactPoint);
					float TargetDistance   = DistanceOnSpline + SplineForwardOffset;

					TargetDistance = FMath::Clamp(TargetDistance, 0.f, SplineComponent->GetSplineLength());

					FVector TargetLocation     = SplineComponent->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
					FVector FinalTargetLocation = UKismetMathLibrary::RandomPointInBoundingBox(TargetLocation, PathfindingSeparation);

					if (OwnerComp.GetBlackboardComponent())
					{
						OwnerComp.GetBlackboardComponent()->SetValueAsVector(MoveToVector.SelectedKeyName, FinalTargetLocation);
						return EBTNodeResult::Succeeded;
					}
				}
				else
				{
					ProceduralNPCSubsystem->AddToBTIgnoreArray(HitActor);
				}
			}
		}
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}


