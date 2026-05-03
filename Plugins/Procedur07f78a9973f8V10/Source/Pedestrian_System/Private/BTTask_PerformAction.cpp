//  Copyright (c) 2025 KomodoBit Games. All rights reserved.

#include "BTTask_PerformAction.h"

#include "AIController.h"
#include "NPCActionInterface.h"
#include "Engine/HitResult.h"
#include "BehaviorTree/BlackboardComponent.h"


EBTNodeResult::Type UBTTask_PerformAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(OwnerComp.GetAIOwner()->GetPawn()->Implements<UNPCActionInterface>())
	{
		switch (ActionTypeNPC)
		{
		case NPCAction1:
			INPCActionInterface::Execute_NPCAction1(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;
			
		
		case NPCAction2:
			INPCActionInterface::Execute_NPCAction2(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;
		
		case NPCAction3:
			INPCActionInterface::Execute_NPCAction3(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction4:
			INPCActionInterface::Execute_NPCAction4(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction5:
			INPCActionInterface::Execute_NPCAction5(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction6:
			INPCActionInterface::Execute_NPCAction6(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction7:
			INPCActionInterface::Execute_NPCAction7(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction8:
			INPCActionInterface::Execute_NPCAction8(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction9:
			INPCActionInterface::Execute_NPCAction9(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction10:
			INPCActionInterface::Execute_NPCAction10(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction11:
			INPCActionInterface::Execute_NPCAction11(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction12:
			INPCActionInterface::Execute_NPCAction12(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction13:
			INPCActionInterface::Execute_NPCAction13(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction14:
			INPCActionInterface::Execute_NPCAction14(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction15:
			INPCActionInterface::Execute_NPCAction15(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction16:
			INPCActionInterface::Execute_NPCAction16(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction17:
			INPCActionInterface::Execute_NPCAction17(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction18:
			INPCActionInterface::Execute_NPCAction18(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction19:
			INPCActionInterface::Execute_NPCAction19(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction20:
			INPCActionInterface::Execute_NPCAction20(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction21:
			INPCActionInterface::Execute_NPCAction21(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction22:
			INPCActionInterface::Execute_NPCAction22(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;


		case NPCAction23:
			INPCActionInterface::Execute_NPCAction23(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;


		case NPCAction24:
			INPCActionInterface::Execute_NPCAction24(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction25:
			INPCActionInterface::Execute_NPCAction25(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;


		case NPCAction26:
			INPCActionInterface::Execute_NPCAction26(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction27:
			INPCActionInterface::Execute_NPCAction27(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction28:
			INPCActionInterface::Execute_NPCAction28(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction29:
			INPCActionInterface::Execute_NPCAction29(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction30:
			INPCActionInterface::Execute_NPCAction30(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction31:
			INPCActionInterface::Execute_NPCAction31(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;

		case NPCAction32:
			INPCActionInterface::Execute_NPCAction32(OwnerComp.GetAIOwner()->GetPawn());
			return EBTNodeResult::Succeeded;
			
		
		}
	}
	return EBTNodeResult::Failed;
}
