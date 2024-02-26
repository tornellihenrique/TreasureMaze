#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TMHUD.generated.h"

struct FTMGameState;
class UTMEndSessionWidget;
class UTMGameplayWidget;

UCLASS(Abstract)
class TREASUREMAZE_API ATMHUD : public AHUD
{
	GENERATED_BODY()

public:

	ATMHUD();

	virtual void BeginPlay() override;

	void CreateGameplayWidget();

	void ShowEndSessionWidget(const FTMGameState InGameState);
	void HideEndSessionWidget();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UTMGameplayWidget> GameplayWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UTMEndSessionWidget> EndSessionWidgetClass;

public:

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UTMGameplayWidget* GameplayWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UTMEndSessionWidget* EndSessionWidget;
	
};
