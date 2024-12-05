// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LuaState.h"
#include "PKLuaState.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FOLDEREXAMSPIM2024_API UPKLuaState : public ULuaState
{
	GENERATED_BODY()

public:
	UPKLuaState();

	UFUNCTION()
	FLuaValue GetPlayerPawnLocation();

	UFUNCTION()
	FLuaValue GetSimpleString();

	UFUNCTION()
	FLuaValue SetEntityMaxSpeed(FLuaValue InMaxSpeed);

	UFUNCTION()
	FLuaValue PrintNumber(FLuaValue InNumber);

	/*
	 * Variables
	 */
};
