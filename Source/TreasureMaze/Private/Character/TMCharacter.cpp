// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/TMCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "World/TMMaze.h"

ATMCharacter::ATMCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATMCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshPerformingAction(DeltaTime);
}

bool ATMCharacter::PerformAction(const int32 TargetTile)
{
	if (!IsLocallyControlled()) return false;
	if (!MazeInstance.IsValid()) return false;

	if (bIsPerformingAction) return false;

	if (!MazeInstance->IsTileValid(TargetTile)) return false;

	LastTile = CurrentTile;
	CurrentTile = TargetTile;
	
	bIsPerformingAction = true;

	return true;
}

void ATMCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATMCharacter::RefreshPerformingAction(const float DeltaTime)
{
	if (!IsLocallyControlled()) return;
	if (!MazeInstance.IsValid()) return;
	
	if (!bIsPerformingAction) return;

	const FVector CurrentLocation = GetActorLocation();
	const FVector Tile = MazeInstance->GetTileLocationFromIndex(CurrentTile);

	if (FVector::Dist(CurrentLocation, Tile) < TileDistanceThreshold)
	{
		bIsPerformingAction = false;

		return;
	}
	
	const FVector TargetDirection = (Tile - CurrentLocation).GetSafeNormal2D();
	AddMovementInput(TargetDirection);
}
