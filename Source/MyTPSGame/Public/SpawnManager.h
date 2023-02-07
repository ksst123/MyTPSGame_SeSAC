// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class MYTPSGAME_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 PrevRandIndex = -1;
	// 랜덤한 시간마다 랜덤한 위치에서 적을 생성
	// - 랜덤 위치들의 목록
	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPoints;
	// - 랜덤 시간
	UPROPERTY(EditAnywhere)
	float MinTime = 2.0f;
	UPROPERTY(EditAnywhere)
	float MaxTime = 5.0f;
	// - 생성 시간
	float SpawnTime;
	// - 타이머핸들
	FTimerHandle EnemySpawnTimerHandle;
	// - 적 팩토리
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> EnemyFactory;
	// 적 스폰 기능
	void SpawnEnemy();
};
