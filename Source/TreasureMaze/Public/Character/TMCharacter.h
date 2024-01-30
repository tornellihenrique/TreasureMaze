// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TMCharacter.generated.h"

UCLASS()
class TREASUREMAZE_API ATMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	ATMCharacter();

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;
	
};
