// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <stack>

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utility/TMLibrary.h"
#include "TMMaze.generated.h"

UCLASS(Abstract, AutoExpandCategories = ("Settings", "Settings|Maze"))
class TREASUREMAZE_API ATMMaze : public AActor
{
	GENERATED_BODY()

public:
	
	ATMMaze();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Settings")
	void GenerateMaze();

	void ClearStack();

	void RecursiveBackTracker();

	void RefreshMazeTiles();

	UFUNCTION(BlueprintPure, Category = "Maze")
	const TArray<FVector>& GetTiles() const { return MazeTiles; }

	UFUNCTION(BlueprintPure, Category = "Maze")
	const FVector& GetTileLocationFromWidthHeight(const int32 InWidth, const int32 InHeight) const;

	UFUNCTION(BlueprintPure, Category = "Maze")
	const FVector& GetTileLocationFromIndex(const int32 InIndex) const;

	UFUNCTION(BlueprintPure, Category = "Maze")
	int32 GetTileIndex(const int32 InWidth, const int32 InHeight) const;

	UFUNCTION(BlueprintPure, Category = "Maze")
	bool GetTileWidthHeight(const int32 InIndex, int32& OutWidth, int32& OutHeight) const;

	FIntVector2 GetTileWidthHeight(const int32 InIndex) const;

	UFUNCTION(BlueprintPure, Category = "Maze")
	bool IsTileValid(const int32 InIndex) const { return MazeTiles.IsValidIndex(InIndex); }

	UFUNCTION(BlueprintPure, Category = "Maze")
	void GetMazeSize(int32& OutWidth, int32& OutHeight) const { OutWidth = MaxWidth; OutHeight = MaxHeight; }

	UFUNCTION(BlueprintPure, Category = "Maze")
	bool IsActionValid(const int32 TargetTile, const ETMAction TargetAction) const;

protected:
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Maze")
	TObjectPtr<USceneComponent> VisualRoot;
	
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

	// Maze Array
	TArray<int> Maze;

	// Maze Tile Array (points to each Tile's center world position)
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Maze")
	TArray<FVector> MazeTiles;

	// Visited cell count
	int VisitedCellCount;

	// Stack for keeping history so we can backtrack
	std::stack<std::pair<int, int>> HistoryStack;
};
