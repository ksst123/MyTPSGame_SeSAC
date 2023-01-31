// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "../MyTPSGame.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnim.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	state = EEnemyState::IDLE;
	
	owner = Cast<AEnemy>(GetOwner());

	// 생성 시 현재 체력을 최대 체력으로 설정
	currentHP = maxHP;
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// owner의 EnemyAnim의 state에 내 state를 넣어주고 싶다.
	/*if (owner->EnemyAnim != nullptr)
	{
		owner->EnemyAnim->state = this->state;
	}*/
	

	switch (state)
	{
	case EEnemyState::IDLE:
		TickIdle();
		break;
	case EEnemyState::MOVE:
		TickMove();
		break;
	case EEnemyState::ATTACK:
		TickAttack();
		break;
	case EEnemyState::DAMAGE:
		TickDamage();
		break;
	case EEnemyState::DIE:
		TickDie();
		break;
	}
}

void UEnemyFSM::OnHitEvent()
{
	owner->EnemyAnim->bAttackPlay = false;

	// 공격 (조건: 공격거리 안에 있는가?)
	float dist = player->GetDistanceTo(owner);
	if (dist <= AttackRange)
	{
		PRINT_LOG(TEXT("Enemy is attacking!"));
	}
}

/// <summary>
/// 대기, 플레이어를 찾으면 이동으로 전이
/// </summary>
void UEnemyFSM::TickIdle()
{
	// 플레이어를 검색
	player = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// 만약 플레이어를 찾는다면
	if (player != nullptr)
	{
		// 이동으로 전이
		SetState(EEnemyState::MOVE);
	}
	
}

//void BeginOverlap(AActor* otherActor)
//{
//	// otherActor가 에너미 일 때와 플레이어 일 때 각각 다르게 처리하고 싶다
//	otherActor->GetClass();
//	if (otherActor->IsA(AEnemy::StaticClass()))
//	{
//
//	}
//	else if (otherActor->IsA(ATPSPlayer::StaticClass()))
//	{
//
//	}
//}

// 목적지를 향해 이동
// 목적지와의 거리가 공격 가능한 거리라면
// 공격 상태로 전이
void UEnemyFSM::TickMove()
{
	// 목적지를 향하는 방향 생성
	FVector dir = player->GetActorLocation() - owner->GetActorLocation();
	// 목적지로 이동
	owner->AddMovementInput(dir.GetSafeNormal());
	// 목적지와의 거리가 공격 가능한 거리라면
	// float dist = player->GetDistanceTo(owner);
	// float dist = dir.Size();
	float dist = FVector::Dist(player->GetActorLocation(), owner->GetActorLocation());
	if (dist < AttackRange)
	{
		// 공격 상태로 전이
		SetState(EEnemyState::ATTACK);
	}
	
	
}

// 공격 타이밍
void UEnemyFSM::TickAttack()
{
	// 시간이 흐르다가
	currentTime += GetWorld()->GetDeltaSeconds();

	// 현재 시간이 공격 시간을 초과하면
	/*if (!bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
	}*/
	
	// 공격 동작이 종료되면
	if (currentTime > AttackDelayTime)
	{
		// 계속 공격할 것인지 판단
		float dist = player->GetDistanceTo(owner);
		// 공격 거리보다 멀어졌다면(도망갔다면)
		if (dist > AttackRange)
		{
			// 이동 상태로 전이
			SetState(EEnemyState::MOVE);
		}
		else // 공격거리 안에 있으면 계속해서 공격
		{
			currentTime = 0.f;
			bAttackPlay = false;
			owner->EnemyAnim->bAttackPlay = true;
		}
	}
	
	
}

// player->enemy 공격
void UEnemyFSM::TickDamage()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	if (currentTime > 1.f)
	{
		SetState(EEnemyState::IDLE);
		currentTime = 0;
	}
}

void UEnemyFSM::TickDie()
{
	currentTime += GetWorld()->GetDeltaSeconds();

	FVector p0 = owner->GetActorLocation();
	FVector vt = FVector(0.f, 0.f, -1.f) * 200.f * GetWorld()->GetDeltaSeconds(); // owner->GetActorUpVector() * -1;
	owner->SetActorLocation(p0 + vt);

	if (currentTime > 1.f)
	{
		owner->Destroy();
	}
}

void UEnemyFSM::SetState(EEnemyState next)
{
	state = next;
	owner->EnemyAnim->state = next;
}

// 플레이어에게 맞았다.
void UEnemyFSM::OnDamageProcess(int DamageValue)
{
	// 체력을 소모
	currentHP -= DamageValue;
	// 체력이 0이 되면
	if (currentHP <= 0)
	{
		// 에너미 사망
		SetState(EEnemyState::DIE);
		owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else // 그렇지 않으면
	{
		// Damage 한다
		SetState(EEnemyState::DAMAGE);
	}


}