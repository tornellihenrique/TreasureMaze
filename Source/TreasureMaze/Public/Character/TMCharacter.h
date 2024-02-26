// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TMCharacter.generated.h"

class ATMMaze;

UCLASS()
class TREASUREMAZE_API ATMCharacter : public ACharacter
{
	GENERATED_BODY()

	friend class ATMGameMode;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Character")
	float TileDistanceThreshold{100.0f};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Character", Transient)
	int32 CurrentTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Character", Transient)
	int32 LastTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Character", Transient)
	bool bIsPerformingAction{false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Character", Transient)
	TWeakObjectPtr<ATMMaze> MazeInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Character", Transient)
	int32 TreasureTile{0};

public:
	
	ATMCharacter();

	virtual void Tick(float DeltaTime) override;

	bool PerformAction(const int32 TargetTile);

	bool IsPerformingAction() const;
	int32 GetCurrentTile() const;

protected:
	
	virtual void BeginPlay() override;

	void RefreshPerformingAction(float DeltaTime);
	
};

inline bool ATMCharacter::IsPerformingAction() const
{
	return bIsPerformingAction;
}

inline int32 ATMCharacter::GetCurrentTile() const
{
	return CurrentTile;
}
