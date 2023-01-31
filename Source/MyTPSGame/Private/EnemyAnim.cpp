// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include "Enemy.h"

void UEnemyAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	owner = Cast<AEnemy>(TryGetPawnOwner());
}


//void UEnemyAnim::OnHitEvent()
//{
//	if (owner != nullptr)
//	{
//		// EnemyFSM�� OnHitEvent �Լ��� ȣ���ϰ� �ʹ�.
//		owner->EnemyFSM->OnHitEvent();
//	}
//}

void UEnemyAnim::AnimNotify_OnHit()
{
	if (owner == nullptr)
	{
		return;
	}
	// EnemyFSM�� OnHitEvent �Լ��� ȣ���ϰ� �ʹ�.
	owner->EnemyFSM->OnHitEvent();
}
