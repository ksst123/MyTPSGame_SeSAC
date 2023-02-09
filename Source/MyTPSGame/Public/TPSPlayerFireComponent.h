// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerBaseComponent.h"
#include "TPSPlayerFireComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYTPSGAME_API UTPSPlayerFireComponent : public UTPSPlayerBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSPlayerFireComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupPlayerInput(class UInputComponent* PlayerInputComponent) override;		

	// �Ѿ� ����
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> BulletFactory;
	// UClass�� ����ص� ���� ���� ������ ����������, UClass�� ������ �ʹ� �о� ������ �߻��� �� �ֱ� ������ TSubobject<>�� ��Ȯ�� ������ �����ش�.
	// class UClass* BulletFactory;

	// �Ѿ� �浹 ����Ʈ ����
	UPROPERTY(EditAnywhere)
	class UParticleSystem* BulletEffectFactory;

	// ���� ���忡�� ������ ����(Crosshair, Sniper)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> CrosshairFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> SniperFactory;

	UPROPERTY()
	class UUserWidget* CrosshairUI;
	UPROPERTY()
	class UUserWidget* SniperUI;

	UPROPERTY(EditAnywhere)
	float FireInterval = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
	bool bChooseGrenadeGun;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxGunAmmo = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GunAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxSniperAmmo = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SniperAmmo;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> CameraShakeFactory;
	UPROPERTY()
	class UCameraShakeBase* canShakeInstance;

	FTimerHandle FireTimerHandle;


	void OnActionFirePressed();
	void OnActionFireReleased();
	void OnActionReload();

	// 1�� Ű�� 2�� Ű�� ���� �ѱ�ü �Է�ó�� �Լ�
	void OnActionGrenade();
	void OnActionSniper();

	// ������ �Ѿ��� �߻��ϴ� �Լ�
	void DoFire();

	void OnMyGunReload();
	void OnMySniperReload();

	void ChooseGun(bool bGrenade);

	// Zoom In / Out ���� �Լ���
	void OnActionZoomIn(); // Ȯ�� (FOV 30)
	void OnActionZoomOut(); // ��� (FOV 90)



};
