// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PKBaseEntity.h"
#include "GameFramework/Actor.h"
#include "PKEntity.generated.h"

UCLASS()
class FOLDEREXAMSPIM2024_API APKEntity : public AActor, public PKBaseEntity
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APKEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
