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

	// ���� �� ���� ü���� �ִ� ü������ ����
	owner->currentHP = owner->maxHP;

	// �¾ �� ���� �������� ����
	UpdateRandomLocation(RandomLocationRadius, RandomLocation);

	// ������ �����ϴ� PathManager�� ã�´�.
	PathManager = Cast<APathManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APathManager::StaticClass()));

	WayIndex = FMath::RandRange(0, PathManager->WayPoints.Num() - 1);
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
	// ���� Die �ִϸ��̼��� ������ �ʾҴٸ�
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
	// �ִϸ��̼� ��� �ð� ������ ���� currentTime �ʱ�ȭ
	currentTime = 0;
}

// �÷��̾�� �¾Ҵ�.
void UEnemyFSM::OnDamageProcess(int DamageValue)
{
	AIowner->StopMovement();

	// ü���� �Ҹ�
	owner->currentHP -= DamageValue;
	// ü���� 0�� �Ǹ�
	if (owner->currentHP <= 0)
	{
		// ���ʹ� ���
		SetState(EEnemyState::DIE);
		owner->EnemyAnim->bEnemyDieEnd = false;
		// �ִ� ��Ÿ���� Die ������ �÷���
		owner->OnMyDamage("Die");

		owner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else // �׷��� ������
	{
		// Damage �Ѵ�
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
	// ���� �� �� �ִ� �� ���� Ÿ���� �ִ°�?
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FPathFindingQuery query;
	FAIMoveRequest request;
	request.SetAcceptanceRadius(5.f); // ���� ����� ������ �ƴ�, �������� �����ߴ��� ���θ� Ȯ���� �� ������ ũ�⿡ ������ �ֱ� ����
	request.SetGoalLocation(player->GetActorLocation());
	AIowner->BuildPathfindingQuery(request, query);
	FPathFindingResult result = NavSystem->FindPathSync(query);

	// �� �� �ִٸ� 
	if (result.Result == ENavigationQueryResult::Success)
	{
		// Ÿ�������� �̵�
		AIowner->MoveToLocation(player->GetActorLocation());
	}
	// �� �� ���ٸ� 
	else
	{
		// �������� ��ġ�� �ϳ� �����ؼ� �װ����� �̵�
		// UpdateRandomLocation(RandomLocationRadius, RandomLocation);
		auto r = AIowner->MoveToLocation(RandomLocation);
		if (r == EPathFollowingRequestResult::AlreadyAtGoal || r == EPathFollowingRequestResult::Failed)
		{
			// ���� �� ������ ��ġ�� �����ߴٸ�, �ٽ� �������� ��ġ�� �缱��
			UpdateRandomLocation(RandomLocationRadius, RandomLocation);

			// �̵�
			// AIowner->MoveToLocation(player->GetActorLocation());
		}
	}


	// owner->AddMovementInput(dir.GetSafeNormal()); // ���� �̵��� ������ �̵�
}

// ������ ��ġ�� ������� �̵��Ѵ�.
void UEnemyFSM::TickPatrol()
{
	FVector PatrolTarget = PathManager->WayPoints[WayIndex]->GetActorLocation();

	// ������ ����
	/*int ArrayLength = PathManager->WayPoints.Num();
	WayIndex = (WayIndex + 1) % ArrayLength;*/

	// ������ ����
	/*int ArrayLength = PathManager->WayPoints.Num();
	WayIndex = (WayIndex + ArrayLength - 1) % ArrayLength;*/

	// ������ ����
	//auto result = AIowner->MoveToLocation(PatrolTarget);
	//// ���� ���� ��ġ�� �����ߴٸ�
	//if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	//{
	//	// ������ ��ġ�� ���� ��ġ�� ����
	//	WayIndex++;
	//	// WayIndex�� ���� PathManager->WayPoints�� ũ�� �̻��̸�
	//	if (WayIndex >= PathManager->WayPoints.Num())
	//	{
	//		// WayIndex�� ���� 0���� �Ѵ�.
	//		WayIndex = 0;
	//	}
	//}

	//������ ����
	//if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	//{
	//	// ������ ��ġ�� ���� ��ġ�� ����
	//	WayIndex--;
	//	// WayIndex�� ���� PathManager->WayPoints�� ũ�� �̻��̸�
	//	if (WayIndex < 0)
	//	{
	//		// WayIndex�� ���� 0���� �Ѵ�.
	//		WayIndex = PathManager->WayPoints.Num() - 1;
	//	}
	//}


	// ���� ���� ����
	auto result = AIowner->MoveToLocation(PatrolTarget);
	// ���� ���� ��ġ�� �����ߴٸ�
	if (result == EPathFollowingRequestResult::AlreadyAtGoal || result == EPathFollowingRequestResult::Failed)
	{
		WayIndex = FMath::RandRange(0, PathManager->WayPoints.Num() - 1);
	}
	

	// Ÿ�ٰ��� �Ÿ��� ����
	// float Distance = owner->GetActorLocation().Size() - player->GetActorLocation().Size();
	// UKismetSystemLibrary::SphereOverlapActors(GetWorld(), owner->...
	float Distance = owner->GetDistanceTo(player);

	// ���� Ÿ���� �����Ÿ� �ȿ� �ִٸ�
	if(Distance <= DetectDistance)
	{
		// Chase ���·� ����
		moveSubState = EEnemyMoveSubState::CHASE;
	}
	
}


// ���� ���·� ����
void UEnemyFSM::TickChase()
{
	// �������� ���� �̵�
	AIowner->MoveToLocation(player->GetActorLocation());


	// �������� ���ϴ� ���� ����
	FVector dir = player->GetActorLocation() - owner->GetActorLocation();

	// ���������� �Ÿ��� ���
	// float dist = player->GetDistanceTo(owner);
	// float dist = dir.Size();
	float dist = FVector::Dist(player->GetActorLocation(), owner->GetActorLocation());

	// ���������� �Ÿ��� ���� ������ �Ÿ����
	if (dist < AttackRange)
	{
		// ���� ���·� ����
		SetState(EEnemyState::ATTACK);
	}
	else if(dist > AbandonDistance) // �׷��� �ʰ� ���� �Ÿ����� �־����ٸ�
	{
		moveSubState = EEnemyMoveSubState::PATROL;
	}
}