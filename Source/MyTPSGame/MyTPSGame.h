// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CALL_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

#define PRINT_CALL_INFO() UE_LOG(LogTemp, Warning, TEXT("%s"), *CALL_INFO) // ġȯ

#define PRINT_LOG(fmt, ...) UE_LOG(LogTemp, Warning, TEXT("%s %s"), *CALL_INFO, *FString::Printf(fmt, ##__VA_ARGS__))