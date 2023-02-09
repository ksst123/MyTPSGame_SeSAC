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

	// 총알 공장
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABulletActor> BulletFactory;
	// UClass를 사용해도 위와 같은 구현이 가지하지만, UClass의 범위가 너무 넓어 문제가 발생할 수 있기 때문에 TSubobject<>로 정확히 범위를 정해준다.
	// class UClass* BulletFactory;

	// 총알 충돌 이펙트 공장
	UPROPERTY(EditAnywhere)
	class UParticleSystem* BulletEffectFactory;

	// 위젯 공장에서 위젯을 생성(Crosshair, Sniper)
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

	// 1번 키와 2번 키에 대한 총교체 입력처리 함수
	void OnActionGrenade();
	void OnActionSniper();

	// 실제로 총알을 발사하는 함수
	void DoFire();

	void OnMyGunReload();
	void OnMySniperReload();

	void ChooseGun(bool bGrenade);

	// Zoom In / Out 구현 함수들
	void OnActionZoomIn(); // 확대 (FOV 30)
	void OnActionZoomOut(); // 축소 (FOV 90)



};
