// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TMLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TMUtility.generated.h"

UCLASS()
class TREASUREMAZE_API UTMUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "TM",
	Meta = (DefaultToSelf = "Actor", AutoCreateRefTerm = "DisplayName", ReturnDisplayName = "Value"))
	static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);

	template <typename EnumType> requires std::is_enum_v<EnumType>
	static int32 GetIndexByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetIndexByValue(static_cast<int64>(Value));
	}

	template <typename EnumType> requires std::is_enum_v<EnumType>
	static FString GetNameStringByValue(const EnumType Value)
	{
		return StaticEnum<EnumType>()->GetNameStringByValue(static_cast<int64>(Value));
	}

	static FString IntToBinaryString(const int32 Value);

	static FString BoolToString(bool InBool);
	
};
