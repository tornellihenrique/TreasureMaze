
#include "UI/TMHUD.h"

#include "UI/TMEndSessionWidget.h"
#include "UI/TMGameplayWidget.h"
#include "Widgets/SWeakWidget.h"

ATMHUD::ATMHUD()
{
}

void ATMHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ATMHUD::CreateGameplayWidget()
{
	if (!GameplayWidget && GameplayWidgetClass && PlayerOwner)
	{
		GameplayWidget = CreateWidget<UTMGameplayWidget>(PlayerOwner, GameplayWidgetClass);
	}

	if (GameplayWidget && !GameplayWidget->IsInViewport())
	{
		GameplayWidget->AddToViewport();
	}
}

void ATMHUD::ShowEndSessionWidget(const FTMGameState InGameState)
{
	if (!EndSessionWidget && EndSessionWidgetClass && PlayerOwner)
	{
		EndSessionWidget = CreateWidget<UTMEndSessionWidget>(PlayerOwner, EndSessionWidgetClass);
	}

	if (EndSessionWidget && !EndSessionWidget->IsInViewport())
	{
		EndSessionWidget->GameState = InGameState;
		EndSessionWidget->AddToViewport(2);
	}
}

void ATMHUD::HideEndSessionWidget()
{
	if (EndSessionWidget && EndSessionWidget->IsInViewport())
	{
		EndSessionWidget->RemoveFromParent();
	}
}
