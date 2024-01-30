// Fill out your copyright notice in the Description page of Project Settings.

#include "World/TMMaze.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Utility/TMUtility.h"

// Sets default values
ATMMaze::ATMMaze() : MaxWidth(5), MaxHeight(5), TileSize(100.0f), VisitedCellCount(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ISMTileComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM Tiles"));
	ISMTileComponent->SetupAttachment(RootComponent);
	
	ISMWallComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("ISM Walls"));
	ISMWallComponent->SetupAttachment(ISMTileComponent);
}

// Called when the game starts or when spawned
void ATMMaze::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATMMaze::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	const bool bShowDebug = UTMUtility::ShouldDisplayDebugForActor(this, FName{TEXTVIEW("TM.Maze")});
#endif
}

void ATMMaze::GenerateMaze()
{
	// Empty the stack And Array
	ClearStack();
	Maze.Empty();

	// Clear all the instances
	ISMTileComponent->ClearInstances();
	ISMWallComponent->ClearInstances();

	// Sets the size of array and zero it out
	Maze.SetNumZeroed(MaxWidth * MaxHeight, true);
	// Add the first tile to the history list
	HistoryStack.push(std::make_pair(0, 0));
	// Mark it as visited
	Maze[0] = CELL_VISITED;
	VisitedCellCount = 1;

	RecursiveBackTracker();

	// Reset the visited bit to 0
	for (int i = 0; i < Maze.Num(); i++)
	{
		Maze[i] = Maze[i] & ~(0b10000);
	}

	// Place Tiles
	for (int x = 0; x < MaxWidth; x++)
	{
		for (int y = 0; y < MaxHeight; y++)
		{
			auto GetNeighborViaOffset = [&](int a, int b) -> int
			{
				return (y+b) * MaxWidth + (x+a);
			};

			if (Maze[y * MaxWidth + x] & CELL_VISITED)
			{
				GLog->Log("Cell is already Visited!");
			}

			// Add tile meshes
			FVector TilePosition = FVector(y * TileSize, x * TileSize, 0.0f);
			int32 Index = ISMTileComponent->AddInstance(FTransform(FRotator::ZeroRotator, TilePosition, FVector::OneVector), false);

			// Add walls to the maze depending upon if there does not exist a path between nodes
			if ((Maze[y * MaxWidth + x] & CELL_PATH_N) == 0 && y < MaxHeight - 1 && (Maze[GetNeighborViaOffset(0,1)] & CELL_VISITED) == 0)
			{
				FRotator WallRotation = FRotator(0.0f, 90.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize + TileSize, x * TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			if ((Maze[y * MaxWidth + x] & CELL_PATH_S) == 0 && y > 0 && (Maze[GetNeighborViaOffset(0, -1)] & CELL_VISITED) == 0)
			{
				FRotator WallRotation = FRotator(0.0f, 90.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize - TileSize, x * TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			if ((Maze[y * MaxWidth + x] & CELL_PATH_E) == 0 && x < MaxWidth - 1 && (Maze[GetNeighborViaOffset(1, 0)] & CELL_VISITED) == 0)
			{
				FRotator WallRotation = FRotator(0.0f, 0.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize, x * TileSize + TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			if ((Maze[y * MaxWidth + x] & CELL_PATH_W) == 0 && x > 0 && (Maze[GetNeighborViaOffset(-1, 0)] & CELL_VISITED) == 0)
			{
				FRotator WallRotation = FRotator(0.0f, 0.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize, x * TileSize - TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}

			Maze[y * MaxWidth + x] |= CELL_VISITED;

			// Add walls to the corners
			if (x == 0)
			{
				FRotator WallRotation = FRotator(0.0f, 0.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize, x * TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			else if (y == 0)
			{
				FRotator WallRotation = FRotator(0.0f, -90.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize, x * TileSize + TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			
			if (x == MaxWidth - 1)
			{
				FRotator WallRotation = FRotator(0.0f, 0.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize, x * TileSize + TileSize, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
			if (y == MaxHeight - 1)
			{
				FRotator WallRotation = FRotator(0.0f, 90.0f, 0.0f);
				FVector WallLocation = FVector(y * TileSize + TileSize, x * TileSize - 0.0f, 0.0f);
				ISMWallComponent->AddInstance(FTransform(WallRotation, WallLocation, FVector::OneVector), false);
			}
		}
	}

}

void ATMMaze::ClearStack()
{
	while (!HistoryStack.empty())
	{
		HistoryStack.pop();
	}
}

void ATMMaze::RecursiveBackTracker()
{
	while (VisitedCellCount < MaxWidth * MaxHeight)
	{
		// Lambda for offset calculation
		auto Offset = [&](int x, int y)
		{
			// y * Width + x
			return (HistoryStack.top().second + y) * MaxWidth + (HistoryStack.top().first + x);
		};

		// Create a set of unvisited neighbors
		TArray<int> Neighbors;

		// check North Neighbor
		if (HistoryStack.top().second < MaxHeight - 1)
		{
			// Check if the node is unvisited
			if ((Maze[Offset(0, 1)] & CELL_VISITED) == 0)
			{
				// Add it to the list
				Neighbors.Add(0);
			}
		}
		// Check South Neighbor
		if ((HistoryStack.top().second > 0))
		{
			if((Maze[Offset(0, -1)] & CELL_VISITED) == 0)
			Neighbors.Add(1);
		}
		// Check East Neighbor
		if ((HistoryStack.top().first < MaxWidth - 1))
		{
			if((Maze[Offset(1, 0)] & CELL_VISITED) == 0)
			Neighbors.Add(2);
		}
		// Check West Neighbor
		if ((HistoryStack.top().first > 0))
		{
			if((Maze[Offset(-1, 0)] & CELL_VISITED) == 0)
			Neighbors.Add(3);
		}

		// Check if there are Neighbors
		if (!Neighbors.IsEmpty())
		{
			int NextDirection = Neighbors[FMath::Rand() % Neighbors.Num()];

			// Draw paths
			switch (NextDirection)
			{
			case 0: // North
				Maze[Offset(0, 0)] |= CELL_PATH_N; // Current Cell points to the north
				Maze[Offset(0, 1)] |= CELL_PATH_S; // the northern cell points to the south
				HistoryStack.push(std::make_pair(HistoryStack.top().first + 0, HistoryStack.top().second + 1)); // Push the northern neighbor into the history stack
				break;
			case 1: // South
				Maze[Offset(0, 0)] |= CELL_PATH_S; 
				Maze[Offset(0, -1)] |= CELL_PATH_N;
				HistoryStack.push(std::make_pair(HistoryStack.top().first + 0, HistoryStack.top().second - 1));
				break;
			case 2: // East
				Maze[Offset(0, 0)] |= CELL_PATH_E;
				Maze[Offset(1, 0)] |= CELL_PATH_W;
				HistoryStack.push(std::make_pair(HistoryStack.top().first + 1, HistoryStack.top().second + 0));
				break;
			case 3: // West
				Maze[Offset(0, 0)] |= CELL_PATH_W;
				Maze[Offset(-1, 0)] |= CELL_PATH_E;
				HistoryStack.push(std::make_pair(HistoryStack.top().first - 1, HistoryStack.top().second + 0));
				break;
			default:
				UE_LOG(LogTemp, Error, TEXT("Something Went Wrong In the Next Direction Int."));
				break;
			}

			Maze[Offset(0, 0)] |= CELL_VISITED;
			VisitedCellCount++;
		}
		else
		{
			// Back-track
			HistoryStack.pop();
		}
	}
}
