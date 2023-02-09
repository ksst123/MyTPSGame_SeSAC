// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletActor.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "TPSPlayerAnim.h"
#include "TPSPlayerMoveComponent.h"
#include "TPSPlayerFireComponent.h"


// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 외형에 해당하는 애셋 읽기 
	// Game = Content 폴더
	// ConstructorHelpers = 생성자 도우미, 생성자에서만 사용
	// FObjectFinder = 애셋, 이미지 등등, FClassFinder = 블루프린트
	ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterSkeletal(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn.SKM_Quinn'"));

	// 외형 애셋 읽기를 성공했다면
	if (CharacterSkeletal.Succeeded()) // == if(CharacterSkeletal.Object~~
	{
		// 캐릭터 외형을 수동으로 설정
		GetMesh()->SetSkeletalMesh(CharacterSkeletal.Object);
		// 캐릭터 Transform 수정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -90.f), FRotator(0.f, -90.f, 0.f));
		// GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		// GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	}


	// SpringArm Component, Camera Component 생성
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));

	// SpringArm을 RootComponent에 Attach
	SpringArmComponent->SetupAttachment(RootComponent);
	// SpringArm Location 설정
	SpringArmComponent->SetRelativeLocation(FVector(0.f));
	SpringArmComponent->TargetArmLength = 300.f;


	// Camera는 SpringArm에 Attach
	CameraComponent->SetupAttachment(SpringArmComponent);
	// CameraComponent 로테이션

	// 입력값을 회전에 반영
	bUseControllerRotationYaw = true;
	SpringArmComponent->bUsePawnControlRotation = true;
	CameraComponent->bUsePawnControlRotation = true;

	// 카메라 움직임 살짝 느리게
	GetCharacterMovement()->bOrientRotationToMovement = true;


	// Gun Mesh Component 생성
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh"));
	// Gun Mesh Component를 캐릭터의 기본 Skeletal Mesh Component에 부착
	GunMeshComponent->SetupAttachment(GetMesh());
	// 총 스켈레탈 메시 애셋 읽기
	ConstructorHelpers::FObjectFinder<USkeletalMesh> GunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	// 메시 애셋 넣는 것을 성공했다면
	if (GunMesh.Succeeded())
	{
		// 읽은 총 메시 애셋을 Gun Mesh Component에 넣기
		GunMeshComponent->SetSkeletalMesh(GunMesh.Object);
		// Gun Mesh Component 위치 설정
		GunMeshComponent->SetRelativeLocationAndRotation(FVector(-16.156039, 42.550698, 128.479177), FRotator(-0.470297, 5.066306, 1.584315));
		GunMeshComponent->SetupAttachment(this->GetMesh(), TEXT("GunSocket"));
	}

	// Sniper Mesh Component 생성
	SniperMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper Mesh"));
	// Sniper Mesh Component를 캐릭터의 기본 Skeletal Mesh Component에 부착
	SniperMeshComponent->SetupAttachment(GetMesh());
	// 저격총 스태틱 메시 애셋 읽기
	ConstructorHelpers::FObjectFinder<UStaticMesh> SniperMesh(TEXT("/Script/Engine.StaticMesh'/Game/SniperGun/sniper1.sniper1'"));
	// 메시 애셋 넣는 것을 성공했다면
	if (SniperMesh.Succeeded())
	{
		// 읽은 저격 총 메시 애셋을 Sniper Mesh Component에 넣기
		SniperMeshComponent->SetStaticMesh(SniperMesh.Object);
		SniperMeshComponent->SetRelativeLocationAndRotation(FVector(-18.080458, 72.236662, 132.622571), FRotator(1.341071, 0.420521, -0.576818));
		SniperMeshComponent->SetRelativeScale3D(FVector(0.15f));
		SniperMeshComponent->SetupAttachment(this->GetMesh(), TEXT("GunSocket"));
	}

	ConstructorHelpers::FObjectFinder<USoundWave> TempFireSound(TEXT("/Script/Engine.SoundWave'/Game/SniperGun/Rifle.Rifle'"));
	if (TempFireSound.Succeeded())
	{
		FireSound = TempFireSound.Object;
	}

	// GetCharacterMovement()->Crouch(true);

	// 이동 컴포넌트를 생성
	MoveComponent = CreateDefaultSubobject<UTPSPlayerMoveComponent>(TEXT("Player Move Component"));

	// 발사 컴포넌트를 생성
	FireComponent = CreateDefaultSubobject<UTPSPlayerFireComponent>(TEXT("Player Fire Component"));
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupInputDelegate.Broadcast(PlayerInputComponent);
	//MoveComponent->SetupPlayerInput(PlayerInputComponent);
	//FireComponent->SetupPlayerInput(PlayerInputComponent);
}

void ATPSPlayer::OnMyHit_Implementation(int damage)
{
	CurrentHP -= damage;
}