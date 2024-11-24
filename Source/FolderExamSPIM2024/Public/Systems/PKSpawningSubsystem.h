// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PKEntityManagerSubsystem.h"
#include "Actors/PKEntityActor.h"
#include "PKSpawningSubsystem.generated.h"

/**
 *
 */
UCLASS()
class FOLDEREXAMSPIM2024_API UPKSpawningSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	APKEntityActor* SpawnEntityActorAtLocation(const FVector& Location, const FRotator& Rotation, const FVector& Scale, FVector Velocity, FVector Acceleration);

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void DespawnEntityActor(APKEntityActor* EntityActor);
};
