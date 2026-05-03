//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "AC_NPCSpawnActivator.h"

#include "AC_CrowdSpawnerV2.h"
#include "Kismet/GameplayStatics.h"
#include "I_NPCSpawningInterface.h"
#include "UObject/Interface.h"
#include "TimerManager.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UAC_NPCSpawnActivator::UAC_NPCSpawnActivator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_NPCSpawnActivator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CacheNPCActors();
	SetNPCCheckTimer();

	
	
}

void UAC_NPCSpawnActivator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NPCCheckTimer);
	}
}


// Called every frame
void UAC_NPCSpawnActivator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UAC_NPCSpawnActivator::ActivateSpawningNearby()
{
	//This function takes pending cachce changes and loops over them sending spawn or removal events to the actors in the NPCCache array.
	if (GetOwner())
	{
		FVector OwnerLocation = GetOwner()->GetActorLocation();

		//Adds to the array outside of the loop to ensure safety from crashes.
		ProcessPendingNPCCacheChanges();
	
		for (AActor* NPCActor : CachedNPCActors)
		{
			if (NPCActor)
			{
				if (UAC_CrowdSpawnerV2* CrowdSpawnerComponent = NPCActor->FindComponentByClass<UAC_CrowdSpawnerV2>())
				{
					if (CrowdSpawnerComponent->GetClass()->ImplementsInterface(UI_NPCSpawningInterface::StaticClass()))
					{
						if (FVector::Dist(OwnerLocation, NPCActor->GetActorLocation()) <= MaxSpawnDistance)
						{
							if (II_NPCSpawningInterface* SpawningInterface = Cast<II_NPCSpawningInterface>(CrowdSpawnerComponent))
							{
								SpawningInterface->SendSpawnEvent();
							}
						}
						else
						{
							if (II_NPCSpawningInterface* SpawningInterface = Cast<II_NPCSpawningInterface>(CrowdSpawnerComponent))
							{
								SpawningInterface->SendDestroyEvent();
							}
						}
					}
				}
			}
		}
	}
}

void UAC_NPCSpawnActivator::CacheNPCActors()
{
	//This caches all actors in a level on begin play to keep performance good when iterating.
	TArray<AActor*> NPCActors;
	
	UGameplayStatics::GetAllActorsOfClass(this, AActor::StaticClass(), NPCActors);

	for (AActor* NPCActor : NPCActors)
	{
		if (NPCActor->FindComponentByClass<UAC_CrowdSpawnerV2>())
		{
			if (NPCActor)
			{
				CachedNPCActors.AddUnique(NPCActor);
			}
		}
	}
	
}

void UAC_NPCSpawnActivator::SetNPCCheckTimer()
{
	if (GetOwner() && GetWorld())
	{
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(NPCCheckTimer, this, &UAC_NPCSpawnActivator::ActivateSpawningNearby, CheckRateInterval, true);
	}
}

void UAC_NPCSpawnActivator::ProcessPendingNPCCacheChanges()
{
	
	for (AActor* ActorToRemove : PendingNPCRemoves)
	{
		CachedNPCActors.Remove(ActorToRemove);
	}
	PendingNPCRemoves.Empty();

	for (AActor* ActorToAdd : PendingNPCAdds)
	{
		if (!CachedNPCActors.Contains(ActorToAdd))
		{
			CachedNPCActors.Add(ActorToAdd);
		}
	}
	PendingNPCAdds.Empty();
}



void UAC_NPCSpawnActivator::AddNPCtoCache(AActor* NPCActor)
{
	if (IsValid(NPCActor))
	{
		if (!CachedNPCActors.Contains(NPCActor))
		{
			PendingNPCAdds.AddUnique(NPCActor);
			
		}
	}
	
}

void UAC_NPCSpawnActivator::RemoveNPCfromCache(AActor* NPCActor)
{
	if (IsValid(NPCActor))
	{
		if (CachedNPCActors.Contains(NPCActor))
		{
			PendingNPCRemoves.AddUnique(NPCActor);
			
		}
	}
}





