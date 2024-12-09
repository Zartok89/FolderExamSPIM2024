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
	FLuaValue SpawnEntity(FLuaValue InLocationX, FLuaValue InLocationY, FLuaValue InVelocityX, FLuaValue InVelocityY, FLuaValue InAccelerationX, FLuaValue InAccelerationY);

	UFUNCTION()
	FLuaValue SetEntityMaxSpeed(FLuaValue InMaxSpeed);

	UFUNCTION()
	FLuaValue PrintNumber(FLuaValue InNumber);

	UFUNCTION(BlueprintCallable)
	void StartTimer();

	void StopTimer();

	void SpawnTimer();

	/*
	 * Variables
	 */
	float SpawnTimeDelay = 2.f;
	FTimerHandle LuaTimerHandle;
	bool bCanSpawn = false;
};
