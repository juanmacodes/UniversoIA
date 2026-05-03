//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "PedestrianDestroyer.h"
#include "Math/Vector.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"



// Sets default values for this component's properties
UPedestrianDestroyer::UPedestrianDestroyer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPedestrianDestroyer::BeginPlay()
{
	Super::BeginPlay();
	//Initialize do once bool with value
	bDo = false;

	
}

void UPedestrianDestroyer::DestroyByDistance(float DistanceToSpawn)
{
	//Get Player Character Node equivalent
	APawn* Character = UGameplayStatics::GetPlayerPawn(this, 0);

	if (IsValid(Character)) 
	{

	//Get owner of components location
	FVector Self = GetOwner()->GetActorLocation();
	//Get Player Character Location
	FVector GetPlayerCharacter = Character->GetActorLocation();
	//Get Distance To Node equivelent
	float DistanceFromCharacter = FVector::Distance(Self, GetPlayerCharacter);
	
	
	//Verify Character is not null
	if (IsValid(Character))
	{
		//Check Distance from player using above variables
		if (DistanceFromCharacter >= DistanceToSpawn)
		{
			//Start Do Once
			if (bDo == false)
			{
				bDo = true;
				//Destroy pedestrian from level
				GetOwner()->Destroy();
			}
		}
		//Reset Do Once
		else
		{
			bDo = false;
		}
	}
}
}

