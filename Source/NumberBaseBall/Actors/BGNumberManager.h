#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BGNumberManager.generated.h"

UCLASS()
class NUMBERBASEBALL_API UBGNumberManager : public UObject
{
	GENERATED_BODY()

public:
	UBGNumberManager();
	
	UFUNCTION(BlueprintCallable, Category = "NumberManager")
	static UBGNumberManager* Get();

	UFUNCTION(BlueprintCallable, Category = "NumberManager", meta = (WorldContext = "WorldContextObject"))
	void SpawnNumber(UObject* WorldContextObject, FString Number, FVector Location);

	void DestroySpawnedNumbers();

private:
	static UBGNumberManager* SingletonInstance;
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "NumberManager")
	TSubclassOf<class ABGNumber> NumberActorClass;

	TArray<ABGNumber*> SpawnedNumbers;
};
