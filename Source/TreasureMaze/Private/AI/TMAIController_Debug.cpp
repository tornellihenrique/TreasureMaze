// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/TMAIController.h"

#include "DisplayDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Engine/Canvas.h"
#include "Utility/TMUtility.h"
#include "World/TMMaze.h"

#define LOCTEXT_NAMESPACE "TMAIControllerDebug"

static const FName AIControllerDebugDisplayName{TEXTVIEW("TM.AIController")};

void ATMAIController::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation)
{
	/** Begin Initial Setup */
	const auto Scale{FMath::Min(Canvas->SizeX / (1280.0f * Canvas->GetDPIScale()), Canvas->SizeY / (720.0f * Canvas->GetDPIScale()))};

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{200.0f * Scale};

	auto MaxVerticalLocation{VerticalLocation};
	auto HorizontalLocation{5.0f * Scale};

	static const auto DebugModeHeaderText{LOCTEXT("DebugModeHeader", "Debug mode is enabled! Press (Shift + 0) to disable.")};

	DisplayDebugHeader(Canvas, DebugModeHeaderText, FLinearColor::Green, Scale, HorizontalLocation, VerticalLocation);

	VerticalLocation += RowOffset;
	MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation);

	// Check if any of the Displays are active
	if (!DisplayInfo.IsDisplayOn(AIControllerDebugDisplayName))
	{
		VerticalLocation = MaxVerticalLocation;

		Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
		return;
	}
	/** End Initial Setup */

	/** Begin First Line */
	const auto InitialVerticalLocation{VerticalLocation};

	static const auto ParkourVaultHeaderText{FText::AsCultureInvariant(FString{TEXTVIEW("TM.AIController (Shift + 1)")})};

	if (DisplayInfo.IsDisplayOn(AIControllerDebugDisplayName))
	{
		DisplayDebugHeader(Canvas, ParkourVaultHeaderText, FLinearColor::Green, Scale, HorizontalLocation, VerticalLocation);
		DisplayDebugValues(Canvas, Scale, HorizontalLocation, VerticalLocation);

		MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation + RowOffset);
		// VerticalLocation += InitialVerticalLocation;
		// HorizontalLocation += ColumnOffset;
	}
	else
	{
		DisplayDebugHeader(Canvas, ParkourVaultHeaderText, {0.0f, 0.333333f, 0.0f}, Scale, HorizontalLocation, VerticalLocation);

		VerticalLocation += RowOffset;
	}

	VerticalLocation += RowOffset;
	MaxVerticalLocation = FMath::Max(MaxVerticalLocation, VerticalLocation);
	/** End First Line */
	
	VerticalLocation = MaxVerticalLocation;

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}

void ATMAIController::DisplayDebugHeader(const UCanvas* Canvas, const FText& HeaderText, const FLinearColor& HeaderColor, const float Scale, const float HorizontalLocation, float& VerticalLocation)
{
	FCanvasTextItem Text{
		{HorizontalLocation, VerticalLocation},
		HeaderText,
		GEngine->GetMediumFont(),
		HeaderColor
	};

	Text.Scale = {Scale, Scale};
	Text.EnableShadow(FLinearColor::Black);

	Text.Draw(Canvas->Canvas);

	VerticalLocation += 15.0f * Scale;
}

void ATMAIController::DisplayDebugValues(const UCanvas* Canvas, const float Scale, const float HorizontalLocation, float& VerticalLocation) const
{
	VerticalLocation += 4.0f * Scale;

	FCanvasTextItem Text{
		FVector2D::ZeroVector,
		FText::GetEmpty(),
		GEngine->GetMediumFont(),
		FLinearColor::White
	};

	Text.Scale = {Scale * 0.75f, Scale * 0.75f};
	Text.EnableShadow(FLinearColor::Black);

	const auto RowOffset{12.0f * Scale};
	const auto ColumnOffset{145.0f * Scale};
	
	auto AddValue = [&](const FName& InName, const FString& InValue, const bool bIsBool = false)
	{
		Text.SetColor(FLinearColor::White);

		Text.Text = FText::AsCultureInvariant(FName::NameToDisplayString(InName.ToString(), bIsBool));
		Text.Draw(Canvas->Canvas, {HorizontalLocation, VerticalLocation});

		Text.Text = FText::AsCultureInvariant(InValue);
		Text.Draw(Canvas->Canvas, {HorizontalLocation + ColumnOffset, VerticalLocation});

		VerticalLocation += RowOffset;
	};

	AddValue(FName{TEXTVIEW("CurrentTile")}, FString::Printf(TEXT("%d"), CurrentTile));
	AddValue(FName{TEXTVIEW("TargetTile")}, FString::Printf(TEXT("%d"), TargetTile));
	AddValue(FName{TEXTVIEW("TreasureTile")}, FString::Printf(TEXT("%d"), TreasureTile));
	if (MazeInstance.IsValid())
	{
		const auto Coordinate = MazeInstance->GetTileWidthHeight(CurrentTile);
		AddValue(FName{TEXTVIEW("CurrentCoordinate")}, FString::Printf(TEXT("X = %d; Y = %d"), Coordinate.X, Coordinate.Y));
	}
	AddValue(FName{TEXTVIEW("CurrentAction")}, UTMUtility::GetNameStringByValue(CurrentAction.GetValue()));
}

#undef LOCTEXT_NAMESPACE
