// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKSpawningSubsystem.h"

#include "Components/PKMeshComponent.h"
#include "Components/PKPhysicsComponent.h"
#include "Components/PKTransformComponent.h"

APKEntityActor* UPKSpawningSubsystem::SpawnEntityActorAtLocation(const FVector& Location, const FRotator& Rotation,
	const FVector& Scale)
{
	// Get the Entity Manager Subsystem
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return nullptr;
	}

	// Spawn the Entity Actor in the world
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	SpawnParams.Instigator = nullptr;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APKEntityActor* EntityActor = GetWorld()->SpawnActor<APKEntityActor>(APKEntityActor::StaticClass(), Location, Rotation, SpawnParams);
	if (!EntityActor)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn APKEntityActor!"));
		return nullptr;
	}

	// Register the actor as an entity
	int32 EntityID = EntityManager->CreateEntity();
	EntityActor->SetEntityID(EntityID);

	// Add a Transform Component
	TSharedPtr<PKTransformComponent> TransformComponent = MakeShared<PKTransformComponent>();
	if (TransformComponent.IsValid())
	{
		TransformComponent->Position = Location;
		TransformComponent->Rotation = Rotation;
		TransformComponent->Scale = Scale;
		EntityManager->GetComponentManager().AddComponent<PKTransformComponent>(EntityID, TEXT("Transform"), TransformComponent);
		UE_LOG(LogTemp, Warning, TEXT("Added TransformComponent component to PKEntityActor: %d"), EntityID);
	}

	// Add a Physics Component
	TSharedPtr<PKPhysicsComponent> PhysicsComponent = MakeShared<PKPhysicsComponent>();
	if (PhysicsComponent.IsValid())
	{
		PhysicsComponent->Velocity = FVector::ZeroVector;
		PhysicsComponent->Acceleration = FVector::ZeroVector;
		PhysicsComponent->bIsSimulating = true;
		EntityManager->GetComponentManager().AddComponent<PKPhysicsComponent>(EntityID, TEXT("Physics"), PhysicsComponent);
		UE_LOG(LogTemp, Warning, TEXT("Added PhysicsComponent component to PKEntityActor: %d"), EntityID);
	}

	// Set the mesh transform
	if (EntityActor->EntityMesh)
	{
		EntityActor->EntityMesh->SetWorldLocationAndRotation(Location, Rotation);
		EntityActor->EntityMesh->SetWorldScale3D(Scale);
	}

	// Log the spawn
	UE_LOG(LogTemp, Warning, TEXT("Spawned PKEntityActor with Entity ID: %d at Location: %s"), EntityID, *Location.ToString());

	return EntityActor;
}

void UPKSpawningSubsystem::DespawnEntityActor(APKEntityActor* EntityActor)
{
	if (!EntityActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("DespawnEntityActor: EntityActor is null!"));
		return;
	}

	// Get the Entity Manager Subsystem
	UPKEntityManagerSubsystem* EntityManager = GetWorld()->GetSubsystem<UPKEntityManagerSubsystem>();
	if (!EntityManager)
	{
		UE_LOG(LogTemp, Error, TEXT("PKEntityManagerSubsystem not found!"));
		return;
	}

	// Get the Entity ID from the actor
	int32 EntityID = EntityActor->GetEntityID();

	// Destroy the entity in the Entity Manager
	if (EntityManager->DestroyEntity(EntityID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Successfully destroyed entity with ID: %d"), EntityID);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy entity with ID: %d"), EntityID);
	}

	// Destroy the actor in the world
	EntityActor->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Destroyed APKEntityActor with Entity ID: %d"), EntityID);
}