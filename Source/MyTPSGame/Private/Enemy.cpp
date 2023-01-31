// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyFSM.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyAnim.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("Enemy FSM"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
		GetMesh()->SetRelativeScale3D(FVector(0.85f));
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;

	// FClassFinder 에서는 경로 마지막에 _C를 붙여줘야 한다
	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnimInst(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	if (TempAnimInst.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnimInst.Class);
	}
}


// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

