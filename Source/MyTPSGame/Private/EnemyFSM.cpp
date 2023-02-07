// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "../MyTPSGame.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAnim.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "PathManager.h"

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
	moveSubState = EEnemyMoveSubState::PATROL;
	
	owner = Cast<AEnemy>(GetOwner());
	AIowner = Cast<AAIController>(owner->GetController());

	// 생성 시 현재 체력을 최대 체력으로 설정
	owner->currentHP = owner->maxHP;

	// 태어날 때 랜덤 목적지를 정함
	UpdateRandomLocation(RandomLocationRadius, RandomLocation);

	// 레벨에 존재하는 PathManager를 찾는다.
	PathManager = Cast<APathManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APathManager::StaticClass()));

	WayIndex = FMath::RandRange(0, PathManager->WayPoints.Num() - 1);
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
	switch (moveSubState)
	{
	case EEnemyMoveSubState::PATROL:
		TickPatrol();
		break;
	case EEnemyMoveSubState::CHASE:
		TickChase();
		break;
	case EEnemyMoveSubState::OLD_MOVE:
		TickMoveOldMove();
		break;
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
	// 만약 Die 애니메이션이 끝나지 않았다면
	if(!owner->EnemyAnim->bEnemyDieEnd)
	{
		return;
	}

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
	// 애니메이션 재생 시간 보장을 위한 currentTime 초기화
	currentTime = 0;
}

// 플레이어에게 맞았다.
void UEnemyFSM::OnDamageProcess(int DamageValue)
{
	AIowner->StopMovement();

	// 체력을 소모
	owner->currentHP -= DamageValue;
	// 체력이 0이 되면
	if (owner->currentHP <= 0)
	{
		// 에너미 사망
		SetState(EEnemyState::DIE);
		owner->EnemyAnim->bEnemyDieEnd = false;
		// 애님 몽타주의 Die 섹션을 플레이
		owner->OnMyDamage("Die");

		owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else // 그렇지 않으면
	{
		// Damage 한다
		SetState(EEnemyState::DAMAGE);

		int index = FMath::RandRange(0, 1);
		FString SectionName = FString::Printf(TEXT("Damage%d"), index);
		owner->OnMyDamage(FName(*SectionName));
		/*if (FMath::RandRange(0, 100) > 50)
		{
			enemy->OnMyDamage(TEXT("Damage0"));
		}
		else
		{
			enemy->OnMyDamage(TEXT("Damage1"));
		}*/
	}


}

bool UEnemyFSM::UpdateRandomLocation(float radius, FVector& OutLocation)
{
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	FNavLocation navLoc;
	bool result = ns->GetRandomReachablePointInRadius(owner->GetActorLocation(), radius, navLoc);
	if (result)
	{
		OutLocation = navLoc.Location;
	}

	return result;
}

void UEnemyFSM::TickMoveOldMove()
{
	// 내가 갈 수 있는 길 위에 타겟이 있는가?
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FPathFindingQuery query;
	FAIMoveRequest request;
	request.SetAcceptanceRadius(5.f); // 길을 만드는 범위가 아닌, 도착지에 도착했는지 여부를 확인할 때 도착지 크기에 여유를 주기 위해
	request.SetGoalLocation(player->GetActorLocation());
	AIowner->BuildPathfindingQuery(request, query);
	FPathFindingResult result = NavSystem->FindPathSync(query);

	// 갈 수 있다면 
	if (result.Result == ENavigationQueryResult::Success)
	{
		// 타겟쪽으로 이동
		AIowner->MoveToLocation(player->GetActorLocation());
	}
	// 갈 수 없다면 
	else
	{
		// 무작위로 위치를 하나 선정해서 그곳으로 이동
		// UpdateRandomLocation(RandomLocationRadius, RandomLocation);
		auto r = AIowner->MoveToLocation(RandomLocation);
		if (r == EPathFollowingRequestResult::AlreadyAtGoal || r == EPathFollowingRequestResult::Failed)
		{
			// 만약 그 무작위 위치에 도착했다면, 다시 무작위로 위치를 재선정
			UpdateRandomLocation(RandomLocationRadius, RandomLocation);

			// 이동
			// AIowner->MoveToLocation(player->GetActorLocation());
		}
	}


	// owner->AddMovementInput(dir.GetSafeNormal()); // 직선 이동만 가능한 이동
}

// 순찰할 위치를 순서대로 이동한다.
void UEnemyFSM::TickPatrol()
{
	FVector PatrolTarget = PathManager->WayPoints[WayIndex]->GetActorLocation();

	// 순방향 순찰
	/*int ArrayLength = PathManager->WayPoints.Num();
	WayIndex = (WayIndex + 1) % ArrayLength;*/

	// 역방향 순찰
	/*int ArrayLength = PathManager->WayPoints.Num();
	WayIndex = (WayIndex + ArrayLength - 1) % ArrayLength;*/

	// 순방향 순찰
	//auto result = AIowner->MoveToLocation(PatrolTarget);
	//// 만약 순찰 위치에 도착했다면
	//if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	//{
	//	// 순찰할 위치를 다음 위치로 갱신
	//	WayIndex++;
	//	// WayIndex의 값이 PathManager->WayPoints의 크기 이상이면
	//	if (WayIndex >= PathManager->WayPoints.Num())
	//	{
	//		// WayIndex의 값을 0으로 한다.
	//		WayIndex = 0;
	//	}
	//}

	//역방향 순찰
	//if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	//{
	//	// 순찰할 위치를 다음 위치로 갱신
	//	WayIndex--;
	//	// WayIndex의 값이 PathManager->WayPoints의 크기 이상이면
	//	if (WayIndex < 0)
	//	{
	//		// WayIndex의 값을 0으로 한다.
	//		WayIndex = PathManager->WayPoints.Num() - 1;
	//	}
	//}


	// 랜덤 순서 순찰
	auto result = AIowner->MoveToLocation(PatrolTarget);
	// 만약 순찰 위치에 도착했다면
	if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	{
		WayIndex = FMath::RandRange(0, PathManager->WayPoints.Num() - 1);
	}
	

	// 타겟과의 거리를 측정
	// float Distance = owner->GetActorLocation().Size() - player->GetActorLocation().Size();
	// UKismetSystemLibrary::SphereOverlapActors(GetWorld(), owner->...
	float Distance = owner->GetDistanceTo(player);

	// 만약 타겟이 감지거리 안에 있다면
	if(Distance <= DetectDistance)
	{
		// Chase 상태로 전이
		moveSubState = EEnemyMoveSubState::CHASE;
	}
	
}


// 공격 상태로 전이
void UEnemyFSM::TickChase()
{
	// 목적지를 향해 이동
	AIowner->MoveToLocation(player->GetActorLocation());


	// 목적지를 향하는 방향 생성
	FVector dir = player->GetActorLocation() - owner->GetActorLocation();

	// 목적지와의 거리를 재고
	// float dist = player->GetDistanceTo(owner);
	// float dist = dir.Size();
	float dist = FVector::Dist(player->GetActorLocation(), owner->GetActorLocation());

	// 목적지와의 거리가 공격 가능한 거리라면
	if (dist < AttackRange)
	{
		// 공격 상태로 전이
		SetState(EEnemyState::ATTACK);
	}
	else if(dist > AbandonDistance) // 그렇지 않고 포기 거리보다 멀어졌다면
	{
		moveSubState = EEnemyMoveSubState::PATROL;
	}
}