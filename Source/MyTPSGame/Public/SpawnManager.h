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
	// ������ �ð����� ������ ��ġ���� ���� ����
	// - ���� ��ġ���� ���
	UPROPERTY(EditAnywhere)
	TArray<AActor*> SpawnPoints;
	// - ���� �ð�
	UPROPERTY(EditAnywhere)
	float MinTime = 2.0f;
	UPROPERTY(EditAnywhere)
	float MaxTime = 5.0f;
	// - ���� �ð�
	float SpawnTime;
	// - Ÿ�̸��ڵ�
	FTimerHandle EnemySpawnTimerHandle;
	// - �� ���丮
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemy> EnemyFactory;
	// �� ���� ���
	void SpawnEnemy();
};
