#include "Systems/PKLuaState.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Systems/PKPhysicsSubsystem.h"

UPKLuaState::UPKLuaState()
{ 
	Table.Add("SimpleString", FLuaValue("Test String"));
	Table.Add("GetPlayerPawnLocation", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, GetPlayerPawnLocation)));
	Table.Add("GetSimpleString", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, GetSimpleString)));
	Table.Add("SetEntityMaxSpeed", FLuaValue::Function(GET_FUNCTION_NAME_CHECKED(UPKLuaState, SetEntityMaxSpeed)));
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

FLuaValue UPKLuaState::GetSimpleString()
{
	return FLuaValue("Hello World");
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

FLuaValue UPKLuaState::PrintNumber(FLuaValue InNumber)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan, FString::Printf(TEXT("This is c++: %f"), InNumber.ToFloat()));
	return FLuaValue(InNumber.ToFloat()*2);
}