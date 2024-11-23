#pragma once

#include "CoreMinimal.h"

class PKBaseComponent;

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKComponentMananger
{
public:
	PKComponentMananger();
	~PKComponentMananger();

	template <typename T>
	void AddComponent(int32 EntityID, const FName& ComponentName, TSharedPtr<T> Component)
	{
		EntityComponents.FindOrAdd(EntityID).Add(ComponentName, Component);
	}

	template <typename T>
	T* GetComponent(int32 EntityID, const FName& ComponentName)
	{
		if (EntityComponents.Contains(EntityID))
		{
			TMap<FName, TSharedPtr<PKBaseComponent>>& Components = EntityComponents[EntityID];
			if (Components.Contains(ComponentName))
			{
				return static_cast<T*>(Components[ComponentName].Get());
			}
		}
		return nullptr;
	}

	void RemoveComponent(int32 EntityID, const FName& ComponentName)
	{
		if (EntityComponents.Contains(EntityID))
		{
			EntityComponents[EntityID].Remove(ComponentName);
		}
	}

	void RemoveAllComponents(int32 EntityID)
	{
		EntityComponents.Remove(EntityID);
	}

	/*
	 * Variables
	 */
	TMap<int32, TMap<FName, TSharedPtr<PKBaseComponent>>> EntityComponents;
};
