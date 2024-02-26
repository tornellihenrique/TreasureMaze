// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/TMAIController.h"

#include "Character/TMCharacter.h"
#include "Framework/TMGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utility/TMUtility.h"
#include "World/TMMaze.h"

ATMAIController::ATMAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATMAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!TMGameModeInstance.IsValid()) return;
	if (!CharacterInstance.IsValid()) return;
	if (!MazeInstance.IsValid()) return;
	
	if (!TMGameModeInstance->IsGameInProgress()) return;

#if WITH_EDITORONLY_DATA && ENABLE_DRAW_DEBUG
	bDisplayDebugTraces = UTMUtility::ShouldDisplayDebugForActor(this, FName{TEXTVIEW("TM.AIController")});

	if (bDisplayDebugTraces)
	{
		for (int32 Width = 0; Width < MaxWidth; Width++)
		{
			for (int32 Height = 0; Height < MaxHeight; Height++)
			{
				const int32 Index = Width * MaxHeight + Height;
			
				const TArray<FVector>& Tiles = MazeInstance->GetTiles();

				if (!Tiles.IsValidIndex(Index)) continue;

				const auto TileLocation = Tiles[Index];
			
				DrawDebugSphere(GetWorld(), TileLocation, 40.f, 10, TreasureTile == Index ? FColor::Yellow : VisitedTiles.Contains(Index) ? FColor::Red : FColor::Green);

				const auto QTableValue = QTable[Index];

				UKismetSystemLibrary::DrawDebugString(GetWorld(), TileLocation + FVector{100.f, 0.f, 0.f}, FString::Printf(TEXT("%.2f"), QTableValue[ETMAction::Up]), MazeInstance.Get());
				UKismetSystemLibrary::DrawDebugString(GetWorld(), TileLocation + FVector{-100.f, 0.f, 0.f}, FString::Printf(TEXT("%.2f"), QTableValue[ETMAction::Down]), MazeInstance.Get());
				UKismetSystemLibrary::DrawDebugString(GetWorld(), TileLocation + FVector{0.f, 100.f, 0.f}, FString::Printf(TEXT("%.2f"), QTableValue[ETMAction::Right]), MazeInstance.Get());
				UKismetSystemLibrary::DrawDebugString(GetWorld(), TileLocation + FVector{0.f, -100.f, 0.f}, FString::Printf(TEXT("%.2f"), QTableValue[ETMAction::Left]), MazeInstance.Get());
			}
		}
		
		DrawDebugSphere(GetWorld(), MazeInstance->GetTileLocationFromIndex(CurrentTile) + FVector{0.f, 0.f, 90.f}, 80.f, 10, FColor::Blue);
		DrawDebugSphere(GetWorld(), MazeInstance->GetTileLocationFromIndex(TargetTile) + FVector{0.f, 0.f, 90.f}, 70.f, 10, FColor::Yellow);
	}
#endif

	if (CharacterInstance->IsPerformingAction()) return;

	CurrentTile = CharacterInstance->GetCurrentTile();

	const auto TargetAction = ChooseAction();
	if (TargetAction == ETMAction::None) return;

	CurrentAction = TargetAction;

	if (!PerformAction(CurrentAction, TargetTile))
	{
		TargetTile = CurrentTile;
	}
	
	const float Reward = CalculateReward(CurrentTile, TargetTile, CurrentAction);
	
	UpdateQTable(CurrentTile, CurrentAction, TargetTile, Reward);

	VisitedTiles.Add(TargetTile);
}

void ATMAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CharacterInstance = CastChecked<ATMCharacter>(InPawn);
}

void ATMAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ATMAIController::BeginPlay()
{
	Super::BeginPlay();

	if (auto* TMGameMode = Cast<ATMGameMode>(GetWorld()->GetAuthGameMode()))
	{
		TMGameModeInstance = TMGameMode;
		
		MazeInstance = TMGameMode->GetMaze();
		TreasureTile = TMGameMode->GetTreasureTile();

		if (MazeInstance.IsValid())
		{
			MazeInstance->GetMazeSize(MaxWidth, MaxHeight);
			
			InitializeQTable();
		}
	}
}

ETMAction ATMAIController::ChooseAction() const
{
	if (!CharacterInstance.IsValid()) return ETMAction::None;
	
	TMap<ETMAction, float> PossibleActions = QTable[CurrentTile]; // Obtenha as possíveis ações e recompensas para o tile atual

	// Encontre a ação com a maior recompensa
	ETMAction BestAction = None;
	float MaxReward = -FLT_MAX;
	TArray<ETMAction> BestActions;

	for (const auto& Action : PossibleActions)
	{
		if (Action.Value > MaxReward)
		{
			MaxReward = Action.Value;
			BestAction = Action.Key;
			BestActions.Empty();
			BestActions.Add(BestAction);
		}
		else if (Action.Value == MaxReward)
		{
			BestActions.Add(Action.Key);
		}
	}

	// Se houver múltiplas ações com a mesma recompensa máxima, escolha uma aleatoriamente
	if (BestActions.Num() > 1)
	{
		const int32 RandomIndex = FMath::RandRange(0, BestActions.Num() - 1);
		BestAction = BestActions[RandomIndex];
	}

	return BestAction;
}

bool ATMAIController::PerformAction(ETMAction InTargetAction, int32& OutTargetTile)
{
	if (!MazeInstance.IsValid()) return false;

	if (TMGameModeInstance.IsValid()) TMGameModeInstance->OnPerformAction();
	
	const int32 NewTile = GetTargetTile(CurrentTile, InTargetAction);

	// Se for um movimento inválido não queremos realmente relizar o movimento.
	// Porém a recompensa ainda será contabilizada.
	
	if (MazeInstance->IsActionValid(CurrentTile, InTargetAction) && NewTile != -1)
	{
		CharacterInstance->PerformAction(NewTile);

		OutTargetTile = NewTile;

		return true;
	}

	return false;
}

int32 ATMAIController::GetTargetTile(int32 InCurrentTile, ETMAction InTargetAction) const
{
	if (!MazeInstance.IsValid()) return -1;
	
	FInt32Vector2 Offset{0, 0};

	switch (InTargetAction)
	{
		case Up: Offset.Y = 1; break;
		case Down: Offset.Y = -1; break;
		case Left: Offset.X = -1; break;
		case Right: Offset.X = 1; break;
		default:;
	}

	int32 Width, Height;
	if (!MazeInstance->GetTileWidthHeight(InCurrentTile, Width, Height)) return -1;

	return MazeInstance->GetTileIndex(Width + Offset.X, Height + Offset.Y);
}

float ATMAIController::CalculateReward(int32 InCurrentTile, int32 InTargetTile, ETMAction InTargetAction) const
{
	if (!MazeInstance.IsValid()) return 0.0f;
	
	// Verifica se a ação é válida
	if (!MazeInstance->IsActionValid(InCurrentTile, InTargetAction))
	{
		return InvalidActionReward; // Penalidade por tentar mover contra uma parede
	}

	// Verifica se o tesouro foi encontrado
	if (TreasureTile == InTargetTile)
	{
		if (TMGameModeInstance.IsValid()) TMGameModeInstance->OnFindTreasure(QTable);
		
		return TreasureFoundActionReward; // Recompensa máxima por encontrar o tesouro
	}

	// Verifica se o slot já foi visitado
	if (VisitedTiles.Contains(InTargetTile))
	{
		return VisitedSlotActionReward; // Penalidade por revisitar um slot
	}

	// Ação comum
	return UsualActionReward; // Penalidade padrão para desencorajar movimentos aleatórios
}

void ATMAIController::InitializeQTable()
{
	for (int32 Width = 0; Width < MaxWidth; Width++)
	{
		for (int32 Height = 0; Height < MaxHeight; Height++)
		{
			TMap<ETMAction, float> ActionRewards{{ETMAction::Up, 0.0f},{ETMAction::Down, 0.0f},{ETMAction::Left, 0.0f},{ETMAction::Right, 0.0f}};
			
			QTable.Add(Width * MaxHeight + Height, ActionRewards);
		}
	}
}

/*
	Função responsável por atualizar os valores na QTable com base na ação tomada e na recompensa recebida.
	Utiliza a fórmula de atualização do Q-learning para ajustar o valor Q(s, a) para o estado atual 's' e ação 'a'.
	A fórmula é: Q(s, a) = Q(s, a) + α[R(s, a) + γ * max(Q(s', a')) - Q(s, a)],
	onde α é a taxa de aprendizado, γ é o fator de desconto, R(s, a) é a recompensa recebida,
	e max(Q(s', a')) é o valor máximo da QTable para o próximo estado 's''.
	A função calcula o novo valor Q e o armazena de volta na QTable.
*/
void ATMAIController::UpdateQTable(int32 InCurrentTile, ETMAction InTargetAction, int32 InTargetTile, float Reward)
{
	const float OldQValue = QTable[InCurrentTile][InTargetAction];
	const float MaxNextQValue = GetMaxQValue(InTargetTile);

	const float NewQValue = OldQValue + Alpha * (Reward + Gamma * MaxNextQValue - OldQValue);
	QTable[InCurrentTile][InTargetAction] = NewQValue;
}

float ATMAIController::GetMaxQValue(int32 InTargetTile) const
{
	float MaxQValue = -FLT_MAX;
	const TMap<ETMAction, float>& Actions = QTable[InTargetTile];

	for (const auto& Action : Actions)
	{
		if (Action.Value > MaxQValue)
		{
			MaxQValue = Action.Value;
		}
	}

	return MaxQValue;
}
