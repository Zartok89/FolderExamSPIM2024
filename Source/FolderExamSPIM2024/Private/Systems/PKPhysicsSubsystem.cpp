// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKPhysicsSubsystem.h"

void UPKPhysicsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	EntityManagerSubsystem = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (EntityManagerSubsystem)
	{
		ComponentManager = &EntityManagerSubsystem->GetComponentManager();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize PhysicsSubsystem: EntityManagerSubsystem not found!"));
	}
}

void UPKPhysicsSubsystem::Deinitialize()
{
	Super::Deinitialize();

	EntityManagerSubsystem = nullptr;
	ComponentManager = nullptr;
}

void UPKPhysicsSubsystem::Tick(float DeltaTime)
{
	if (!ComponentManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PhysicsSubsystem: ComponentManager is null!"));
		return;
	}

	for (int32 i = 0; i < ComponentManager->PhysicsComponents.Num(); ++i)
	{
		PKPhysicsComponent& PhysicsComponent = ComponentManager->PhysicsComponents[i];

		if (!PhysicsComponent.bIsSimulating)
		{
			continue;
		}

		PKTransformComponent* TransformComponent = ComponentManager->GetTransformComponent(PhysicsComponent.EntityID);
		if (!TransformComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("PhysicsSubsystem: TransformComponent not found for Entity %d"), PhysicsComponent.EntityID);
			continue;
		}

		PhysicsComponent.Velocity += PhysicsComponent.Acceleration * DeltaTime;

		TransformComponent->Position += PhysicsComponent.Velocity * DeltaTime;
	}
}

TStatId UPKPhysicsSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(PKPhysicsSubsystem, STATGROUP_Tickables);
}