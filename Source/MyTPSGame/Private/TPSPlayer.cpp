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

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;


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
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// UI 생성
	CrosshairUI = CreateWidget(GetWorld(), CrosshairFactory);
	SniperUI = CreateWidget(GetWorld(), SniperFactory);
	// CrosshairUI를 시작시 기본으로 화면에 출력
	CrosshairUI->AddToViewport();
	// Grenade Gun을 시작시 기본 총으로 설정
	ChooseGun(GRENADE_GUN);

	// Sniper Gun일 때 ZoomIn을 하면 CrosshairUI X, SniperUI O
	// Sniper Gun일 때 ZoomOut을 하면 CrosshairUI O, SniperUI X
	// 기본총(Grenade)를 선택하면 CrosshairUI O, SniperUI X

	GunAmmo = MaxGunAmmo;
	SniperAmmo = MaxSniperAmmo;
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
	
	// 총교체 액션 연결
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &ATPSPlayer::OnActionGrenade);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &ATPSPlayer::OnActionSniper);

	// 줌인/줌아웃 액션 연결
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &ATPSPlayer::OnActionZoomIn);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &ATPSPlayer::OnActionZoomOut);

	// 달리기 액션 연결
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::OnActionRunPressed);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::OnActionRunReleased);

	// 살금살금 걷기 액션 연결
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &ATPSPlayer::OnActionCrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &ATPSPlayer::OnActionCrouchPressed);

	// 재장전
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ATPSPlayer::OnActionReload);

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

void ATPSPlayer::OnActionRunPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void ATPSPlayer::OnActionRunReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATPSPlayer::OnActionCrouchPressed()
{
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
}

void ATPSPlayer::OnActionCrouchReleased()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void ATPSPlayer::OnActionJump()
{
	Jump();
}

void ATPSPlayer::OnActionFirePressed()
{
	// 총알 남아있는지 검증
	if (bChooseGrenadeGun)
	{
		// 총알이 남아있다면 1발 차감하고
		if (GunAmmo > 0)
		{
			GunAmmo--;
		}
		// 그렇지 않으면 총을 쏘지 못한다.
		else
		{ 
			return;
		}
	}
	else
	{
		// 총알이 남아있다면 1발 차감하고
		if (SniperAmmo > 0)
		{ 
			SniperAmmo--; 
		}
		// 그렇지 않으면 총을 쏘지 못한다.
		else
		{ 
			return;
		}
	}

	// 카메라 진동
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	// 카메라 진동
	/*auto controller = Cast<APlayerController>(GetController());
	controller->PlayerCameraManager;*/

	CameraManager->StartCameraShake(CameraShakeFactory);
	// 만약 카메라가 이미 진동 중이면(=만약 canShakeInstance가 nullptr이 아니고, 진동중이라면)
	if (canShakeInstance != nullptr && !(canShakeInstance->IsFinished()))
	{
		// 취소하고
		CameraManager->StopCameraShake(canShakeInstance);
	}
	// 카메라 진동
	canShakeInstance = CameraManager->StartCameraShake(CameraShakeFactory);

	// 총쏘는 애니메이션 재생
	UTPSPlayerAnim* anim = Cast<UTPSPlayerAnim>(GetMesh()->GetAnimInstance());
	anim->OnFire(TEXT("Default"));

	// 총소리 재생
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());

	// 만약 Grenade 총이라면
	if (bChooseGrenadeGun)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ATPSPlayer::DoFire, FireInterval, true);
		DoFire();
	}
	// 그렇지 않다면
	else
	{
		FHitResult HitInfo;
		FVector StartTrace = CameraComponent->GetComponentLocation();
		FVector EndTrace = StartTrace + CameraComponent->GetForwardVector() * 100000;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		/*FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECollisionChannel::);
		GetWorld()->LineTraceSingleByObjectType();*/
		

		bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, StartTrace, EndTrace, ECollisionChannel::ECC_Visibility, CollisionParams);
		// UKismetSystemLibrary::SphereTraceSingle();

		// 만약 충돌한 것이 있다면
		if (bHit)
		{
			// 총알 충돌 위치에 이펙트 생성
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffectFactory, HitInfo.ImpactPoint);
			
			// 만약 충돌 대상이 Enemy 라면
			AEnemy* enemy = Cast<AEnemy>(HitInfo.GetActor()); // auto enemy
			if (enemy != nullptr) // if(enemy && enemy->IsA(AEnemy::StaticClass()))
			{
				// Enemy에게 데미지를 준다
				enemy->EnemyFSM->OnDamageProcess(1);
				/*UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("Enemy FSM")));
				fsm->OnDamageProcess(1);*/
			}
			
			 
			// 충돌한 컴포넌트 정보를 변수에 담기
			UPrimitiveComponent* HitComp = HitInfo.GetComponent();
			// 충돌한 오브젝트와 상호작용(충돌한 대상이 물리작용을 한다면)
			if (HitComp && HitComp->IsSimulatingPhysics())
			{
				// 힘을 가한다.
				FVector ForceDir = (HitInfo.TraceEnd - HitInfo.TraceStart).GetSafeNormal();
				// ForceDir.Normalize();
				FVector Force = ForceDir * 1000000 * HitComp->GetMass();
				HitComp->AddForce(Force);
			}
		}

		DoFire();
	}


}

void ATPSPlayer::OnActionFireReleased()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ATPSPlayer::OnActionGrenade()
{
	ChooseGun(GRENADE_GUN);
}

void ATPSPlayer::OnActionSniper()
{
	ChooseGun(SNIPER_GUN);
}

void ATPSPlayer::OnActionReload()
{
	UTPSPlayerAnim* anim = Cast<UTPSPlayerAnim>(GetMesh()->GetAnimInstance());
	if (bChooseGrenadeGun)
	{
		anim->OnFire(TEXT("GunReload"));
	}
	else
	{
		anim->OnFire(TEXT("SniperReload"));
	}
}

void ATPSPlayer::DoFire()
{
	// 총알 생성 위치
	FVector SpawnDirection;
	// 총알 생성 위치는 플레이어 1미터 앞
	SpawnDirection = GetActorLocation() + GetActorForwardVector() * 1.f;

	// 총 스켈레탈에서 생성한 총알 발사 위치 소켓의 Transform 데이터를 받아와서 저장
	FTransform FirePosition = GunMeshComponent->GetSocketTransform(TEXT("FirePosition"));
	// FirePosition.SetRotation(FQuat(GetControlRotation()));

	// 총알 액터 생성
	GetWorld()->SpawnActor<ABulletActor>(BulletFactory, FirePosition);
	// UGameplayStatics::, GetWorld()->, ACharacter::, UKismetLibrary:: ... 
}

void ATPSPlayer::ChooseGun(bool bGrenade)
{
	// 만약 총을 바꾸기 전이 Sniper 총이고 바꾸려는 것이 Grenade 총이면
	if (!bChooseGrenadeGun && bGrenade)
	{
		// FOV를 90, CrosshairUI O, SniperUI X
		CameraComponent->SetFieldOfView(90.f);
		CrosshairUI->AddToViewport();
		SniperUI->RemoveFromParent();
	}
	

	bChooseGrenadeGun = bGrenade;
	GunMeshComponent->SetVisibility(bGrenade);
	SniperMeshComponent->SetVisibility(!bGrenade);
}

void ATPSPlayer::OnActionZoomIn()
{
	// 만약 Grenade 총이라면 함수 종료
	if (bChooseGrenadeGun)
	{
		return;
	}

	// 만약 Sniper 총이라면
	
	// 줌아웃
	CameraComponent->SetFieldOfView(20.f);
	// CrosshairUI 를 안 보이게 하고, 확대경을 보이게 하고 싶다
	CrosshairUI->RemoveFromParent();
	SniperUI->AddToViewport();
}

void ATPSPlayer::OnActionZoomOut()
{
	// 만약 Grenade 총이라면 함수 종료
	if (bChooseGrenadeGun)
	{
		return;
	}

	// 만약 Sniper 총이라면

	// 줌인
	CameraComponent->SetFieldOfView(90.f);
	// CrosshairUI 를 안 보이게 하고, 확대경을 보이게 하고 싶다
	CrosshairUI->AddToViewport();
	SniperUI->RemoveFromParent();
}

void ATPSPlayer::OnMyGunReload()
{
	GunAmmo = MaxGunAmmo;
}

void ATPSPlayer::OnMySniperReload()
{
	SniperAmmo = MaxSniperAmmo;
}
