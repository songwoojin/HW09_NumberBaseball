#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGNumber.generated.h"

UCLASS()
class NUMBERBASEBALL_API ABGNumber : public AActor
{
	GENERATED_BODY()

public:
	ABGNumber();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Scene Root")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StaticMeh Component")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly, Category="Number")
	TArray<UStaticMesh*> NumberMeshes;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

public:
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetNumber(int32 Index);
	void SetNumber(TCHAR Character);
	
	FVector TargetLocation;
	
private:
	bool bDestroying = false;

};
