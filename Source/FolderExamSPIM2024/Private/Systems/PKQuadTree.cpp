// Fill out your copyright notice in the Description page of Project Settings.

#include "Systems/PKQuadTree.h"

PKQuadTree::PKQuadTree(const FVector2D& InBoundsMin, const FVector2D& InBoundsMax, int32 InMaxEntitiesPerNode, int32 InMaxDepth) : MaxEntitiesPerNode(InMaxEntitiesPerNode), MaxDepth(InMaxDepth)
{
	RootNode = new PKQuadTreeNode(InBoundsMin, InBoundsMax);
}

PKQuadTree::~PKQuadTree()
{
	delete RootNode;
}

void PKQuadTree::Insert(int32 EntityID, const FVector2D& Position, float Radius)
{
	Insert(RootNode, EntityID, Position, Radius, 0);
}

void PKQuadTree::Remove(int32 EntityID, const FVector2D& Position, float Radius)
{
	Remove(RootNode, EntityID, Position, Radius);
}

void PKQuadTree::Query(const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const
{
	Query(RootNode, QueryMin, QueryMax, OutResults);
}

void PKQuadTree::Clear()
{
	delete RootNode;
	RootNode = new PKQuadTreeNode(RootNode->BoundsMin, RootNode->BoundsMax);
}

void PKQuadTree::Subdivide(PKQuadTreeNode* Node)
{
	FVector2D Center = (Node->BoundsMin + Node->BoundsMax) * 0.5f;

	Node->Children[0] = new PKQuadTreeNode(Node->BoundsMin, Center);
	Node->Children[1] = new PKQuadTreeNode(FVector2D(Center.X, Node->BoundsMin.Y), FVector2D(Node->BoundsMax.X, Center.Y));
	Node->Children[2] = new PKQuadTreeNode(FVector2D(Node->BoundsMin.X, Center.Y), FVector2D(Center.X, Node->BoundsMax.Y));
	Node->Children[3] = new PKQuadTreeNode(Center, Node->BoundsMax);
}

void PKQuadTree::Insert(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius, int32 Depth)
{
	if (Node == RootNode)
	{
		EntityPositions.Add(EntityID, Position);
	}

	if (Node->IsLeaf())
	{
		if (Node->EntityIDs.Num() < MaxEntitiesPerNode || Depth >= MaxDepth)
		{
			Node->EntityIDs.Add(EntityID);
			return;
		}

		Subdivide(Node);
	}

	FVector2D Center = (Node->BoundsMin + Node->BoundsMax) * 0.5f;

	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Overlaps(Child->BoundsMin, Child->BoundsMax, Position - FVector2D(Radius, Radius), Position + FVector2D(Radius, Radius)))
		{
			Insert(Child, EntityID, Position, Radius, Depth + 1);
		}
	}
}

void PKQuadTree::Remove(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius)
{
	if (Node == RootNode)
	{
		EntityPositions.Remove(EntityID);
	}

	if (Node->IsLeaf())
	{
		Node->EntityIDs.Remove(EntityID);
		return;
	}

	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Overlaps(Child->BoundsMin, Child->BoundsMax, Position - FVector2D(Radius, Radius), Position + FVector2D(Radius, Radius)))
		{
			Remove(Child, EntityID, Position, Radius);
		}
	}
}

void PKQuadTree::Query(PKQuadTreeNode* Node, const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const
{
	if (!Overlaps(Node->BoundsMin, Node->BoundsMax, QueryMin, QueryMax))
	{
		return;
	}

	OutResults.Append(Node->EntityIDs);

	if (Node->IsLeaf())
	{
		return;
	}

	for (PKQuadTreeNode* Child : Node->Children)
	{
		Query(Child, QueryMin, QueryMax, OutResults);
	}
}

void PKQuadTree::DrawDebug(UWorld* World, float Lifetime) const
{
	if (!World || !RootNode)
	{
		return;
	}

	DrawDebugNode(World, RootNode, Lifetime);

	for (const auto& Entity : EntityPositions)
	{
		const FVector2D& Position = Entity.Value;
		DrawDebugSphere(World, FVector(Position.X, Position.Y, 0.0f), 10.0f, 12, FColor::Green, false, Lifetime);
	}
}

void PKQuadTree::DrawDebugNode(UWorld* World, PKQuadTreeNode* Node, float Lifetime) const
{
	if (!Node)
	{
		return;
	}

	FVector Center = FVector((Node->BoundsMin.X + Node->BoundsMax.X) * 0.5f, (Node->BoundsMin.Y + Node->BoundsMax.Y) * 0.5f, 0.0f);
	FVector Extent = FVector((Node->BoundsMax.X - Node->BoundsMin.X) * 0.5f, (Node->BoundsMax.Y - Node->BoundsMin.Y) * 0.5f, 0.0f);

	DrawDebugBox(World, Center, Extent, FColor::Blue, false, Lifetime, 0, 2.0f);

	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Child)
		{
			DrawDebugNode(World, Child, Lifetime);
		}
	}
}

bool PKQuadTree::Overlaps(const FVector2D& MinA, const FVector2D& MaxA, const FVector2D& MinB, const FVector2D& MaxB) const
{
	return !(MinA.X > MaxB.X || MaxA.X < MinB.X || MinA.Y > MaxB.Y || MaxA.Y < MinB.Y);
}