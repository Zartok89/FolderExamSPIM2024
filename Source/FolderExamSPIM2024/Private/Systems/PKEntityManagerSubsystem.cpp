// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKEntityManagerSubsystem.h"

int32 UPKEntityManagerSubsystem::CreateEntity()
{
	int32 NewID = NextEntityID++;
	Entities.Add(NewID);

	UE_LOG(LogTemp, Warning, TEXT("Created new entity with ID: %d"), NewID);
	return NewID;
}

bool UPKEntityManagerSubsystem::DestroyEntity(int32 EntityID)
{
	if (!Entities.Contains(EntityID))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to destroy entity: Entity with ID %d does not exist!"), EntityID);
		return false;
	}

	Entities.Remove(EntityID);

	ComponentManager.RemoveAllComponents(EntityID);

	UE_LOG(LogTemp, Warning, TEXT("Destroyed entity with ID: %d"), EntityID);
	return true;
}

bool UPKEntityManagerSubsystem::EntityExists(int32 EntityID) const
{
	return Entities.Contains(EntityID);
}