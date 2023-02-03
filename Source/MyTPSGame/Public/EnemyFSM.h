// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h" // �ٸ� ������ϵ鿡 ���� ������ ��Ƴ���, �̸� �̿��ؼ� UClass �ν��Ͻ��� CDO �ν��Ͻ��� ����.

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

	// ���� ���� �Ÿ� = ���ʹ̰� �̵��ϴٰ� ���ߴ� ���ǰ�
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

	// PathManager�� �˰�
	UPROPERTY()
	class APathManager* PathManager;
	// PathManager�� WayPoints�� �̿��ؼ� ������ �������� ���Ѵ�.
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
