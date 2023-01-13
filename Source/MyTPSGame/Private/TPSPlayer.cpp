// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletActor.h"


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
		GunMeshComponent->SetRelativeLocationAndRotation(FVector(0.f, 50.f, 130.f), FRotator(0.f));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// direction 방향으로 이동
	FTransform trans(GetControlRotation());
	FVector ResultDirection = trans.TransformVector(direction);
	// 현재 인풋 값은 카메라를 기준으로 잡기 때문에, 위나 아래를 본 상태로 전후 이동을 하면 작동되지 않는다. 이를 수정하기 위해서
	ResultDirection.Z = 0;
	ResultDirection.Normalize();
	AddMovementInput(ResultDirection);
	// Tick 보다 인풋 함수가 더 적게 호출될 경우 오차가 생길수도 있기 때문에 direction 값을 0으로 한다.
	direction = FVector::ZeroVector;

	// P = P0 + vt
	// SetActorLocation(GetActorLocation() + direction * WalkSpeed * DeltaTime);
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 상하좌우 이동 연결
	PlayerInputComponent->BindAxis(TEXT("MoveHorizontal"), this, &ATPSPlayer::OnAxisHorizontal);
	PlayerInputComponent->BindAxis(TEXT("MoveVertical"), this, &ATPSPlayer::OnAxisVertical);

	// 플레이어 시점 변경 연결
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATPSPlayer::OnAxisLookUp);
	PlayerInputComponent->BindAxis(TEXT("TurnRight"), this, &ATPSPlayer::OnAxisTurnRight);

	// 점프 액션 연결
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ATPSPlayer::OnActionJump);

	// 발사 액션 연결
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::OnActionFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::OnActionFireReleased);
	
	
}

void ATPSPlayer::OnAxisHorizontal(float value)
{
	direction.Y = value;
}

void ATPSPlayer::OnAxisVertical(float value)
{
	direction.X = value;
}

void ATPSPlayer::OnAxisLookUp(float value)
{
	// Y축(Pitch) 회전
	AddControllerPitchInput(value);
}

void ATPSPlayer::OnAxisTurnRight(float value)
{
	// Z축(Yaw) 회전
	AddControllerYawInput(value);
}

void ATPSPlayer::OnActionJump()
{
	Jump();
}

void ATPSPlayer::OnActionFirePressed()
{
	DoFire();
}

void ATPSPlayer::OnActionFireReleased()
{

}

void ATPSPlayer::DoFire()
{
	// 총알 생성 위치
	FVector SpawnDirection;
	// 총알 생성 위치는 플레이어 1미터 앞
	SpawnDirection = GetActorLocation() + GetActorForwardVector() * 1.f;

	// 총 스켈레탈에서 생성한 총알 발사 위치 소켓의 Transform 데이터를 받아와서 저장
	FTransform FirePosition = GunMeshComponent->GetSocketTransform(TEXT("FirePosition"));

	// 총알 액터 생성
	GetWorld()->SpawnActor<ABulletActor>(BulletFactory, FirePosition);
	// UGameplayStatics::, GetWorld()->, ACharacter::, UKismetLibrary:: ... 
}

