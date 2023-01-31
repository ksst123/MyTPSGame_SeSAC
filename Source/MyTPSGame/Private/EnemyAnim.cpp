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
//		// EnemyFSM의 OnHitEvent 함수를 호출하고 싶다.
//		owner->EnemyFSM->OnHitEvent();
//	}
//}

void UEnemyAnim::AnimNotify_OnHit()
{
	if (owner == nullptr)
	{
		return;
	}
	// EnemyFSM의 OnHitEvent 함수를 호출하고 싶다.
	owner->EnemyFSM->OnHitEvent();
}
