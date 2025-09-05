#include "BGNumberManager.h"
#include "BGNumber.h"

UBGNumberManager* UBGNumberManager::SingletonInstance = nullptr;

UBGNumberManager::UBGNumberManager()
{
}

UBGNumberManager* UBGNumberManager::Get()
{
	if (!SingletonInstance)
	{
		SingletonInstance = NewObject<UBGNumberManager>();
		SingletonInstance->AddToRoot();
	}
	return SingletonInstance;
}

void UBGNumberManager::SpawnNumber(UObject* WorldContextObject, FString Number, FVector Location)
{

	if (!NumberActorClass)
	{
		NumberActorClass = LoadClass<ABGNumber>(nullptr, TEXT("/Game/Actors/BP_Numbers.BP_Numbers_C"));
		if (!NumberActorClass)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to load NumberActorClass!"));
			return;
		}
	}


	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Log, TEXT("!WorldContextObject"))
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

	if (!World)
	{
		return;
	}
	
	FVector SpawnLocation = Location;
	float OffsetY = 50.f;    
	for (TCHAR Character : Number)
	{			
		if (!NumberActorClass) return;
		
		UE_LOG(LogTemp, Log, TEXT("Spawn Number"))

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		SpawnParams.Instigator = nullptr;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		ABGNumber* SpawnedNumber = World->SpawnActor<ABGNumber>(NumberActorClass, SpawnLocation, FRotator(-90, 0, 0), SpawnParams);
		if (SpawnedNumber)
		{
			SpawnedNumber->SetNumber(Character);
			SpawnedNumbers.Add(SpawnedNumber);
		}	
		SpawnLocation.Y -= OffsetY;
	}

}

void UBGNumberManager::DestroySpawnedNumbers()
{
	if (SpawnedNumbers.Num() > 0)
	{
		for (ABGNumber* NumActor : SpawnedNumbers)
		{
			if (IsValid(NumActor))
			{
				if (NumActor->HasAuthority())
				{
					NumActor->Destroy();
				}
			}
		}
		SpawnedNumbers.Empty();
	}
}
