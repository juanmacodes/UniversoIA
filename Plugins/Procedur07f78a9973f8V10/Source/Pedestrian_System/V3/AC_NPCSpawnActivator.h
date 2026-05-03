//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "I_NPCSpawningInterface.h"
#include "Components/ActorComponent.h"
#include "UObject/Interface.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "AC_NPCSpawnActivator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEDESTRIAN_SYSTEM_API UAC_NPCSpawnActivator : public UActorComponent, public II_NPCSpawningInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_NPCSpawnActivator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	TArray<AActor*> CachedNPCActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Spawning")
	float MaxSpawnDistance = 8000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="NPC Spawning")
	float CheckRateInterval = 0.5;

	UFUNCTION()
	void ActivateSpawningNearby();
	
	virtual void AddNPCtoCache(AActor* NPCActor) override;

	virtual void RemoveNPCfromCache(AActor* NPCActor) override;

	void CacheNPCActors();

	void SetNPCCheckTimer();

	FTimerHandle NPCCheckTimer;

private:

	UPROPERTY()
	TArray<AActor*> PendingNPCAdds;
	
	UPROPERTY()
	TArray<AActor*> PendingNPCRemoves;

	UFUNCTION()
	void ProcessPendingNPCCacheChanges();

	

	
	
};



