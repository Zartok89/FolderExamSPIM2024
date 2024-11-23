// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKSpawningSubsystem.h"

#include "Components/PKPhysicsComponent.h"
#include "Components/PKTransformComponent.h"

int32 UPKSpawningSubsystem::SpawnEntityAtLocation(const FVector& Location, const FRotator& Rotation,
	const FVector& Scale)
{
	// Get the Entity Manager Subsystem
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("Entity Manager Subsystem not found!"));
		return -1;
	}

	// Create a new entity
	int32 EntityID = EntityManager->CreateEntity();
	if (EntityID == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create entity!"));
		return -1;
	}

	// Add a Transform Component
	TSharedPtr<PKTransformComponent> TransformComponent = MakeShared<PKTransformComponent>();
	TransformComponent->Position = Location;
	TransformComponent->Rotation = Rotation;
	TransformComponent->Scale = Scale;
	EntityManager->GetComponentManager().AddComponent<PKTransformComponent>(EntityID, TEXT("Transform"), TransformComponent);

	// Add a Physics Component
	TSharedPtr<PKPhysicsComponent> PhysicsComponent = MakeShared<PKPhysicsComponent>();
	PhysicsComponent->Velocity = FVector::ZeroVector;
	PhysicsComponent->Acceleration = FVector::ZeroVector;
	PhysicsComponent->bIsSimulating = true;
	EntityManager->GetComponentManager().AddComponent<PKPhysicsComponent>(EntityID, TEXT("Physics"), PhysicsComponent);

	// Log the spawn
	UE_LOG(LogTemp, Log, TEXT("Spawned Entity ID: %d at Location: %s"), EntityID, *Location.ToString());

	return EntityID;
}