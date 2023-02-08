// Copyright Epic Games, Inc. All Rights Reserved.


#include "MyTPSGameGameModeBase.h"
#include "MyTPSGame.h"
#include "SpawnManager.h"
#include "Kismet/GameplayStatics.h"

AMyTPSGameGameModeBase::AMyTPSGameGameModeBase()
{
	// UE_LOG(LogTemp, Warning, TEXT("안녕"));
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *CALL_INFO, TEXT("asdkfvfsd"));
	// PRINT_CALL_INFO();

	PRINT_LOG(TEXT("%s %s %s"), TEXT("Hello"), TEXT("안녕"), TEXT("Hi"));
}

void AMyTPSGameGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
	
	LevelUp();
}

void AMyTPSGameGameModeBase::LevelUp()
{
	Level++;

	// 스폰매니저의 GoalSpawnCount를 현재 레벨에 맞게 설정
	SpawnManager->GoalSpawnCount = Level;

	SpawnManager->CurrentSpawnCount = 0;

	// needExp 설정
	if (Level < 20)
	{
		NeedExp = Level;
	}
	else if (Level < 40)
	{
		NeedExp = Level * 2;
	}
	else if (Level < 60)
	{
		NeedExp = Level * 3;
	}
	
}

void AMyTPSGameGameModeBase::AddExp(int32 amount)
{
	CurrentExp += amount;

	// 현재 경험치가 레벨업에 필요한 경험치에 도달했다면 레벨업
	if (CurrentExp >= NeedExp)
	{
		// 레벨업 시작
		LevelUpProcess();
	}

}

void AMyTPSGameGameModeBase::LevelUpProcess()
{
	// 레벨업 이펙트
	//

	CurrentExp -= NeedExp;
	LevelUp();

	if (CurrentExp >= NeedExp)
	{
		// 레벨업 시작
		FTimerHandle th;
		GetWorldTimerManager().SetTimer(th, this, &AMyTPSGameGameModeBase::LevelUpProcess, 0.2f, false);
	}
}