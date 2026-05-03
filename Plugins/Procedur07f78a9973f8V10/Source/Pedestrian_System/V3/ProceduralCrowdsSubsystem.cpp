//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "ProceduralCrowdsSubsystem.h"
#include "GameFramework/Actor.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/GameInstance.h"
#include "Logging/LogMacros.h" 


//these functions are used for limiting the zombie spawn count in the world.
bool UProceduralCrowdsSubsystem::LimitNPCSpawned(int32 MaxNPCSpawnAllowed)
{
	return NPCSpawnedCount < MaxNPCSpawnAllowed;
}

int32 UProceduralCrowdsSubsystem::IncrementNPCSpawnedCount()
{
	return ++NPCSpawnedCount;
}

int32 UProceduralCrowdsSubsystem::DecrementNPCSpawnedCount()
{
	
	return --NPCSpawnedCount;
}

void UProceduralCrowdsSubsystem::AddToBTIgnoreArray(AActor* ActorToAdd)
{
	if (IsValid(ActorToAdd))
	{
		BTIgnoreArray.Add(ActorToAdd);
		UE_LOG(LogTemp, Warning, TEXT("BTIgnore Array Length - %d"), BTIgnoreArray.Num());
	}
}
