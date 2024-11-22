// Fill out your copyright notice in the Description page of Project Settings.


#include "PKEntity.h"

// Sets default values
APKEntity::APKEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APKEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APKEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

