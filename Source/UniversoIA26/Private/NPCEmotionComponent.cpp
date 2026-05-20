#include "NPCEmotionComponent.h"

void UNPCEmotionComponent::SetEmotion(ENPCEmotion NewEmotion)
{
	if (CurrentEmotion == NewEmotion)
	{
		return;
	}

	CurrentEmotion = NewEmotion;
	OnEmotionChanged.Broadcast(CurrentEmotion);
}