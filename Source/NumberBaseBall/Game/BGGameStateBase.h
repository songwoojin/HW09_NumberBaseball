#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BGGameStateBase.generated.h"

UCLASS()
class NUMBERBASEBALL_API ABGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("DefaultName")));
};
