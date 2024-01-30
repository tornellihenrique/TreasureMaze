// Fill out your copyright notice in the Description page of Project Settings.

#include "Utility/TMUtility.h"

#include "GameFramework/HUD.h"

bool UTMUtility::ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName)
{
	const auto* World{IsValid(Actor) ? Actor->GetWorld() : nullptr};
	const auto* PlayerController{IsValid(World) ? World->GetFirstPlayerController() : nullptr};
	auto* Hud{IsValid(PlayerController) ? PlayerController->GetHUD() : nullptr};

	return IsValid(Hud) && Hud->ShouldDisplayDebug(DisplayName) && Hud->GetCurrentDebugTargetActor() == Actor;
}
