// Fill out your copyright notice in the Description page of Project Settings.

#include "World/TMTreasure.h"

#include "Character/TMCharacter.h"
#include "Components/BoxComponent.h"
#include "Framework/TMGameMode.h"

ATMTreasure::ATMTreasure()
{
	Collision = CreateDefaultSubobject<UBoxComponent>(FName{TEXTVIEW("Collision")});
	if (IsValid(Collision))
	{
		Collision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
		RootComponent = Collision;
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName{TEXTVIEW("Mesh")});
	if (IsValid(Mesh))
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh->SetupAttachment(Collision);
	}
	
	PrimaryActorTick.bCanEverTick = true;
}

void ATMTreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATMTreasure::BeginPlay()
{
	Super::BeginPlay();
}

void ATMTreasure::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ATMCharacter* Character = Cast<ATMCharacter>(OtherActor))
	{
		if (ATMGameMode* GM = GetWorld()->GetAuthGameMode<ATMGameMode>())
		{
			GM->OnCollectTreasure();
		}
	}
}
