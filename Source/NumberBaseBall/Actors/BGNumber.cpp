#include "BGNumber.h"
#include "Components/BillboardComponent.h"

ABGNumber::ABGNumber()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Root);

	bReplicates = true;
	Root->SetIsReplicated(true);
}

void ABGNumber::BeginPlay()
{
	Super::BeginPlay();
}

void ABGNumber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABGNumber::SetNumber(TCHAR Character)
{
	int32 Index = -1;

	if (Character >= '0' && Character <= '9')
		Index = Character - '0';
	else if (Character == 'S')
		Index = 10;
	else if (Character == 'B')
		Index = 11;

	if (HasAuthority())
	{
		MulticastSetNumber(Index);
	}
}

void ABGNumber::MulticastSetNumber_Implementation(int32 Index)
{

	if (NumberMeshes.IsValidIndex(Index))
	{
		StaticMesh->SetStaticMesh(NumberMeshes[Index]);
	}
}
