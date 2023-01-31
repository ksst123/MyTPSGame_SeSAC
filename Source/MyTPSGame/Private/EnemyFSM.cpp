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

	// ���� �� ���� ü���� �ִ� ü������ ����
	currentHP = maxHP;
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// owner�� EnemyAnim�� state�� �� state�� �־��ְ� �ʹ�.
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

	// ���� (����: ���ݰŸ� �ȿ� �ִ°�?)
	float dist = player->GetDistanceTo(owner);
	if (dist <= AttackRange)
	{
		PRINT_LOG(TEXT("Enemy is attacking!"));
	}
}

/// <summary>
/// ���, �÷��̾ ã���� �̵����� ����
/// </summary>
void UEnemyFSM::TickIdle()
{
	// �÷��̾ �˻�
	player = Cast<ATPSPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	// ���� �÷��̾ ã�´ٸ�
	if (player != nullptr)
	{
		// �̵����� ����
		SetState(EEnemyState::MOVE);
	}
	
}

//void BeginOverlap(AActor* otherActor)
//{
//	// otherActor�� ���ʹ� �� ���� �÷��̾� �� �� ���� �ٸ��� ó���ϰ� �ʹ�
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

// �������� ���� �̵�
// ���������� �Ÿ��� ���� ������ �Ÿ����
// ���� ���·� ����
void UEnemyFSM::TickMove()
{
	// �������� ���ϴ� ���� ����
	FVector dir = player->GetActorLocation() - owner->GetActorLocation();
	// �������� �̵�
	owner->AddMovementInput(dir.GetSafeNormal());
	// ���������� �Ÿ��� ���� ������ �Ÿ����
	// float dist = player->GetDistanceTo(owner);
	// float dist = dir.Size();
	float dist = FVector::Dist(player->GetActorLocation(), owner->GetActorLocation());
	if (dist < AttackRange)
	{
		// ���� ���·� ����
		SetState(EEnemyState::ATTACK);
	}
	
	
}

// ���� Ÿ�̹�
void UEnemyFSM::TickAttack()
{
	// �ð��� �帣�ٰ�
	currentTime += GetWorld()->GetDeltaSeconds();

	// ���� �ð��� ���� �ð��� �ʰ��ϸ�
	/*if (!bAttackPlay && currentTime > 0.1f)
	{
		bAttackPlay = true;
	}*/
	
	// ���� ������ ����Ǹ�
	if (currentTime > AttackDelayTime)
	{
		// ��� ������ ������ �Ǵ�
		float dist = player->GetDistanceTo(owner);
		// ���� �Ÿ����� �־����ٸ�(�������ٸ�)
		if (dist > AttackRange)
		{
			// �̵� ���·� ����
			SetState(EEnemyState::MOVE);
		}
		else // ���ݰŸ� �ȿ� ������ ����ؼ� ����
		{
			currentTime = 0.f;
			bAttackPlay = false;
			owner->EnemyAnim->bAttackPlay = true;
		}
	}
	
	
}

// player->enemy ����
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

// �÷��̾�� �¾Ҵ�.
void UEnemyFSM::OnDamageProcess(int DamageValue)
{
	// ü���� �Ҹ�
	currentHP -= DamageValue;
	// ü���� 0�� �Ǹ�
	if (currentHP <= 0)
	{
		// ���ʹ� ���
		SetState(EEnemyState::DIE);
		owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else // �׷��� ������
	{
		// Damage �Ѵ�
		SetState(EEnemyState::DAMAGE);
	}


}