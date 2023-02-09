// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerFireComponent.h"
#include "TPSPlayer.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TPSPlayerAnim.h"
#include "Enemy.h"
#include "Blueprint/UserWidget.h"
#include "EnemyFSM.h"
#include "BulletActor.h"

// Sets default values for this component's properties
UTPSPlayerFireComponent::UTPSPlayerFireComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTPSPlayerFireComponent::BeginPlay()
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
void UTPSPlayerFireComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTPSPlayerFireComponent::SetupPlayerInput(UInputComponent* PlayerInputComponent)
{
	// 발사 액션 연결
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &UTPSPlayerFireComponent::OnActionFireReleased);

	// 총교체 액션 연결
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionGrenade);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionSniper);

	// 줌인/줌아웃 액션 연결
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionZoomIn);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &UTPSPlayerFireComponent::OnActionZoomOut);

	// 재장전
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionReload);
}

void UTPSPlayerFireComponent::OnActionFirePressed()
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
	UTPSPlayerAnim* anim = Cast<UTPSPlayerAnim>(owner->GetMesh()->GetAnimInstance());
	anim->OnFire(TEXT("Default"));

	// 총소리 재생
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), owner->FireSound, owner->GetActorLocation());

	// 만약 Grenade 총이라면
	if (bChooseGrenadeGun)
	{
		owner->GetWorldTimerManager().SetTimer(FireTimerHandle, this, &UTPSPlayerFireComponent::DoFire, FireInterval, true);
		DoFire();
	}
	// 그렇지 않다면
	else
	{
		FHitResult HitInfo;
		FVector StartTrace = owner->CameraComponent->GetComponentLocation();
		FVector EndTrace = StartTrace + owner->CameraComponent->GetForwardVector() * 100000;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(owner);

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

void UTPSPlayerFireComponent::OnActionFireReleased()
{
	owner->GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void UTPSPlayerFireComponent::OnActionGrenade()
{
	ChooseGun(GRENADE_GUN);
}

void UTPSPlayerFireComponent::OnActionSniper()
{
	ChooseGun(SNIPER_GUN);
}

void UTPSPlayerFireComponent::OnActionReload()
{
	UTPSPlayerAnim* anim = Cast<UTPSPlayerAnim>(owner->GetMesh()->GetAnimInstance());
	if (bChooseGrenadeGun)
	{
		anim->OnFire(TEXT("GunReload"));
	}
	else
	{
		anim->OnFire(TEXT("SniperReload"));
	}
}

void UTPSPlayerFireComponent::DoFire()
{
	// 총알 생성 위치
	FVector SpawnDirection;
	// 총알 생성 위치는 플레이어 1미터 앞
	SpawnDirection = owner->GetActorLocation() + owner->GetActorForwardVector() * 1.f;

	// 총 스켈레탈에서 생성한 총알 발사 위치 소켓의 Transform 데이터를 받아와서 저장
	FTransform FirePosition = owner->GunMeshComponent->GetSocketTransform(TEXT("FirePosition"));
	// FirePosition.SetRotation(FQuat(GetControlRotation()));

	// 총알 액터 생성
	GetWorld()->SpawnActor<ABulletActor>(BulletFactory, FirePosition);
	// UGameplayStatics::, GetWorld()->, ACharacter::, UKismetLibrary:: ... 
}

void UTPSPlayerFireComponent::ChooseGun(bool bGrenade)
{
	// 만약 총을 바꾸기 전이 Sniper 총이고 바꾸려는 것이 Grenade 총이면
	if (!bChooseGrenadeGun && bGrenade)
	{
		// FOV를 90, CrosshairUI O, SniperUI X
		owner->CameraComponent->SetFieldOfView(90.f);
		CrosshairUI->AddToViewport();
		SniperUI->RemoveFromParent();
	}


	bChooseGrenadeGun = bGrenade;
	owner->GunMeshComponent->SetVisibility(bGrenade);
	owner->SniperMeshComponent->SetVisibility(!bGrenade);
}

void UTPSPlayerFireComponent::OnActionZoomIn()
{
	// 만약 Grenade 총이라면 함수 종료
	if (bChooseGrenadeGun)
	{
		return;
	}

	// 만약 Sniper 총이라면

	// 줌아웃
	owner->CameraComponent->SetFieldOfView(20.f);
	// CrosshairUI 를 안 보이게 하고, 확대경을 보이게 하고 싶다
	CrosshairUI->RemoveFromParent();
	SniperUI->AddToViewport();
}

void UTPSPlayerFireComponent::OnActionZoomOut()
{
	// 만약 Grenade 총이라면 함수 종료
	if (bChooseGrenadeGun)
	{
		return;
	}

	// 만약 Sniper 총이라면

	// 줌인
	owner->CameraComponent->SetFieldOfView(90.f);
	// CrosshairUI 를 안 보이게 하고, 확대경을 보이게 하고 싶다
	CrosshairUI->AddToViewport();
	SniperUI->RemoveFromParent();
}

void UTPSPlayerFireComponent::OnMyGunReload()
{
	GunAmmo = MaxGunAmmo;
}

void UTPSPlayerFireComponent::OnMySniperReload()
{
	SniperAmmo = MaxSniperAmmo;
}
