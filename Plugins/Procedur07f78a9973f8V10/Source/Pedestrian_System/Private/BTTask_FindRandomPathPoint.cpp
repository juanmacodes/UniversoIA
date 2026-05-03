//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_FindRandomPathPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"


EBTNodeResult::Type UBTTask_FindRandomPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	//Start Fvector from "Controlled Pawn" in blueprints
	const FVector Start = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation();

	// End FVector From "Controlled Pawn" in blueprints with parameters for tracedistance and start
	const FVector End = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() + Start;


	//holds no values, just used for sphere trace syntax
	TArray<AActor*> ActorsToIgnore;

	//Array of the HitResults from the objects hit
	TArray<FHitResult> HitArray;

	//Setting the object channel to use
	TArray <TEnumAsByte <EObjectTypeQuery>> nameofobjects;
	nameofobjects.Add(ObjectToUse);

	

	// Sphere Trace to find all the pathpoints in the trace radius and then get a random index of one of them
	const bool Hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), Start, End, PathpointRadius, nameofobjects, false, ActorsToIgnore, DebugDrawTypes, HitArray, true, FLinearColor::Red, FLinearColor::Green, 1.0f);
	if(Hit)
	{
		int32 RandomIndex = FMath::RandRange(0, HitArray.Num() - 1);
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(NextWaypointVector.SelectedKeyName, HitArray[RandomIndex].ImpactPoint);

				
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
