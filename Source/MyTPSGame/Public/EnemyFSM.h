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

	// ���� ���� �Ÿ� = ���ʹ̰� �̵��ϴٰ� ���ߴ� ���ǰ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackDelayTime = 3.f;

	class AAIController* AIowner;

	void OnDamageProcess(int DamageValue);

	// ���ʹ� ü��
	int currentHP;
	int maxHP = 2;

	void OnHitEvent();


private:
	void TickIdle();
	void TickMove();
	void TickAttack();
	void TickDamage();
	void TickDie();

	void SetState(EEnemyState next);

	class ATPSPlayer* player; // caching

	class AEnemy* owner;

	float currentTime = 0.f;

	bool bAttackPlay;

};
