// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoint.h"

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
	// 적 팩토리로부터 랜덤한 위치에서 적 생성
	int32 rIndex = FMath::RandRange(0, SpawnPoints.Num()-1);

	// 만약 선택된 인덱스가 이전 인덱스와 같다면 값을 다시 정한다.
	/*while(true)
	{
		rIndex = FMath::RandRange(0, SpawnPoints.Num()-1);
		if(rIndex != PrevRandIndex)
		{
			break;
		}
	}*/
	if(rIndex == PrevRandIndex)
	{
		rIndex = (rIndex + 1) % SpawnPoints.Num();
	}
	PrevRandIndex = rIndex;

	FTransform transform = SpawnPoints[rIndex]->GetActorTransform();
	GetWorld()->SpawnActor<AEnemy>(EnemyFactory, transform);

	GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &ASpawnManager::SpawnEnemy, SpawnTime, false);
}