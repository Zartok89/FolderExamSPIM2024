#include "Actors/PKEntityActor.h"

APKEntityActor::APKEntityActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EntityID = -1;
}

void APKEntityActor::BeginPlay()
{
	Super::BeginPlay();
}

void APKEntityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

