// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"
#include "../MyTPSGameGameModeBase.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), SpawnPoints);

	// 랜덤한 시간 후에 적 생성 함수를 호출
	SpawnTime = FMath::RandRange(MinTime, MaxTime);
	// GetWorld()->GetTimerManager(), GetWorldTimerManager() : 둘 다 같음
	GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &ASpawnManager::SpawnEnemy, SpawnTime, false);
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnManager::SpawnEnemy()
{
	// 만약 현재 스폰 개수가 목표 스폰 개수보다 작다면 스폰
	if (CurrentSpawnCount < GoalSpawnCount)
	{

		// 적 팩토리로부터 랜덤한 위치에서 적 생성
		int32 rIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

		// 만약 선택된 인덱스가 이전 인덱스와 같다면 값을 다시 정한다.
		/*while(true)
		{
			rIndex = FMath::RandRange(0, SpawnPoints.Num()-1);
			if(rIndex != PrevRandIndex)
			{
				break;
			}
		}*/
		if (rIndex == PrevRandIndex)
		{
			rIndex = (rIndex + 1) % SpawnPoints.Num();
		}
		PrevRandIndex = rIndex;

		FTransform transform = SpawnPoints[rIndex]->GetActorTransform();

		// 에너미 종류를 랜덤하게 스폰
		// int32 EnemyIndex = FMath::RandRange(0, 1);

		// 만약 50% 이하면 EnemyFactory[0], 그렇지 않으면 ENemyFactory[1]을 스폰
		int32 SpawnRate = FMath::RandRange(0, 99);
		int32 LevelSpawnRate = 50;
		
		int32 level = Cast<AMyTPSGameGameModeBase>(GetWorld()->GetAuthGameMode())->Level;
		if (level < 3)
		{
			LevelSpawnRate = 50;
		}
		else
		{
			// FMath::Max(a, b) : 둘 중 최대값을 반환
			LevelSpawnRate = FMath::Max(20, 50 - level);

			// LevelSpawnRate = -1;
		}
		

		if (SpawnRate < LevelSpawnRate)
		{
			GetWorld()->SpawnActor<AEnemy>(EnemyFactory[0], transform);
		}
		else
		{
			GetWorld()->SpawnActor<AEnemy>(EnemyFactory[1], transform);
		}

		CurrentSpawnCount++;
	}

	SpawnTime = FMath::RandRange(MinTime, MaxTime);
	GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &ASpawnManager::SpawnEnemy, SpawnTime, false);
}