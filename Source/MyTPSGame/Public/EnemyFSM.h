// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h" // 다른 헤더파일들에 대한 정보를 모아놓고, 이를 이용해서 UClass 인스턴스와 CDO 인스턴스를 만듦.

UENUM(BlueprintType)
enum class EEnemyState : uint8 // 0~255
{
	IDLE,
	MOVE,
	ATTACK,
	DAMAGE,
	DIE,
};

UENUM(BlueprintType)
enum class EEnemyMoveSubState : uint8
{
	PATROL,
	CHASE,
	OLD_MOVE,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EEnemyState state;

	EEnemyMoveSubState	moveSubState;

	// 공격 가능 거리 = 에너미가 이동하다가 멈추는 조건값
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDelayTime = 3.f;

	UPROPERTY(EditAnywhere)
	float RandomLocationRadius = 500.f;

	UPROPERTY()
	class AAIController* AIowner;

	UPROPERTY()
	class ATPSPlayer* player; // caching

	UPROPERTY()
	class AEnemy* owner;

	FVector RandomLocation;

	bool UpdateRandomLocation(float radius, FVector& OutLocation);

	void OnDamageProcess(int DamageValue);


	void OnHitEvent();

	// PathManager를 알고
	UPROPERTY()
	class APathManager* PathManager;
	// PathManager의 WayPoints를 이용해서 순찰할 목적지를 정한다.
	int32 WayIndex;

private:
	void TickIdle();
	void TickMove();
	void TickAttack();
	void TickDamage();
	void TickDie();
	void TickMoveOldMove();
	void TickPatrol();
	void TickChase();

	void SetState(EEnemyState next);



	float currentTime = 0.f;

	bool bAttackPlay;

};
