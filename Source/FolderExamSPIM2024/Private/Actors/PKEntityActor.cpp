#include "Actors/PKEntityActor.h"

APKEntityActor::APKEntityActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EntityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityMesh"));
	SetRootComponent(EntityMesh);
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());

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
	BoxCollision->SetBoxExtent(CollisionSize);

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void APKEntityActor::BeginPlay()
{
	Super::BeginPlay();
}

void APKEntityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}