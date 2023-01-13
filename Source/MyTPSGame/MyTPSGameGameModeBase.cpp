// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyTPSGameGameModeBase.h"
#include "MyTPSGame.h"

AMyTPSGameGameModeBase::AMyTPSGameGameModeBase()
{
	// UE_LOG(LogTemp, Warning, TEXT("안녕"));
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *CALL_INFO, TEXT("asdkfvfsd"));
	// PRINT_CALL_INFO();

	PRINT_LOG(TEXT("%s %s %s"), TEXT("Hello"), TEXT("안녕"), TEXT("Hi"));
}
