//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "I_NPCSpawningInterface.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "UObject/Interface.h"
#include "AC_CrowdSpawnerV2.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestroyEvent);

USTRUCT(BlueprintType)
struct FSpawnParameters
{
	GENERATED_BODY()
	public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 MaxSpawnAmount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	FVector CharacterHeight = FVector(0, 0, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	FVector ProceduralSeparation = FVector(0, 0, 0);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<AActor> NPCToSpawn = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	FRotator SpawnRotation = FRotator(0, 0, 0);
	
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEDESTRIAN_SYSTEM_API UAC_CrowdSpawnerV2 : public UActorComponent, public II_NPCSpawningInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_CrowdSpawnerV2();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	virtual void SendSpawnEvent() override;

	virtual void SendDestroyEvent() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool UseCPPSpawning = true;
	
	UFUNCTION()
	void SpawnNPCCPP();

	UFUNCTION()
	void DestroyNPCCPP();

	UFUNCTION(BlueprintCallable, Category = "Crowd Spawner V2")
	void SpawnNPCBP(int32 MaxSpawnAmount, FVector CharacterHeight, FVector ProceduralSeparation, TSubclassOf<AActor> NPCToSpawn, FRotator SpawnRotation);

	UFUNCTION(BlueprintCallable, Category = "Crowd Spawner V2")
	void DestroyNPCBP();

	UPROPERTY()
	bool BDoOnceSpawn = false;
	
	UPROPERTY()
	bool BDoOnceDestroy = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd Spawner")
	bool IsPawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crowd Spawner")
	FSpawnParameters SpawnParameters;

	UPROPERTY(BlueprintAssignable, Category = "Crowd Spawner")
	FOnSpawnEvent OnSpawnEvent;

	UPROPERTY(BlueprintAssignable, Category = "Crowd Spawner")
	FOnDestroyEvent OnDestroyEvent;

	UFUNCTION()
	void SendSpawnedActorToPlayer(AActor* ActorToSend);

	UFUNCTION()
	void SendDestroyedActorToPlayer(AActor* ActorToDestroy);
	
		
};
