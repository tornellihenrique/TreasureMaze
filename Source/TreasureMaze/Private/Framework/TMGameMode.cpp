// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/TMGameMode.h"

#include "AIController.h"
#include "Character/TMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TMPlayer.h"
#include "Player/TMPlayerController.h"
#include "UI/TMHUD.h"
#include "World/TMMaze.h"
#include "World/TMTreasure.h"

ATMGameMode::ATMGameMode()
{
	bStartPlayersAsSpectators = false;
}

void ATMGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ATMGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	StartNewSession();
}

AActor* ATMGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (!MazeInstance.IsValid()) return Super::ChoosePlayerStart_Implementation(Player);

	const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

	if (MazeTiles.Num() <= 0) return Super::ChoosePlayerStart_Implementation(Player);
	
	const auto DummyActor = GetWorld()->SpawnActor<AActor>(MazeTiles[0], FRotator::ZeroRotator);

	return DummyActor;
}

void ATMGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (auto* TMPC = Cast<ATMPlayerController>(NewPlayer))
	{
		TMPlayerController = TMPC;

		if (CharacterInstance.IsValid())
		{
			if (ATMPlayer* Player = TMPlayerController->GetPawn<ATMPlayer>())
			{
				Player->SetCharacter(CharacterInstance.Get());
			}
		}
	}
}

void ATMGameMode::StartNewSession()
{
	if (MazeInstance.IsValid())
	{
		MazeInstance->Destroy();
		MazeInstance.Reset();
	}
	
	MazeInstance = GetWorld()->SpawnActor<ATMMaze>(MazeClass);

	if (MazeInstance.IsValid())
	{
		MazeInstance->GenerateMaze();
		
		ChooseTreasureTile();
		SpawnCharacter();

		StartGame();
	}
}

void ATMGameMode::EndSession()
{
	if (MazeInstance.IsValid())
	{
		MazeInstance->Destroy();
		MazeInstance.Reset();
	}
}

void ATMGameMode::OnCollectTreasure()
{
	if (bIsGameInProgress)
	{
		if (TreasureInstance.IsValid())
		{
			TreasureInstance->Destroy();
			TreasureInstance.Reset();
		}
		
		WinGame();
	}
}

void ATMGameMode::ChooseTreasureTile()
{
	if (MazeInstance.IsValid())
	{
		const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

		if (MazeTiles.Num() > 0)
		{
			TreasureTile = FMath::RandRange(0, MazeTiles.Num() - 1);
		}
	}
}

void ATMGameMode::PositionCharacter() const
{
	if (MazeInstance.IsValid() && CharacterInstance.IsValid())
	{
		const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

		if (MazeTiles.Num() > 0)
		{
			CharacterInstance->SetActorLocation(MazeTiles[0] + FVector{0.f, 0.f, 90.f});
			CharacterInstance->CurrentTile = 0;
			CharacterInstance->LastTile = 0;
				
			CharacterInstance->MazeInstance = MazeInstance;
			CharacterInstance->TreasureTile = TreasureTile;
		}
	}
}

void ATMGameMode::PositionPawn() const
{
	if (TMPlayerController.IsValid())
	{
		if (ATMPlayer* Player = TMPlayerController->GetPawn<ATMPlayer>())
		{
			const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

			if (MazeTiles.Num() > 0)
			{
				Player->SetActorLocation(MazeTiles[0] + FVector{0.f, 0.f, 90.f});
			}
		}
	}
}

void ATMGameMode::SpawnCharacter()
{
	if (MazeInstance.IsValid())
	{
		const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

		static constexpr int32 TargetTile = 0;

		if (MazeTiles.IsValidIndex(TargetTile))
		{
			if (ATMCharacter* SpawnedCharacter = GetWorld()->SpawnActorDeferred<ATMCharacter>(CharacterClass, {FRotator::ZeroRotator, MazeTiles[TargetTile] + FVector{0.f, 0.f, 90.f}}))
			{
				SpawnedCharacter->CurrentTile = TargetTile;
				SpawnedCharacter->LastTile = TargetTile;
				
				SpawnedCharacter->MazeInstance = MazeInstance;
				SpawnedCharacter->TreasureTile = TreasureTile;

				CharacterInstance = SpawnedCharacter;
			}
		}
	}
}

void ATMGameMode::SpawnTreasure()
{
	if (MazeInstance.IsValid())
	{
		const TArray<FVector> MazeTiles = MazeInstance->GetTiles();

		if (MazeTiles.Num() > 0)
		{
			if (TreasureInstance.IsValid())
			{
				TreasureInstance->Destroy();
				TreasureInstance.Reset();
			}
			
			TreasureInstance = GetWorld()->SpawnActor<ATMTreasure>(TreasureClass, MazeTiles[TreasureTile], FRotator::ZeroRotator);
		}
	}
}

void ATMGameMode::StartGame()
{
	ToggleEndSessionUI(false);
	
    bIsGameInProgress = true;

	TMGameState.Duration = 0.f;
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &ThisClass::UpdateGameStateDuration, 1.f, true);
	
	PositionCharacter();

	if (bFirstMatch) PositionPawn();
	
	SpawnTreasure();

	ToggleInput(true);

	bFirstMatch = false;
}

void ATMGameMode::EndGame()
{
	bIsGameInProgress = false;
	
	GetWorldTimerManager().ClearTimer(GameTimerHandle);

	ToggleInput(false);
}

void ATMGameMode::WinGame()
{
	EndGame();

	ToggleEndSessionUI(true);

	FTimerHandle DummyHandle;
	GetWorld()->GetTimerManager().SetTimer(DummyHandle, this, &ThisClass::StartGame, IntervalDuration, false);
}

void ATMGameMode::ToggleEndSessionUI(const bool bInValue) const
{
	if (TMPlayerController.IsValid())
	{
		if (auto* HUD = TMPlayerController->GetHUD<ATMHUD>())
		{
			if (bInValue)
			{
				HUD->ShowEndSessionWidget(TMGameState);
			}
			else
			{
				HUD->HideEndSessionWidget();
			}
		}
	}
}

void ATMGameMode::ToggleInput(const bool bInValue) const
{
	if (bInValue)
	{
		if (CharacterInstance.IsValid())
		{
			CharacterInstance->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}

		if (TMPlayerController.IsValid())
		{
			TMPlayerController->EnableInput(TMPlayerController.Get());
		}
	}
	else
	{
		if (TMPlayerController.IsValid())
		{
			TMPlayerController->DisableInput(TMPlayerController.Get());
		}

		if (CharacterInstance.IsValid())
		{
			CharacterInstance->GetCharacterMovement()->StopActiveMovement();
			CharacterInstance->GetCharacterMovement()->SetMovementMode(MOVE_None);
		}
	}
}

void ATMGameMode::UpdateGameStateDuration()
{
	TMGameState.Duration += 1;
}

void ATMGameMode::OnPerformAction()
{
	TMGameState.TotalActionsPerformed += 1;
}

void ATMGameMode::OnFindTreasure(const TMap<int32, TMap<ETMAction, float>>& QTable)
{
	float TotalReward = 0.f;
	int32 TotalCount = 0;

	for (const auto& SlotEntry : QTable)
	{
		const TMap<ETMAction, float>& Actions = SlotEntry.Value;
		
		for (const auto& ActionEntry : Actions)
		{
			TotalReward += ActionEntry.Value;
			TotalCount++;
		}
	}

	TMGameState.QTableAverage = TotalCount > 0 ? TotalReward / static_cast<float>(TotalCount) : 0.f;
}
