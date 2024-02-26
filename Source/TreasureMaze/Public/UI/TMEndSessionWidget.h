

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Framework/TMGameMode.h"
#include "TMEndSessionWidget.generated.h"

UCLASS()
class TREASUREMAZE_API UTMEndSessionWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
	FTMGameState GameState;
	
};
