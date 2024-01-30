// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stack>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TMMaze.generated.h"

UCLASS(AutoExpandCategories = ("Settings", "Settings|Maze"))
class TREASUREMAZE_API ATMMaze : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ATMMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Settings")
	void GenerateMaze();

	void ClearStack();

	void RecursiveBackTracker();

protected:
	
	// Instanced static mesh component for tiles
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Maze")
	TObjectPtr<UInstancedStaticMeshComponent> ISMTileComponent;
	// Instanced static mesh component for walls
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Maze")
	TObjectPtr<UInstancedStaticMeshComponent> ISMWallComponent;
	// Width of maze
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Maze", meta = (MinClamp = "5", UIMin = "5"))
	int MaxWidth;
	// Height of maze
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Maze", meta = (MinClamp = "5", UIMin = "5"))
	int MaxHeight;
	// size of each cell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Maze", meta = (ClampMin = "10.0", UIMin = "10.0"))
	float TileSize;

	// Maze Array (later replace by a custom type)
	TArray<int> Maze;

	// Visited cell count
	int VisitedCellCount;

	// Stack for keeping history so we can backtrack
	std::stack<std::pair<int, int>> HistoryStack;

	// Determines which state the maze cell is in
	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};
};
