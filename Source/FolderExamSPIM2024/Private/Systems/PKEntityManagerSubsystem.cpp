// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKEntityManagerSubsystem.h"

void UPKEntityManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("UPKEntityManagerSubsystem initialized."));
}

int32 UPKEntityManagerSubsystem::CreateEntity()
{
	int32 EntityID = NextEntityID++;
	Entities.Add(EntityID);
	return EntityID;
}

bool UPKEntityManagerSubsystem::DestroyEntity(int32 EntityID)
{
	if (!Entities.Contains(EntityID))
	{
		return false;
	}

	ComponentManager.RemoveAllComponents(EntityID);

	Entities.Remove(EntityID);

	EntityToActorMap.Remove(EntityID);

	return true;
}

bool UPKEntityManagerSubsystem::EntityExists(int32 EntityID) const
{
	return Entities.Contains(EntityID);
}

void UPKEntityManagerSubsystem::MapEntityToActor(int32 EntityID, AActor* Actor)
{
	if (Actor)
	{
		EntityToActorMap.Add(EntityID, Actor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to map Entity %d to a null Actor"), EntityID);
	}
}

AActor* UPKEntityManagerSubsystem::GetActorForEntity(int32 EntityID) const
{
	if (EntityToActorMap.Contains(EntityID))
	{
		AActor* Actor = EntityToActorMap[EntityID];
		return Actor;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Actor found for Entity %d"), EntityID);
		return nullptr;
	}
}