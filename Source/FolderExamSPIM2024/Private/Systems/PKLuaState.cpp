#include "Systems/PKLuaState.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Systems/PKPhysicsSubsystem.h"
#include "Systems/PKSpawningSubsystem.h"
#include "TimerManager.h"

UPKLuaState::UPKLuaState()
{
	Table.Add("GetPlayerPawnLocation", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, GetPlayerPawnLocation)));
	Table.Add("SetEntityMaxSpeed", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, SetEntityMaxSpeed)));
	Table.Add("SpawnEntity", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, SpawnEntity)));
	Table.Add("PrintNumber", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, PrintNumber)));
}

FLuaValue UPKLuaState::GetPlayerPawnLocation()
{
	FVector Location = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	FLuaValue LocalTable = CreateLuaTable();
	LocalTable.SetField("x", FLuaValue(Location.X));
	LocalTable.SetField("y", FLuaValue(Location.Y));
	LocalTable.SetField("z", FLuaValue(Location.Z));

	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("Player Position: %f, %f, %f"), Location.X, Location.Y, Location.Z));

	return LocalTable;
}

FLuaValue UPKLuaState::SetEntityMaxSpeed(FLuaValue InMaxSpeed)
{
	if (UWorld* World = GetWorld())
	{
		if (UPKPhysicsSubsystem* PhysicsSubsystem = World->GetSubsystem<UPKPhysicsSubsystem>())
		{
			PhysicsSubsystem->MaxVelocity = InMaxSpeed.ToFloat();
			return FLuaValue(PhysicsSubsystem->MaxVelocity);
		}
	}
	return FLuaValue();
}

FLuaValue UPKLuaState::SpawnEntity(FLuaValue InLocationX, FLuaValue InLocationY, FLuaValue InVelocityX, FLuaValue InVelocityY, FLuaValue InAccelerationX, FLuaValue InAccelerationY)
{
	if (bCanSpawn)
	{
		if (UWorld* World = GetWorld())
		{
			if (UPKSpawningSubsystem* SpawningSubsystem = World->GetSubsystem<UPKSpawningSubsystem>())
			{
				float LocationX = InLocationX.ToFloat();
				float LocationY = InLocationY.ToFloat();
				float VelocityX = InVelocityX.ToFloat();
				float VelocityY = InVelocityY.ToFloat();
				float AccelerationX = InAccelerationX.ToFloat();
				float AccelerationY = InAccelerationY.ToFloat();
				SpawningSubsystem->SpawnEntityActorAtLocation({ LocationX, LocationY, -30.f }, { 0, 0, 0 }, { 1.f, 1.f, 1.f }, { VelocityX, VelocityY, 0.f }, { AccelerationX, AccelerationY, 0.f }, 1.f, 50.f);
				bCanSpawn = false;
				return FLuaValue();
			}
		}
	}

	return FLuaValue();
}

FLuaValue UPKLuaState::PrintNumber(FLuaValue InNumber)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("This is c++: %f"), InNumber.ToFloat()));
	return FLuaValue(InNumber.ToFloat() * 2);
}

void UPKLuaState::StartTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		LuaTimerHandle,
		this,
		&UPKLuaState::SpawnTimer,
		SpawnTimeDelay,
		true
	);
}

void UPKLuaState::StopTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(LuaTimerHandle);
}

void UPKLuaState::SpawnTimer()
{
	bCanSpawn = !bCanSpawn;
}