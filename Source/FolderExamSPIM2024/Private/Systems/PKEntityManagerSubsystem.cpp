// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKEntityManagerSubsystem.h"

int32 UPKEntityManagerSubsystem::CreateEntity()
{
	int32 NewID = NextEntityID++;
	Entities.Add(NewID);
	return NewID;
}

bool UPKEntityManagerSubsystem::DestroyEntity(int32 EntityID)
{
	Entities.Remove(EntityID);
	ComponentManager.RemoveAllComponents(EntityID);
	return true;
}