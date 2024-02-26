// Fill out your copyright notice in the Description page of Project Settings.

#include "Utility/TMUtility.h"

#include "GameFramework/HUD.h"

bool UTMUtility::ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName)
{
	const auto* World{IsValid(Actor) ? Actor->GetWorld() : nullptr};
	const auto* PlayerController{IsValid(World) ? World->GetFirstPlayerController() : nullptr};
	const auto* Hud{IsValid(PlayerController) ? PlayerController->GetHUD() : nullptr};

	return IsValid(Hud) && Hud->ShouldDisplayDebug(DisplayName);
}

FString UTMUtility::IntToBinaryString(int32 Value)
{
	FString BinaryString;
	do
	{
		// Append the least significant bit (0 or 1) of the value to the binary string
		BinaryString = FString::Printf(TEXT("%d%s"), Value & 1, *BinaryString);

		// Shift the value to the right by 1 bit to process the next bit
		Value >>= 1;
	} while (Value);
	
	return BinaryString;
}

FString UTMUtility::BoolToString(bool InBool)
{
	return InBool ? TEXT("true") : TEXT("false");
}
