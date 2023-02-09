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
	
	// UI ����
	CrosshairUI = CreateWidget(GetWorld(), CrosshairFactory);
	SniperUI = CreateWidget(GetWorld(), SniperFactory);
	// CrosshairUI�� ���۽� �⺻���� ȭ�鿡 ���
	CrosshairUI->AddToViewport();
	// Grenade Gun�� ���۽� �⺻ ������ ����
	ChooseGun(GRENADE_GUN);

	// Sniper Gun�� �� ZoomIn�� �ϸ� CrosshairUI X, SniperUI O
	// Sniper Gun�� �� ZoomOut�� �ϸ� CrosshairUI O, SniperUI X
	// �⺻��(Grenade)�� �����ϸ� CrosshairUI O, SniperUI X

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
	// �߻� �׼� ����
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionFirePressed);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &UTPSPlayerFireComponent::OnActionFireReleased);

	// �ѱ�ü �׼� ����
	PlayerInputComponent->BindAction(TEXT("GrenadeGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionGrenade);
	PlayerInputComponent->BindAction(TEXT("SniperGun"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionSniper);

	// ����/�ܾƿ� �׼� ����
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionZoomIn);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &UTPSPlayerFireComponent::OnActionZoomOut);

	// ������
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &UTPSPlayerFireComponent::OnActionReload);
}

void UTPSPlayerFireComponent::OnActionFirePressed()
{
	// �Ѿ� �����ִ��� ����
	if (bChooseGrenadeGun)
	{
		// �Ѿ��� �����ִٸ� 1�� �����ϰ�
		if (GunAmmo > 0)
		{
			GunAmmo--;
		}
		// �׷��� ������ ���� ���� ���Ѵ�.
		else
		{
			return;
		}
	}
	else
	{
		// �Ѿ��� �����ִٸ� 1�� �����ϰ�
		if (SniperAmmo > 0)
		{
			SniperAmmo--;
		}
		// �׷��� ������ ���� ���� ���Ѵ�.
		else
		{
			return;
		}
	}

	// ī�޶� ����
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	// ī�޶� ����
	/*auto controller = Cast<APlayerController>(GetController());
	controller->PlayerCameraManager;*/

	CameraManager->StartCameraShake(CameraShakeFactory);
	// ���� ī�޶� �̹� ���� ���̸�(=���� canShakeInstance�� nullptr�� �ƴϰ�, �������̶��)
	if (canShakeInstance != nullptr && !(canShakeInstance->IsFinished()))
	{
		// ����ϰ�
		CameraManager->StopCameraShake(canShakeInstance);
	}
	// ī�޶� ����
	canShakeInstance = CameraManager->StartCameraShake(CameraShakeFactory);

	// �ѽ�� �ִϸ��̼� ���
	UTPSPlayerAnim* anim = Cast<UTPSPlayerAnim>(owner->GetMesh()->GetAnimInstance());
	anim->OnFire(TEXT("Default"));

	// �ѼҸ� ���
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), owner->FireSound, owner->GetActorLocation());

	// ���� Grenade ���̶��
	if (bChooseGrenadeGun)
	{
		owner->GetWorldTimerManager().SetTimer(FireTimerHandle, this, &UTPSPlayerFireComponent::DoFire, FireInterval, true);
		DoFire();
	}
	// �׷��� �ʴٸ�
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

		// ���� �浹�� ���� �ִٸ�
		if (bHit)
		{
			// �Ѿ� �浹 ��ġ�� ����Ʈ ����
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletEffectFactory, HitInfo.ImpactPoint);

			// ���� �浹 ����� Enemy ���
			AEnemy* enemy = Cast<AEnemy>(HitInfo.GetActor()); // auto enemy
			if (enemy != nullptr) // if(enemy && enemy->IsA(AEnemy::StaticClass()))
			{
				// Enemy���� �������� �ش�
				enemy->EnemyFSM->OnDamageProcess(1);
				/*UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("Enemy FSM")));
				fsm->OnDamageProcess(1);*/
			}


			// �浹�� ������Ʈ ������ ������ ���
			UPrimitiveComponent* HitComp = HitInfo.GetComponent();
			// �浹�� ������Ʈ�� ��ȣ�ۿ�(�浹�� ����� �����ۿ��� �Ѵٸ�)
			if (HitComp && HitComp->IsSimulatingPhysics())
			{
				// ���� ���Ѵ�.
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
	// �Ѿ� ���� ��ġ
	FVector SpawnDirection;
	// �Ѿ� ���� ��ġ�� �÷��̾� 1���� ��
	SpawnDirection = owner->GetActorLocation() + owner->GetActorForwardVector() * 1.f;

	// �� ���̷�Ż���� ������ �Ѿ� �߻� ��ġ ������ Transform �����͸� �޾ƿͼ� ����
	FTransform FirePosition = owner->GunMeshComponent->GetSocketTransform(TEXT("FirePosition"));
	// FirePosition.SetRotation(FQuat(GetControlRotation()));

	// �Ѿ� ���� ����
	GetWorld()->SpawnActor<ABulletActor>(BulletFactory, FirePosition);
	// UGameplayStatics::, GetWorld()->, ACharacter::, UKismetLibrary:: ... 
}

void UTPSPlayerFireComponent::ChooseGun(bool bGrenade)
{
	// ���� ���� �ٲٱ� ���� Sniper ���̰� �ٲٷ��� ���� Grenade ���̸�
	if (!bChooseGrenadeGun && bGrenade)
	{
		// FOV�� 90, CrosshairUI O, SniperUI X
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
	// ���� Grenade ���̶�� �Լ� ����
	if (bChooseGrenadeGun)
	{
		return;
	}

	// ���� Sniper ���̶��

	// �ܾƿ�
	owner->CameraComponent->SetFieldOfView(20.f);
	// CrosshairUI �� �� ���̰� �ϰ�, Ȯ����� ���̰� �ϰ� �ʹ�
	CrosshairUI->RemoveFromParent();
	SniperUI->AddToViewport();
}

void UTPSPlayerFireComponent::OnActionZoomOut()
{
	// ���� Grenade ���̶�� �Լ� ����
	if (bChooseGrenadeGun)
	{
		return;
	}

	// ���� Sniper ���̶��

	// ����
	owner->CameraComponent->SetFieldOfView(90.f);
	// CrosshairUI �� �� ���̰� �ϰ�, Ȯ����� ���̰� �ϰ� �ʹ�
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
