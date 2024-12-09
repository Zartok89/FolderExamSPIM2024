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

	// Delete the existing root node to reset the tree
	delete RootNode;

	MaxEntitiesPerNode = InMaxEntitiesPerNode;
	MaxDepth = InMaxDepth;

	// Recreate the root node with new bounds
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
	// Calculate the center of the node's bounds
	FVector2D Center = (Node->BoundsMin + Node->BoundsMax) * 0.5f;

	// Create four child nodes with appropriate bounds
	Node->Children[0] = new PKQuadTreeNode(Node->BoundsMin, Center);
	Node->Children[1] = new PKQuadTreeNode(FVector2D(Center.X, Node->BoundsMin.Y), FVector2D(Node->BoundsMax.X, Center.Y));
	Node->Children[2] = new PKQuadTreeNode(FVector2D(Node->BoundsMin.X, Center.Y), FVector2D(Center.X, Node->BoundsMax.Y));
	Node->Children[3] = new PKQuadTreeNode(Center, Node->BoundsMax);

	// Assign friction coefficients to the child nodes
	if (Node == RootNode)
	{
		Node->Children[0]->FrictionCoefficient = 0.99f; // BL
		Node->Children[1]->FrictionCoefficient = 1.02f; // TL
		Node->Children[2]->FrictionCoefficient = 1.05f; // BR
		Node->Children[3]->FrictionCoefficient = 0.94f; // TR
	}
	else
	{
		// Inherit the friction coefficient from the parent node
		for (int i = 0; i < 4; ++i)
		{
			Node->Children[i]->FrictionCoefficient = Node->FrictionCoefficient;
		}
	}
}

void UPKQuadTree::Insert(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius, int32 Depth)
{
	// If inserting into the root node, track the entity's position
	if (Node == RootNode)
	{
		EntityPositions.Add(EntityID, Position);
	}

	if (Node->IsLeaf())
	{
		// If the node can hold more entities or the max depth is reached add the entity to the node
		if (Node->EntityIDs.Num() < MaxEntitiesPerNode || Depth >= MaxDepth)
		{
			Node->EntityIDs.Add(EntityID);
			return;
		}

		// Otherwise, subdivide the node
		Subdivide(Node);
	}

	// Insert the entity into all child nodes that overlap with it
	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Overlaps(Child->BoundsMin, Child->BoundsMax, Position - FVector2D(Radius, Radius), Position + FVector2D(Radius, Radius)))
		{
			Insert(Child, EntityID, Position, Radius, Depth + 1);
		}
	}
}

// Recursive function to remove an entity from the QuadTree
void UPKQuadTree::Remove(PKQuadTreeNode* Node, int32 EntityID, const FVector2D& Position, float Radius)
{
	// If removing from the root node, remove the entity's position
	if (Node == RootNode)
	{
		EntityPositions.Remove(EntityID);
	}

	// If the node is a leaf, remove the entity from the node
	if (Node->IsLeaf())
	{
		Node->EntityIDs.Remove(EntityID);
		return;
	}

	// Remove the entity from all child nodes that overlap with it
	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Overlaps(Child->BoundsMin, Child->BoundsMax, Position - FVector2D(Radius, Radius), Position + FVector2D(Radius, Radius)))
		{
			Remove(Child, EntityID, Position, Radius);
		}
	}
}

// Recursive function to query the QuadTree for entities within a specific area
void UPKQuadTree::Query(PKQuadTreeNode* Node, const FVector2D& QueryMin, const FVector2D& QueryMax, TArray<int32>& OutResults) const
{
	// If the query area doesn't overlap with the node, return
	if (!Overlaps(Node->BoundsMin, Node->BoundsMax, QueryMin, QueryMax))
	{
		return;
	}

	// Add the entities in this node to the results
	OutResults.Append(Node->EntityIDs);

	// If the node is a leaf, stop the recursion
	if (Node->IsLeaf())
	{
		return;
	}

	// Query all child nodes
	for (PKQuadTreeNode* Child : Node->Children)
	{
		Query(Child, QueryMin, QueryMax, OutResults);
	}
}

// Draw debug visualization of the QuadTree
void UPKQuadTree::DrawDebug(UWorld* World, float Lifetime) const
{
	if (!World || !RootNode)
	{
		return;
	}

	DrawDebugNode(World, RootNode, Lifetime);
}

// Recursive function to draw a debug visualization of a node
void UPKQuadTree::DrawDebugNode(UWorld* World, PKQuadTreeNode* Node, float Lifetime) const
{
	if (!Node)
	{
		return;
	}

	FVector Center = FVector((Node->BoundsMin.X + Node->BoundsMax.X) * 0.5f, (Node->BoundsMin.Y + Node->BoundsMax.Y) * 0.5f, 0.0f);
	FVector Extent = FVector((Node->BoundsMax.X - Node->BoundsMin.X) * 0.5f, (Node->BoundsMax.Y - Node->BoundsMin.Y) * 0.5f, 0.0f);

	// Draw the node's bounds as a debug box
	DrawDebugBox(World, Center, Extent, FColor::Purple, false, Lifetime, 0, 40.0f);

	// Draw all child nodes
	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Child)
		{
			DrawDebugNode(World, Child, Lifetime);
		}
	}
}

// Check if two bounding boxes overlap
bool UPKQuadTree::Overlaps(const FVector2D& MinA, const FVector2D& MaxA, const FVector2D& MinB, const FVector2D& MaxB) const
{
	return !(MinA.X > MaxB.X || MaxA.X < MinB.X || MinA.Y > MaxB.Y || MaxA.Y < MinB.Y);
}

// Get the friction coefficient for a specific position
float UPKQuadTree::GetFrictionForPosition(const FVector2D& Position) const
{
	// Find the node containing the position
	PKQuadTreeNode* Node = GetNodeForPosition(RootNode, Position);
	if (Node)
	{
		return Node->FrictionCoefficient;
	}
	return 1.0f;
}

// Find the node that contains a specific position
PKQuadTreeNode* UPKQuadTree::GetNodeForPosition(PKQuadTreeNode* Node, const FVector2D& Position) const
{
	// If the node doesn't exist or doesn't contain the position, return null
	if (!Node || !Overlaps(Node->BoundsMin, Node->BoundsMax, Position, Position))
	{
		return nullptr;
	}

	// If the node is a leaf, return it
	if (Node->IsLeaf())
	{
		return Node;
	}

	// Search in child nodes
	for (PKQuadTreeNode* Child : Node->Children)
	{
		if (Child && Overlaps(Child->BoundsMin, Child->BoundsMax, Position, Position))
		{
			return GetNodeForPosition(Child, Position);
		}
	}

	return nullptr;
}