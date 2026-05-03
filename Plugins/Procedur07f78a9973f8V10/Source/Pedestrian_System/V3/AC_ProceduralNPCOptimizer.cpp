//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "AC_ProceduralNPCOptimizer.h"

#include "BrainComponent.h"
#include "DetourCrowdAIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UAC_ProceduralNPCOptimizer::UAC_ProceduralNPCOptimizer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAC_ProceduralNPCOptimizer::BeginPlay()
{
	Super::BeginPlay();

	//Calls the function that optimizes the character initially then runs it on a timer.
	OptimizeTimer();
	FTimerHandle TickTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &UAC_ProceduralNPCOptimizer::OptimizeTimer, TimerTickRate, true);
	
}

void UAC_ProceduralNPCOptimizer::OptimizeTimer()
{
	//Simple for loop going through all the struct entries.
	for(int i = 0; i < ProceduralNPCOptimizations.Num(); i++)
	{
		if(GetOwner())
		{
			OptimizeNPCLoop(i);
		}
	}
}


void UAC_ProceduralNPCOptimizer::OptimizeNPCLoop(int Index)
{

 // Get Player Character Node equivalent
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (IsValid(PlayerPawn))
    {
        // Get owner's location
        AActor* Owner = GetOwner();
        if (IsValid(Owner))
        {
        	//Initializing variables for use in the distance calculataion.
            FVector Self = Owner->GetActorLocation();
            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            float DistanceFromCharacter = FVector::Distance(Self, PlayerLocation);

        	//ensures the optimization only occurs in the correct distance thresholds
            if (DistanceFromCharacter >= ProceduralNPCOptimizations[Index].OptimizeTierStartDistance && DistanceFromCharacter <= ProceduralNPCOptimizations[Index].OptimizeTierStopDistance)
            {
                if (USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(Owner->GetComponentByClass(USkeletalMeshComponent::StaticClass())))
                {
                    if (!SkeletalMesh->IsSimulatingPhysics())
                    {
                    	//Sets all the optimization params.
                        SkeletalMesh->bPerBoneMotionBlur = ProceduralNPCOptimizations[Index].UseBoneMotionBlur;
                        SkeletalMesh->bDeferKinematicBoneUpdate = ProceduralNPCOptimizations[Index].DeferKinematicBonesUpdate;
                        SkeletalMesh->VisibilityBasedAnimTickOption = ProceduralNPCOptimizations[Index].VisibilityAnimationTick;
                        SkeletalMesh->bEnableUpdateRateOptimizations = ProceduralNPCOptimizations[Index].EnableUpdateRateOptimizations;
                        SkeletalMesh->bNoSkeletonUpdate = ProceduralNPCOptimizations[Index].SkipSkeletalMeshUpdate;
                        SkeletalMesh->bSkipBoundsUpdateWhenInterpolating = ProceduralNPCOptimizations[Index].SkipBoundsWhenInterpolating;
                        SkeletalMesh->bSkipKinematicUpdateWhenInterpolating = ProceduralNPCOptimizations[Index].SkipKinematicUpdateWhenInterpolating;

                    	ToggleBehaviors(Index);
                        if (SkeletalMesh->AnimUpdateRateParams)
                        {
                            SkeletalMesh->AnimUpdateRateParams->UpdateRate = ProceduralNPCOptimizations[Index].SkeletalMeshUpdateRate;
                        }

                        SkeletalMesh->CastShadow = ProceduralNPCOptimizations[Index].CastShadow;
                    }
                    else
                    {
                        if (SkeletalMesh->AnimUpdateRateParams)
                        {
                            SkeletalMesh->AnimUpdateRateParams->UpdateRate = 1;
                        }
                        SkeletalMesh->bNoSkeletonUpdate = false;
                    }
                }
            	if (UCharacterMovementComponent* CharMoveComponent = Cast<UCharacterMovementComponent>(Owner->GetComponentByClass(UCharacterMovementComponent::StaticClass())))
            	{
            		CharMoveComponent->SetComponentTickInterval(ProceduralNPCOptimizations[Index].CharMovementTickRate);
            	}
            	if(GetOwner())
            	{
            		GetOwner()->SetActorTickInterval(ProceduralNPCOptimizations[Index].OwnerTickRate);
            	}
            }
        }
    }
}

void UAC_ProceduralNPCOptimizer::ToggleBehaviors(int Index)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (IsValid(OwnerPawn))
	{
		AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
		if (AIController)
		{
			UBrainComponent* BrainComponent = AIController->GetBrainComponent();
			if (BrainComponent)
			{
				if (ProceduralNPCOptimizations[Index].DisableBehaviorTree == true)
				{
					BrainComponent->StopLogic(TEXT("Brain disabled by component."));
				}
				else
				{
					BrainComponent->StartLogic();
				}
			}
		}
	}
}



