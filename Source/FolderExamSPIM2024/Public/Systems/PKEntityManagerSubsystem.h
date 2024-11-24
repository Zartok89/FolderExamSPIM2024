// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PKComponentMananger.h"
#include "PKEntityManagerSubsystem.generated.h"

/**
 *
 */
UCLASS()
class FOLDEREXAMSPIM2024_API UPKEntityManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EntityManager")
	int32 CreateEntity();

	UFUNCTION(BlueprintCallable, Category = "EntityManager")
	bool DestroyEntity(int32 EntityID);

	UFUNCTION(BlueprintCallable, Category = "EntityManager")
	const TSet<int32>& GetAllEntities() const { return Entities; }

	PKComponentMananger& GetComponentManager() { return ComponentManager; }

	/*
	 * Variables
	 */
	int32 NextEntityID = 0;
	TSet<int32> Entities;
	PKComponentMananger ComponentManager;
};
