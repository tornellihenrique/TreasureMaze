// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum
{
	CELL_PATH_N = 0x01,  // 1  - 00000001
	CELL_PATH_E = 0x02,  // 2  - 00000010
	CELL_PATH_S = 0x04,  // 4  - 00000100
	CELL_PATH_W = 0x08,  // 8  - 00001000
	CELL_VISITED = 0x10, // 16 - 00010000
};

UENUM(BlueprintType)
enum ETMAction : uint8
{
	None,
	Up,
	Down,
	Left,
	Right
};
