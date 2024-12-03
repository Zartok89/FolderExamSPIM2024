#include "Systems/UPKQuadTree.h"

UPKQuadTree::UPKQuadTree()
{
}

void UPKQuadTree::Initialize(const FVector2D& InBoundsMin, const FVector2D& InBoundsMax, int& InMaxEntitiesPerNode, int& InMaxDepth)
{
	MaxEntitiesPerNode = InMaxEntitiesPerNode;
	MaxDepth = InMaxDepth;
	RootNode = new PKQuadTreeNode(InBoundsMin, InBoundsMax);
}

void UPKQuadTree::Update(const FVector2D& InBoundsMin, const FVector2D& InBoundsMax, int& InMaxEntitiesPerNode, int& InMaxDepth)
{
	UE_LOG(LogTemp, Log, TEXT("QuadTree Update called with MaxEntitiesPerNode: %d, MaxDepth: %d"), InMaxEntitiesPerNode, InMaxDepth);

	if (RootNode == nullptr) { return; }
	delete RootNode;

	MaxEntitiesPerNode = InMaxEntitiesPerNode;
	MaxDepth = InMaxDepth;

	RootNode = new PKQuadTreeNode(InBoundsMin, InBoundsMax);
}

void UPKQuadTree::Insert(int32 EntityID, const FVector2D& Position, float Radius)
{
	Insert(RootNode, EntityID, Position, Radius, 0);
}

void UPKQuadTree::Remove(int32 EntityID, const FVector2D& Position, float Radius)
{
	Remove(RootNode, EntityID, Position, Radius);
}

void UPKQuadTree::Query(const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const
{
	Query(RootNode, QueryMin, QueryMax, OutResults);
}

void UPKQuadTree::Clear()
{
	if (RootNode == nullptr)
	{
		return;
	}
	delete RootNode;
	RootNode = new PKQuadTreeNode(RootNode->BoundsMin, RootNode->BoundsMax);
}

void UPKQuadTree::Subdivide(PKQuadTreeNode* Node)
{
	FVector2D Center = (Node->BoundsMin + Node->BoundsMax) * 0.5f;

	Node->Children[0] = new PKQuadTreeNode(Node->BoundsMin, Center);
	Node->Children[1] = new PKQuadTreeNode(FVector2D(Center.X, Node->BoundsMin.Y), FVector2D(Node->BoundsMax.X, Center.Y));
	Node->Children[2] = new PKQuadTreeNode(FVector2D(Node->BoundsMin.X, Center.Y), FVector2D(Center.X, Node->BoundsMax.Y));
	Node->Children[3] = new PKQuadTreeNode(Center, Node->BoundsMax);

	if (Node == RootNode)
	{
		Node->Children[0]->FrictionCoefficient = 1.02f; // BL
		Node->Children[1]->FrictionCoefficient = 1.15f; // TL
		Node->Children[2]->FrictionCoefficient = 1.1f; // BR
		Node->Children[3]->FrictionCoefficient = 1.03f; // TR
	}
	else
	{
		for (int i = 0; i < 4; ++i)
		{
			Node->Children[i]->FrictionCoefficient = Node->FrictionCoefficient;
		}
	}
}

void UPKQuadTree::Insert(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius, int32 Depth)
{
	if (Node == RootNode)
	{
		EntityPositions.Add(EntityID, Position);
	}

	//UE_LOG(LogTemp, Log, TEXT("Insert: MaxEntitiesPerNode = %d, MaxDepth = %d"), MaxEntitiesPerNode, MaxDepth);

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

void UPKQuadTree::Remove(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius)
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

void UPKQuadTree::Query(PKQuadTreeNode* Node, const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const
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

void UPKQuadTree::DrawDebug(UWorld* World, float Lifetime) const
{
	if (!World || !RootNode)
	{
		return;
	}

	DrawDebugNode(World, RootNode, Lifetime);

	for (const auto& Entity : EntityPositions)
	{
		const FVector2D& Position = Entity.Value;
		DrawDebugSphere(World, FVector(Position.X, Position.Y, 35.0f), 50.0f, 12, FColor::Green, false, Lifetime);
	}
}

void UPKQuadTree::DrawDebugNode(UWorld* World, PKQuadTreeNode* Node, float Lifetime) const
{
	if (!Node)
	{
		return;
	}

	FVector Center = FVector((Node->BoundsMin.X + Node->BoundsMax.X) * 0.5f, (Node->BoundsMin.Y + Node->BoundsMax.Y) * 0.5f, 0.0f);
	FVector Extent = FVector((Node->BoundsMax.X - Node->BoundsMin.X) * 0.5f, (Node->BoundsMax.Y - Node->BoundsMin.Y) * 0.5f, 0.0f);

	//DrawDebugBox(World, Center, Extent, FColor::Purple, false, Lifetime, 0, 40.0f);

	FColor Color = FColor::MakeRedToGreenColorFromScalar(Node->FrictionCoefficient);
	DrawDebugBox(World, Center, Extent, Color, false, Lifetime, 0, 40.0f);

	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Child)
		{
			DrawDebugNode(World, Child, Lifetime);
		}
	}
}

bool UPKQuadTree::Overlaps(const FVector2D& MinA, const FVector2D& MaxA, const FVector2D& MinB, const FVector2D& MaxB) const
{
	return !(MinA.X > MaxB.X || MaxA.X < MinB.X || MinA.Y > MaxB.Y || MaxA.Y < MinB.Y);
}

float UPKQuadTree::GetFrictionForPosition(const FVector2D& Position) const
{
	PKQuadTreeNode* Node = GetNodeForPosition(RootNode, Position);
	if (Node)
	{
		return Node->FrictionCoefficient;
	}
	return 1.0f;
}

PKQuadTreeNode* UPKQuadTree::GetNodeForPosition(PKQuadTreeNode* Node, const FVector2D& Position) const
{
	if (!Node || !Overlaps(Node->BoundsMin, Node->BoundsMax, Position, Position))
	{
		return nullptr;
	}

	if (Node->IsLeaf())
	{
		return Node;
	}

	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Child && Overlaps(Child->BoundsMin, Child->BoundsMax, Position, Position))
		{
			return GetNodeForPosition(Child, Position);
		}
	}

	return nullptr;
}