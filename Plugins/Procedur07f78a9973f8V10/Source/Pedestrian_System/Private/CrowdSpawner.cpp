//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "CrowdSpawner.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Vector.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"



// Sets default values for this component's properties
UCrowdSpawner::UCrowdSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCrowdSpawner::BeginPlay()
{
	Super::BeginPlay();
	//Initialize do once bool with value
	bDo = false;

	// ...
	
}





void UCrowdSpawner::CrowdSpawnByDistance(float DistanceToSpawn, int32 MaxSpawnAmount, FVector CharacterHeight, FVector ProceduralSeparation, TSubclassOf<AActor> NPCToSpawn, FRotator SpawnRotation)
{

	//Get Player Character Node equivelent
	APawn* Character = UGameplayStatics::GetPlayerPawn(this, 0);

	if(IsValid(Character))
	{
	//Get owner of components location
	FVector Self = GetOwner()->GetActorLocation();
	//Get Player Character Location
	FVector GetPlayerCharacter = Character->GetActorLocation();
	// Get Distance To Node equivelent
	float DistanceFromCharacter = FVector::Distance(Self, GetPlayerCharacter);
	
	//Verify Character is not null
	if (IsValid(Character)) 
	{
			//Check Distance from player using above variables
			if (DistanceFromCharacter <= DistanceToSpawn)
			{
				//Start Do Once
				if (bDo == false)
				{
					bDo = true;
					
					//Spawn Pedestrians on a blueprint exposed for loop
					for (int i = 0; i < MaxSpawnAmount; i++)
					{
						//Math For Spawn Actor
						const FVector Origin = GetOwner()->GetActorLocation() + CharacterHeight;
						ProceduralSeparation = UKismetMathLibrary::RandomPointInBoundingBox(Origin, ProceduralSeparation);
						GetWorld()->SpawnActor<AActor>(NPCToSpawn, ProceduralSeparation, SpawnRotation);

					}

				}

			}
			//Reset Do Once Node
			else
			{
				bDo = false;
			}

			
			
	}
}
}




