// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct PKQuadTreeNode
{
	PKQuadTreeNode(const FVector2D& InBoundsMin, const FVector2D& InBoundsMax) : BoundsMin(InBoundsMin), BoundsMax(InBoundsMax)
	{
		Children.SetNum(4);
	}

	~PKQuadTreeNode()
	{
		for (PKQuadTreeNode* Child : Children)
		{
			delete Child;
		}
	}

	bool IsLeaf() const
	{
		return Children[0] == nullptr;
	}

	/*
	 * Variables
	 */
	FVector2D BoundsMin;
	FVector2D BoundsMax;
	TArray<int32> EntityIDs;
	TArray<PKQuadTreeNode*> Children;
};

/**
 *
 */
class FOLDEREXAMSPIM2024_API PKQuadTree
{
public:
	PKQuadTree(const FVector2D& InBoundsMin, const FVector2D& InBoundsMax, int32 InMaxEntitiesPerNode = 4, int32 InMaxDepth = 5);
	~PKQuadTree();

	void Insert(int32 EntityID, const FVector2D& Position, float Radius);
	void Remove(int32 EntityID, const FVector2D& Position, float Radius);
	void Query(const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const;

	void Clear();

	void Subdivide(PKQuadTreeNode* Node);
	void Insert(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius, int32 Depth);
	void Remove(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius);
	void Query(PKQuadTreeNode* Node, const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const;

	void DrawDebug(UWorld* World, float Lifetime = 0.0f) const;
	void DrawDebugNode(UWorld* World, PKQuadTreeNode* Node, float Lifetime) const;

	bool Overlaps(const FVector2D& MinA, const FVector2D& MaxA, const FVector2D& MinB, const FVector2D& MaxB) const;

	/*
	 * Variables
	 */
	PKQuadTreeNode* RootNode;
	int32 MaxEntitiesPerNode;
	int32 MaxDepth;
	TMap<int32, FVector2D> EntityPositions;
};
