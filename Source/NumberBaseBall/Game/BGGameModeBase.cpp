#include "Game/BGGameModeBase.h"
#include "Game/BGGameStateBase.h"
#include "Player/BGPlayerController.h"
#include "Player/BGPlayerState.h"
#include "EngineUtils.h"
#include "Actors/BGNumberManager.h"

void ABGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
}

void ABGGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABGPlayerController* BGPC = Cast<ABGPlayerController>(NewPlayer);
	if (IsValid(BGPC) == true)
	{
		BGPC->NotificationText = FText::FromString(TEXT("Baseball Game"));
		AllPlayerControllers.Add(BGPC);
	
		ABGPlayerState* BGPS = BGPC->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			BGPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}
	
		ABGGameStateBase* BGGS = GetGameState<ABGGameStateBase>();
		if (IsValid(BGGS) == true)
		{
			BGGS->MulticastRPCBroadcastLoginMessage(BGPS->PlayerNameString);
		}
	}
}

FString ABGGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i<=9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num){ return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABGGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len()!=3)
		{
			break;
		}
		bool bIsValidNum = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C)== false || C == '0')
			{
				bIsValidNum = false;
				break;
			}
			UniqueDigits.Add(C);
		}
		if (bIsValidNum == false)
		{
			break;
		}
		if (UniqueDigits.Num() != 3)
		{
			break;
		}
		
		bCanPlay = true;
	}
	while (false);

	return bCanPlay;
}

FString ABGGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;
	for (int32 i= 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	UBGNumberManager* NumberManager = UBGNumberManager::Get();
	if (NumberManager)
	{
		// 액터 삭제
		NumberManager->DestroySpawnedNumbers();
		// 숫자 액터 생성
		NumberManager->SpawnNumber(GetWorld(), InGuessNumberString, FVector(0.f, 300.f, 200.f));
		// 판정 결과 액터 생성
		FString NumberText = FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
		NumberManager->SpawnNumber(GetWorld(), NumberText, FVector(0.f, 0.f, 200.f));
	}
	
	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	
	return FString::Printf(TEXT("%dS %dB"),StrikeCount, BallCount);
}

void ABGGameModeBase::PrintChatMessageString(ABGPlayerController* InChattingPlayerController,
	const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len()-3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);
	if (IsGuessNumberString(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);

		FString PlayerInfoString = TEXT("");
		ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			PlayerInfoString = BGPS->GetPlayerInfoString();
		}

		for (TActorIterator<ABGPlayerController> It(GetWorld()); It; ++It)
		{
			ABGPlayerController* BGPC = *It;
			if (IsValid(BGPC) == true)
			{
				FString CombinedMessageString = PlayerInfoString + TEXT(": ") + 
					InChatMessageString + TEXT(" -> ") + JudgeResultString;
				BGPC->ClientRPCPrintChatMessageString(CombinedMessageString);
		
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
			}
		}
	}
	else
	{
		FString PlayerInfoString = TEXT("");
		ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			PlayerInfoString = BGPS->GetPlayerInfoString();
		}
		
		for (TActorIterator<ABGPlayerController> It(GetWorld()); It; ++It)
		{
			ABGPlayerController* BGPC = *It;
			if (IsValid(BGPC) == true)
			{
				FString CombinedMessageString = PlayerInfoString + TEXT(": ") + InChatMessageString;
				BGPC->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
}

void ABGGameModeBase::IncreaseGuessCount(ABGPlayerController* InChattingPlayerController)
{
	ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
	if (IsValid(BGPS) == true)
	{
		BGPS->CurrentGuessCount++;
	}
}

void ABGGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BGPlayerController : AllPlayerControllers)
	{
		ABGPlayerState* BGPS = BGPlayerController->GetPlayerState<ABGPlayerState>();
		if (IsValid(BGPS) == true)
		{
			BGPS->CurrentGuessCount = 0;
		}
	}
}

void ABGGameModeBase::JudgeGame(ABGPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (InStrikeCount == 3)
	{
		ABGPlayerState* BGPS = InChattingPlayerController->GetPlayerState<ABGPlayerState>();
		for (const auto& BGPlayerController : AllPlayerControllers)
		{
			if (IsValid(BGPS) == true)
			{
				FString CombinedMessageString = BGPS->PlayerNameString + TEXT(" has won the game");
				BGPlayerController->NotificationText = FText::FromString(CombinedMessageString);
	
				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BGPlayerController : AllPlayerControllers)
		{
			ABGPlayerState* BGPS = BGPlayerController->GetPlayerState<ABGPlayerState>();
			if (IsValid(BGPS) == true)
			{
				if (BGPS->CurrentGuessCount < BGPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
	
		if (bIsDraw == true)
		{
			for (const auto& BGPlayerController : AllPlayerControllers)
			{
				BGPlayerController->NotificationText = FText::FromString(TEXT("This game was a draw"));
	
				ResetGame();
			}
		}
	}
}
