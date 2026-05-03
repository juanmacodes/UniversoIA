//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFL_BehaviorTreeHelpers.generated.h"

/**
 * 
 */
UCLASS()
class PEDESTRIAN_SYSTEM_API UBPFL_BehaviorTreeHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Crowds V3")
	static void RestartBehaviorTree(AAIController* AIController);

	
	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Crowds V3")
	static void ResumeBehaviorTree(AAIController* AIController);

	
	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Crowds V3")
	static void StopBehaviorTree(AAIController* AIController);

	UFUNCTION(BlueprintCallable, Category = "Procedural NPC Crowds V3")
	static void PauseBehaviorTree(AAIController* AIController);
	
	
};
