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

	// ������ �ð� �Ŀ� �� ���� �Լ��� ȣ��
	SpawnTime = FMath::RandRange(MinTime, MaxTime);
	// GetWorld()->GetTimerManager(), GetWorldTimerManager() : �� �� ����
	GetWorldTimerManager().SetTimer(EnemySpawnTimerHandle, this, &ASpawnManager::SpawnEnemy, SpawnTime, false);
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnManager::SpawnEnemy()
{
	// �� ���丮�κ��� ������ ��ġ���� �� ����
	int32 rIndex = FMath::RandRange(0, SpawnPoints.Num()-1);

	// ���� ���õ� �ε����� ���� �ε����� ���ٸ� ���� �ٽ� ���Ѵ�.
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