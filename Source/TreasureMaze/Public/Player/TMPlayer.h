﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TMPlayer.generated.h"

class ATMCharacter;
struct FInputActionValue;

class UFloatingPawnMovement;
class UInputAction;
class UInputMappingContext;

class UCameraComponent;
class USpringArmComponent;

UCLASS(AutoExpandCategories = ("Settings|TM Player", "State|TM Player"))
class TREASUREMAZE_API ATMPlayer : public APawn
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "TM Player")
	TObjectPtr<UFloatingPawnMovement> FloatingPawnMovement;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TM Player")
	TObjectPtr<USceneComponent> VisualRoot;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TM Player")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TM Player")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Player", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Player", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> DragMoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Player", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Player", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveCharacterAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings|TM Player")
	TObjectPtr<UCurveFloat> ZoomCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Player", Transient)
	TObjectPtr<ATMCharacter> ControlledCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State|TM Player", Transient)
	TObjectPtr<ATMCharacter> TrackingCharacter;

public:
	
	ATMPlayer();
	
	virtual void Tick(float DeltaTime) override;

	virtual void Restart() override;

	void SetCharacter(ATMCharacter* InCharacter);
	
protected:
	
	virtual void BeginPlay() override;

	// Input

public:

	virtual void NotifyControllerChanged() override;

protected:
	
	virtual void SetupPlayerInputComponent(class UInputComponent* Input) override;

	bool ProjectMouseToPlane(FVector& OutIntersection) const;

private:

	FVector CachedDragMoveIntersection{ForceInit};
	FVector CachedDragMove{ForceInit};

	float ZoomDirection{0.f};
	float ZoomTargetValue{50.f};
	float ZoomValue{50.f};
	
	void InputDragMoveStarted();
	void InputDragMoveTriggered();
	void InputZoom(const FInputActionValue& ActionValue);
	void InputMoveCharacter(const FInputActionValue& ActionValue);

	void RefreshZoom(float DeltaTime);

	void RefreshTrackingMove(float DeltaTime);

	void SetZoom(float Value) const;
	
};
