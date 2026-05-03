//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_NPCSpawningInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_NPCSpawningInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PEDESTRIAN_SYSTEM_API II_NPCSpawningInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SendSpawnEvent(){};

	virtual void SendDestroyEvent(){};

	virtual void AddNPCtoCache(AActor* NPCActor){};

	virtual void RemoveNPCfromCache(AActor* NPCActor){};
};
