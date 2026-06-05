#include "NPCChatWidgetBase.h"

void UNPCChatWidgetBase::AddChatLine(const FChatLine& Line)
{
	DisplayedLines.Add(Line);
	BP_OnChatLineAdded(Line);
}

void UNPCChatWidgetBase::ClearChatLines()
{
	DisplayedLines.Reset();
	BP_OnChatCleared();
}