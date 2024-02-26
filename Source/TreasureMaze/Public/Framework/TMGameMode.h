// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Utility/TMLibrary.h"
#include "TMGameMode.generated.h"

class ATMPlayerController;
class ATMTreasure;
class ATMMaze;
class ATMCharacter;

USTRUCT(BlueprintType)
struct TREASUREMAZE_API FTMGameState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TM")
	float StartTime{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TM")
	float Duration{0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TM")
	int32 TotalActionsPerformed{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TM")
	float QTableAverage{0.f};
	
};

UCLASS()
class TREASUREMAZE_API ATMGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM GameMode")
	TSubclassOf<ATMCharacter> CharacterClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM GameMode")
	TSubclassOf<ATMMaze> MazeClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM GameMode")
	TSubclassOf<ATMTreasure> TreasureClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM GameMode")
	float IntervalDuration{5.f};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	TWeakObjectPtr<ATMCharacter> CharacterInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	TWeakObjectPtr<ATMMaze> MazeInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	TWeakObjectPtr<ATMTreasure> TreasureInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	TWeakObjectPtr<ATMPlayerController> TMPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	int32 TreasureTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM GameMode", Transient)
	FTMGameState TMGameState;
	
	bool bIsGameInProgress{false};

	bool bFirstMatch{true};

public:

	ATMGameMode();

	virtual void BeginPlay() override;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void StartNewSession();

	void EndSession();

	void OnCollectTreasure();

protected:

	void ChooseTreasureTile();
	void PositionCharacter() const;
	void PositionPawn() const;

	void SpawnCharacter();
	void SpawnTreasure();

	void StartGame();
	void EndGame();
	void WinGame();

	void ToggleEndSessionUI(const bool bInValue) const;

	void ToggleInput(const bool bInValue) const;

	void UpdateGameStateDuration();

public:

	UFUNCTION(BlueprintPure, Category = "TM GameMode")
	const FTMGameState& GetGameState() const { return TMGameState; }

	UFUNCTION(BlueprintPure, Category = "TM GameMode")
	ATMMaze* GetMaze() const { return MazeInstance.Get(); }

	UFUNCTION(BlueprintPure, Category = "TM GameMode")
	int32 GetTreasureTile() const { return TreasureTile; }

	UFUNCTION(BlueprintPure, Category = "TM GameMode")
	int32 IsGameInProgress() const { return bIsGameInProgress; }

public:

	void OnPerformAction();
	void OnFindTreasure(const TMap<int32, TMap<ETMAction, float>>& QTable);

private:

	FTimerHandle GameTimerHandle;
	
};
