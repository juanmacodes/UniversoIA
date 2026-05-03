//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_FindPathPointForward.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"

EBTNodeResult::Type UBTTask_FindPathPointForward::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Start Fvector from "Controlled Pawn" in blueprints
	const FVector Start = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation() + OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() * TraceStartLocation;

	// End FVector From "Controlled Pawn" in blueprints with parameters for tracedistance and start
	const FVector End = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() * TraceDistance + Start;


	//holds no values, just used for sphere trace syntax
	TArray<AActor*> ActorsToIgnore;

	//Array of the HitResults from the objects hit
	TArray<FHitResult> HitArray;

	//Setting the object channel to use
	TArray <TEnumAsByte <EObjectTypeQuery>> nameofobjects;
	nameofobjects.Add(ObjectToUse);

	

	// Sphere Trace for multi objects with branch check for hit result
	const bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, SphereRadius, nameofobjects, false, ActorsToIgnore, DebugDrawTypes, HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);
	{
		if (Hit)
		{

			//Gets a random index from the hitarray
			int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
			
			// Setting the blackboard key to be the vector of the impact point vector.
			// NOTE: HitArray is getting a random index from the length of the break hit result which converts array to single.
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextWaypointVector.SelectedKeyName, UKismetMathLibrary::RandomPointInBoundingBox(HitArray[RandomIndex].ImpactPoint, PathfindingSeparation));

			if(HitArray.Num() <= 3)
			{
			

				// Rotate the character by 45 degrees
				FRotator CurrentRotation = OwnerComp.GetOwner()->GetActorRotation();
				FRotator NewRotation = FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + 45.0f, CurrentRotation.Roll);
				OwnerComp.GetOwner()->SetActorRotation(NewRotation);

				return EBTNodeResult::Succeeded;


				
					return EBTNodeResult::Succeeded;
				
				
			}
			return EBTNodeResult::Succeeded;
		}
		
	
			
			
			
		
		else
		{
			//Start Fvector from "Controlled Pawn" in blueprints
			const FVector StartRefind = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();

			// End FVector From "Controlled Pawn" in blueprints with parameters for tracedistance and start
			const FVector EndRefind = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() + Start;


			//holds no values, just used for sphere trace syntax
			TArray<AActor*> ActorsToIgnoreRefind;

			//Array of the HitResults from the objects hit
			TArray<FHitResult> HitArrayRefind;

			//Setting the object channel to use
			TArray <TEnumAsByte <EObjectTypeQuery>> nameofobjectsRefind;
			nameofobjectsRefind.Add(ObjectToUse);

	

			// Sphere Trace for multi objects with branch check for hit result
			const bool HitRefind = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), StartRefind, EndRefind, RefindPathpointRadius, nameofobjectsRefind, false, ActorsToIgnoreRefind, DebugDrawTypes, HitArrayRefind, true, FLinearColor::Red, FLinearColor::Green, 1.0f);
			if(HitRefind)
			{
				//Gets a random index from the hit result array.
				int32 RandomIndex = FMath::RandRange(0, HitArrayRefind.Num() - 1);
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextWaypointVector.SelectedKeyName, HitArrayRefind[RandomIndex].ImpactPoint);

				
				return EBTNodeResult::Succeeded;
			}
			return EBTNodeResult::Failed;
		}
	}
	
}


