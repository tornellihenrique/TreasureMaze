// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TMTreasure.generated.h"

class UBoxComponent;

UCLASS(Abstract)
class TREASUREMAZE_API ATMTreasure : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TM Treasure")
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "TM Treasure")
	TObjectPtr<UStaticMeshComponent> Mesh;

public:

	ATMTreasure();
	
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
