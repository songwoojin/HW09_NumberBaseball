// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BGChatInput.h"

#include "Components/EditableTextBox.h"
#include "Player/BGPlayerController.h"

void UBGChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this,&ThisClass::OnChatInputTextCommitted)==false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this,&ThisClass::OnChatInputTextCommitted);
	}
}

void UBGChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this,&ThisClass::OnChatInputTextCommitted)==true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this,&ThisClass::OnChatInputTextCommitted);
	}
}

void UBGChatInput::OnChatInputTextCommitted(const FText& NewText, ETextCommit::Type CommitMethod)
{
	if (CommitMethod==ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController=GetOwningPlayer();
		if (IsValid(OwningPlayerController))
		{
			ABGPlayerController* OwningBGPlayerController=Cast<ABGPlayerController>(OwningPlayerController);
			if (IsValid(OwningBGPlayerController))
			{
				OwningBGPlayerController->SetChatMessageString(NewText.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
