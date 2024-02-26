// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Utility/TMLibrary.h"
#include "TMAIController.generated.h"

class ATMGameMode;
class ATMCharacter;
class ATMMaze;

UCLASS()
class TREASUREMAZE_API ATMAIController : public AAIController
{
	GENERATED_BODY()

protected:

	// Taxa de aprendizado: determina o quão rápido a IA atualiza seus valores na QTable.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller")
	float Alpha{0.1f};

	// Fator de desconto: determina a importância das recompensas futuras na decisão atual.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller")
	float Gamma{0.9};

	// Recompensa por encontrar o tesouro
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller", Meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float TreasureFoundActionReward{1.0f};

	// Recompensa por tentar se mover contra a parede
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller", Meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float InvalidActionReward{-0.75f};

	// Recompensa por se mover para um local já visitado
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller", Meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float VisitedSlotActionReward{-0.25f};

	// Recompensa comum de movimento
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM AI Controller", Meta = (ClampMin = "-1.0", ClampMax = "1.0"))
	float UsualActionReward{-0.04f};

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	bool bDisplayDebugTraces{false};
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	TWeakObjectPtr<ATMCharacter> CharacterInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	TWeakObjectPtr<ATMMaze> MazeInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	int32 TreasureTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	int32 CurrentTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	int32 TargetTile{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	TEnumAsByte<ETMAction> CurrentAction{ETMAction::None};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM AI Controller", Transient)
	TWeakObjectPtr<ATMGameMode> TMGameModeInstance;

public:

	ATMAIController();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:

	virtual void BeginPlay() override;

	ETMAction ChooseAction() const;
	bool PerformAction(ETMAction InTargetAction, int32& OutTargetTile);

	int32 GetTargetTile(int32 InCurrentTile, ETMAction InTargetAction) const;
	float CalculateReward(int32 InCurrentTile, int32 InTargetTile, ETMAction InTargetAction) const;
	
	void InitializeQTable();
	void UpdateQTable(int32 InCurrentTile, ETMAction InTargetAction, int32 InTargetTile, float Reward);
	float GetMaxQValue(int32 InTargetTile) const;

private:
	
	TMap<int32, TMap<ETMAction, float>> QTable;

	TSet<int32> VisitedTiles;
	
	int32 MaxWidth;
	int32 MaxHeight;
	
	// Debug

public:
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;

private:
	
	static void DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor, float Scale, float HorizontalLocation, float& VerticalLocation);
	void DisplayDebugValues(const UCanvas* Canvas, float Scale, float HorizontalLocation, float& VerticalLocation) const;

};
