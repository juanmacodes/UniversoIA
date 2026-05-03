//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BPFL_BehaviorTreeHelpers.h"
#include "AIController.h"
#include "BrainComponent.h"

void UBPFL_BehaviorTreeHelpers::RestartBehaviorTree(AAIController* AIController)
{
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->RestartLogic();
	}
}

void UBPFL_BehaviorTreeHelpers::ResumeBehaviorTree(AAIController* AIController)
{
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->ResumeLogic(FString("Resumed Via Helper Library"));
	}
}

void UBPFL_BehaviorTreeHelpers::StopBehaviorTree(AAIController* AIController)
{
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->StopLogic(FString("Stopped Via Helper Library"));
	}
}

void UBPFL_BehaviorTreeHelpers::PauseBehaviorTree(AAIController* AIController)
{
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->PauseLogic(FString("Paused Via Helper Library"));
	}
}
