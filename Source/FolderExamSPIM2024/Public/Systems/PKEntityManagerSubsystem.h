// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKComponentManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "PKEntityManagerSubsystem.generated.h"

/**
 *
 */
UCLASS()
class FOLDEREXAMSPIM2024_API UPKEntityManagerSubsystem : public UWorldSubsystem
{  
	GENERATED_BODY()  

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	int32 CreateEntity();  

	bool DestroyEntity(int32 EntityID);  

	bool EntityExists(int32 EntityID) const;  

	PKComponentManager& GetComponentManager() { return ComponentManager; }  

	void MapEntityToActor(int32 EntityID, AActor* Actor);  

	AActor* GetActorForEntity(int32 EntityID) const;  

	/*  
	 * Variables  
	 */  
	int32 NextEntityID = 0;  
	TSet<int32> Entities;  
	PKComponentManager ComponentManager;
	UPROPERTY()
	TMap<int32, AActor*> EntityToActorMap;
};  
