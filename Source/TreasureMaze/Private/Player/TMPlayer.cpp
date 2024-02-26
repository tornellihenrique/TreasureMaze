// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/TMPlayer.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Character/TMCharacter.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/TMPlayerController.h"
#include "UI/TMHUD.h"

ATMPlayer::ATMPlayer()
{
	VisualRoot = CreateDefaultSubobject<USceneComponent>(FName{TEXTVIEW("VisualRoot")});
	RootComponent = VisualRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName{TEXTVIEW("SpringArm")});
	if (IsValid(SpringArm))
	{
		SpringArm->SetRelativeRotation({-30.f, 0.f, 0.f});
		SpringArm->TargetArmLength = 1000.f;
		SpringArm->SocketOffset = FVector(-300.f, 0.f, 80.f);
		SpringArm->bDoCollisionTest = false;
		SpringArm->bEnableCameraRotationLag = true;
		SpringArm->SetupAttachment(VisualRoot);
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(FName{TEXTVIEW("Camera")});
	if (IsValid(Camera))
	{
		Camera->FieldOfView = 20.f;
		Camera->SetupAttachment(SpringArm);
	}
	
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(FName{TEXTVIEW("FloatingPawnMovement")});
	if (IsValid(FloatingPawnMovement))
	{
		FloatingPawnMovement->MaxSpeed = 1500.f;
		FloatingPawnMovement->Acceleration = 8000.f;
		FloatingPawnMovement->Deceleration = 6500.f;
		FloatingPawnMovement->TurningBoost = 8.f;

		FloatingPawnMovement->SetPlaneConstraintNormal(FVector::UpVector);
		FloatingPawnMovement->bConstrainToPlane = true;
		FloatingPawnMovement->bSnapToPlaneAtStart = true;
	}
	
	PrimaryActorTick.bCanEverTick = true;
}

void ATMPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RefreshZoom(DeltaTime);

	RefreshTrackingMove(DeltaTime);
}

void ATMPlayer::Restart()
{
	Super::Restart();

	if (const auto* PC = GetController<ATMPlayerController>())
	{
		if (ATMHUD* HUD = PC->GetHUD<ATMHUD>())
		{
			HUD->CreateGameplayWidget();
		}
	}
}

void ATMPlayer::SetCharacter(ATMCharacter* InCharacter)
{
	ControlledCharacter = InCharacter;
	// TrackingCharacter = InCharacter;
}

void ATMPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetZoom(0.5f);
}

void ATMPlayer::NotifyControllerChanged()
{
	const auto* PreviousPlayer{ Cast<APlayerController>(PreviousController) };
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer()) };
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{ Cast<APlayerController>(GetController()) };
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{ ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer()) };
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	Super::NotifyControllerChanged();
}

void ATMPlayer::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{ Cast<UEnhancedInputComponent>(Input) };
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(DragMoveAction, ETriggerEvent::Started, this, &ThisClass::InputDragMoveStarted);
		EnhancedInput->BindAction(DragMoveAction, ETriggerEvent::Triggered, this, &ThisClass::InputDragMoveTriggered);
		EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ThisClass::InputZoom);
		EnhancedInput->BindAction(MoveCharacterAction, ETriggerEvent::Triggered, this, &ThisClass::InputMoveCharacter);
	}
}

bool ATMPlayer::ProjectMouseToPlane(FVector& OutIntersection) const
{
	const auto* PC = GetController<APlayerController>();

	if (!PC) return false;

	FVector2f MousePosition;
	if (!PC->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		int32 ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

		MousePosition = FVector2f(ViewportSizeX, ViewportSizeY) / 2.f;
	}

	const auto* PC1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC1) return false;

	FVector MouseWorldPosition;
	FVector MouseWorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(PC1, FVector2d{MousePosition}, MouseWorldPosition, MouseWorldDirection);

	const auto Plane = FPlane(FVector::ZeroVector, FVector::UpVector.GetSafeNormal());

	float T;
	UKismetMathLibrary::LinePlaneIntersection(MouseWorldPosition, MouseWorldPosition + MouseWorldDirection * 100000.f, Plane, T, OutIntersection);

	return true;
}

void ATMPlayer::InputDragMoveStarted()
{
	ProjectMouseToPlane(CachedDragMoveIntersection);
}

void ATMPlayer::InputDragMoveTriggered()
{
	FVector DragMoveIntersection;
	if (ProjectMouseToPlane(DragMoveIntersection))
	{
		CachedDragMove = CachedDragMoveIntersection - DragMoveIntersection;

		AddActorWorldOffset({CachedDragMove.X, CachedDragMove.Y, 0.f});
	}
}

void ATMPlayer::InputZoom(const FInputActionValue& ActionValue)
{
	ZoomDirection = ActionValue.Get<float>();

	ZoomTargetValue = FMath::Clamp(ZoomDirection * 5.f + ZoomTargetValue, 0.f, 100.f);
}

void ATMPlayer::InputMoveCharacter(const FInputActionValue& ActionValue)
{
	if (!IsValid(ControlledCharacter)) return;

	const auto Value{ ActionValue.Get<FVector2D>() };

	const auto ForwardDirection = FRotator(0.f, GetActorRotation().Yaw, 0.f).Vector();
	const auto RightDirection = FRotationMatrix(FRotator(0.f, GetActorRotation().Yaw, GetActorRotation().Roll)).GetScaledAxis(EAxis::Y);

	ControlledCharacter->AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void ATMPlayer::RefreshZoom(const float DeltaTime)
{
	if (!IsValid(ZoomCurve)) return;

	ZoomValue = UKismetMathLibrary::FInterpTo(ZoomValue, ZoomTargetValue, DeltaTime, 8.f);

	if (FMath::IsNearlyEqual(ZoomValue, ZoomTargetValue, 0.1f)) return;

	const float Value = ZoomCurve->GetFloatValue(ZoomValue / 100.f);

	SetZoom(Value);
}

void ATMPlayer::RefreshTrackingMove(const float DeltaTime)
{
	if (!IsValid(TrackingCharacter)) return;
	
	static constexpr float MovementSpeed{1000.f};
	static const FVector2D ScreenBoundsOffset{50.f, 50.f};

	// Get viewport size
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	// Project actor's location to screen space
	const auto ActorWorldPos = TrackingCharacter->GetActorLocation();
	FVector2D ActorScreenPos;
	if (UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), ActorWorldPos, ActorScreenPos))
	{
		// Check if actor is within screen bounds
		if (ActorScreenPos.X < ScreenBoundsOffset.X ||
			ActorScreenPos.Y < ScreenBoundsOffset.Y ||
			ActorScreenPos.X > ViewportSize.X - ScreenBoundsOffset.X ||
			ActorScreenPos.Y > ViewportSize.Y - ScreenBoundsOffset.Y)
		{
			// Move towards the new destination
			const FVector Direction = (ActorWorldPos - GetActorLocation()).GetSafeNormal();
			const FVector NewLocation = GetActorLocation() + Direction * MovementSpeed * DeltaTime;
			
			SetActorLocation(NewLocation);
		}
	}
}

void ATMPlayer::SetZoom(const float Value) const
{
	SpringArm->TargetArmLength = FMath::Lerp(800.f, 40000.f, Value);
	SpringArm->SetRelativeRotation({FMath::Lerp(-70.f, -90.f, Value), 0.f, 0.f});

	FloatingPawnMovement->MaxSpeed = FMath::Lerp(1000.f, 6000.f, Value);
	
	Camera->SetFieldOfView(FMath::Lerp(20.f, 15.f, Value));
	
	Camera->PostProcessSettings.DepthOfFieldFstop = 3.f;
	Camera->PostProcessSettings.DepthOfFieldSensorWidth = 150.f;
	Camera->PostProcessSettings.DepthOfFieldFocalDistance = SpringArm->TargetArmLength;
}
