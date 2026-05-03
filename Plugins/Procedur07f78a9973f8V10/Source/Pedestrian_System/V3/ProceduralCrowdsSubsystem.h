//  Copyright (c) 2025 KomodoBit Games. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/Actor.h"
#include "Logging/LogMacros.h" 
#include "ProceduralCrowdsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UProceduralCrowdsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Procedural NPC Zombies")
	int32 NPCSpawnedCount;

	UFUNCTION(BlueprintPure, Category = "Procedural NPC Zombies")
	bool LimitNPCSpawned(int32 MaxNPCSpawnAllowed);

	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Zombies")
	int32 IncrementNPCSpawnedCount();

	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Zombies")
	int32 DecrementNPCSpawnedCount();

	UFUNCTION()
	void AddToBTIgnoreArray(AActor* ActorToAdd);

	UPROPERTY()
	TArray<AActor*> BTIgnoreArray;
	
};
