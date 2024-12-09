#include "Actors/PKEntityActor.h"

APKEntityActor::APKEntityActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EntityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	SetRootComponent(EntityMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (DefaultMesh.Succeeded())
	{
		EntityMesh->SetStaticMesh(DefaultMesh.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultMaterial(TEXT("/Engine/BasicShapes/BasicShapeMaterial"));
	if (DefaultMaterial.Succeeded())
	{
		EntityMesh->SetMaterial(0, DefaultMaterial.Object);
	}

	CollisionSize = FVector(50.0f, 50.0f, 50.0f);
}

void APKEntityActor::BeginPlay()
{
	Super::BeginPlay();
}

void APKEntityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}