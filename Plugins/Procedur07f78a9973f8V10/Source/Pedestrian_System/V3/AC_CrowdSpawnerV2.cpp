//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "AC_CrowdSpawnerV2.h"
#include "AC_NPCSpawnActivator.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/Interface.h"
#include "I_NPCSpawningInterface.h"

// Sets default values for this component's properties
UAC_CrowdSpawnerV2::UAC_CrowdSpawnerV2()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UAC_CrowdSpawnerV2::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}




void UAC_CrowdSpawnerV2::SendSpawnEvent()
{
	//Use the CPPSpawning if true, otherwise send a blueprint delegate for blueprint spawning.
	if (UseCPPSpawning)
	{
		SpawnNPCCPP();
		BDoOnceDestroy = false;
		UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Do once for distance to destroy reset"));
	}
	else
	{
		//Calls a blueprint delegate so the blueprint version can be used. Use if you need networking.
		OnSpawnEvent.Broadcast();
		BDoOnceDestroy = false;
		UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Do once for distance to destroy reset"));
	}
}

void UAC_CrowdSpawnerV2::SendDestroyEvent()
{

	//Use the CPP removal if true, otherwise send a blueprint delegate for blueprint removal
	if (UseCPPSpawning)
	{
		DestroyNPCCPP();
		BDoOnceSpawn = false;
		UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: NPC distance do once reset"));
	}
	else
	{
		////Calls a blueprint delegate so the blueprint version can be used. Use if you need networking.
		OnDestroyEvent.Broadcast();
		BDoOnceSpawn = false;
		UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: NPC distance do once reset"));
	}
}

void UAC_CrowdSpawnerV2::SpawnNPCCPP()
{

	//Checks if its a pawn first as pawns only need destroying.
	if (IsPawn == false)
	{
		if (!IsValid(GetOwner()))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Crowd Spawner V2 Component Owner Not Valid"));
			return;
		}
		//Get Player Pawn Node equivelent
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if(!IsValid(PlayerPawn))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Player Pawn Not Valid"));
			return;
		}
		
		
		//ensures they have not been spawned yet, this gets reset to false on the interface event.
		if (BDoOnceSpawn == false)
		{
			BDoOnceSpawn = true;
					
			//Spawn Pedestrians on a blueprint exposed for loop
			for (int32 i = 0; i < SpawnParameters.MaxSpawnAmount; i++)
			{
				//Math For Spawn Actor
				const FVector Origin = GetOwner()->GetActorLocation() + SpawnParameters.CharacterHeight;
				FVector ProceduralSeparationLocal = UKismetMathLibrary::RandomPointInBoundingBox(Origin, SpawnParameters.ProceduralSeparation);
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnParameters.NPCToSpawn, ProceduralSeparationLocal, SpawnParameters.SpawnRotation);
				UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Spawn of NPC Complete"));

				if (SpawnedActor)
				{
					SendSpawnedActorToPlayer(SpawnedActor);
				}
			}
		}
	}
}

void UAC_CrowdSpawnerV2::DestroyNPCCPP()
{

	//if is pawn is set to true in editor then do the destroy function.
	if (IsPawn)
	{
		if (!IsValid(GetOwner()))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Crowd Spawner V2 Component Owner Not Valid"));
			return;
		}
	
		//Get Player Character Node equivalent
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!IsValid(PlayerPawn))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: PlayerPawn Not Valid"));
			return;
		}
	
	
		
		
		//Start Do Once
		if (BDoOnceDestroy == false)
		{
			BDoOnceDestroy = true;

			//this interface function removes the pawn from the spawn activator component
			SendDestroyedActorToPlayer(GetOwner());
			//Destroy pedestrian from level
			GetOwner()->Destroy();
			UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Removal of NPC Complete"));
		}
	}
}

void UAC_CrowdSpawnerV2::SpawnNPCBP(int32 MaxSpawnAmount, FVector CharacterHeight,
	FVector ProceduralSeparation, TSubclassOf<AActor> NPCToSpawn, FRotator SpawnRotation)
{

	//same thing as cpp version but for blueprints, this can be made network replicated.
	if (!IsValid(NPCToSpawn))
	{
		UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: NPC to Spawn value is not valid"));
		return;
	}
	if (!IsValid(GetOwner()))
	{
		UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Crowd Spawner V2 Component Owner Not Valid"));
		return;
	}
	//Get Player Pawn Node equivelent
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if(!IsValid(PlayerPawn))
	{
		UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Player Pawn Not Valid"));
		return;
	}
		
	
	//Start Do Once
	if (BDoOnceSpawn == false)
	{
		BDoOnceSpawn = true;
					
		//Spawn Pedestrians on a blueprint exposed for loop
		for (int32 i = 0; i < MaxSpawnAmount; i++)
		{
			//Math For Spawn Actor
			const FVector Origin = GetOwner()->GetActorLocation() + CharacterHeight;
			FVector ProceduralSeparationLocal = UKismetMathLibrary::RandomPointInBoundingBox(Origin, ProceduralSeparation);
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(NPCToSpawn, ProceduralSeparationLocal, SpawnRotation);
			UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Spawn of NPC Complete"));

			if (SpawnedActor)
			{
				SendSpawnedActorToPlayer(SpawnedActor);
			}
		}
	}
}

void UAC_CrowdSpawnerV2::DestroyNPCBP()
{
	if (IsPawn)
	{
		if (!IsValid(GetOwner()))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: Crowd Spawner V2 Component Owner Not Valid"));
			return;
		}
	
		//Get Player Character Node equivalent
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (!IsValid(PlayerPawn))
		{
			UE_LOG(LogTemp,	Warning, TEXT("Procedural NPC CROWDS: PlayerPawn Not Valid"));
			return;
		}
		
		//Start Do Once
		if (BDoOnceDestroy == false)
		{
			BDoOnceDestroy = true;
			SendDestroyedActorToPlayer(GetOwner());
			//Destroy pedestrian from level
			GetOwner()->Destroy();
			UE_LOG(LogTemp,	Log, TEXT("Procedural NPC CROWDS: Removal of NPC Complete"));
		}
	}
}

void UAC_CrowdSpawnerV2::SendSpawnedActorToPlayer(AActor* ActorToSend)
{
	if (!IsValid(GetOwner()))
	{
		return;
	}
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn))
	{
		return;
	}
		
	if (UAC_NPCSpawnActivator* PlayerNPCActivatorComponent = PlayerPawn->FindComponentByClass<UAC_NPCSpawnActivator>())
	{
		if (PlayerNPCActivatorComponent->GetClass()->ImplementsInterface(UI_NPCSpawningInterface::StaticClass()))
		{
			if (II_NPCSpawningInterface* SpawningInterface = Cast<II_NPCSpawningInterface>(PlayerNPCActivatorComponent))
			{
				//adds the NPC to the player cache for spawn and removal.
				SpawningInterface->AddNPCtoCache(ActorToSend);
			}
		}
	}
}

void UAC_CrowdSpawnerV2::SendDestroyedActorToPlayer(AActor* ActorToDestroy)
{
	if (!IsValid(GetOwner()))
	{
		return;
	}
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(PlayerPawn))
	{
		return;
	}
		
	if (UAC_NPCSpawnActivator* PlayerNPCActivatorComponent = PlayerPawn->FindComponentByClass<UAC_NPCSpawnActivator>())
	{
		if (PlayerNPCActivatorComponent->GetClass()->ImplementsInterface(UI_NPCSpawningInterface::StaticClass()))
		{
			if (II_NPCSpawningInterface* SpawningInterface = Cast<II_NPCSpawningInterface>(PlayerNPCActivatorComponent))
			{
				//removes the NPC from the player cache for spawn and removal.
				SpawningInterface->RemoveNPCfromCache(ActorToDestroy);
			}
		}
	}
}



