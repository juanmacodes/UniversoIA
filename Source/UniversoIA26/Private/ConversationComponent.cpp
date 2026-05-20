#include "ConversationComponent.h"

void UConversationComponent::AddUserMessage(const FString& Text)
{
	FChatLine Line;
	Line.Role = EChatRole::User;
	Line.Text = Text;
	Line.Emotion = ENPCEmotion::Neutral;
	Messages.Add(Line);
}

void UConversationComponent::AddAssistantMessage(const FString& Text, ENPCEmotion Emotion)
{
	FChatLine Line;
	Line.Role = EChatRole::Assistant;
	Line.Text = Text;
	Line.Emotion = Emotion;
	Messages.Add(Line);
}

void UConversationComponent::AddSystemMessage(const FString& Text)
{
	FChatLine Line;
	Line.Role = EChatRole::System;
	Line.Text = Text;
	Line.Emotion = ENPCEmotion::Neutral;
	Messages.Add(Line);
}

void UConversationComponent::ClearConversation()
{
	Messages.Reset();
}

TArray<FChatLine> UConversationComponent::GetLastMessages(int32 MaxCount) const
{
	if (MaxCount <= 0 || Messages.Num() <= MaxCount)
	{
		return Messages;
	}

	TArray<FChatLine> Result;
	const int32 StartIndex = Messages.Num() - MaxCount;

	for (int32 i = StartIndex; i < Messages.Num(); ++i)
	{
		Result.Add(Messages[i]);
	}

	return Result;
}