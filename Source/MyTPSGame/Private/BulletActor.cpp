// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../MyTPSGame.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Sphere Component 생성
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	// SphereCollision을 RootComponent로 설정
	SetRootComponent(SphereCollision);
	// Sphere Component의 반지름을 13으로 변경
	SphereCollision->SetSphereRadius(13.f);
	// Sphere Component의 콜리전 설정을 BlockAll로 변경
	SphereCollision->SetCollisionProfileName(TEXT("BlockAll"));

	// Mesh Component 생성
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	// MeshComponoent를 RootComponent에 부착
	MeshComponent->SetupAttachment(RootComponent);
	// Mesh 크기를 0.25 배로 변경
	MeshComponent->SetRelativeScale3D(FVector(0.25f));
	// Mesh Component의 콜리전 설정을 NoCollsion으로 변경
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// MeshCommponent->SetCollisionProfileName(TEXT("NoCollision"));

	// Projectile Movement Component 생성
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	// Movement Component가 실제로 움직여야 할 컴포넌트를 설정
	ProjectileComponent->SetUpdatedComponent(RootComponent);
	// 발사체의 초기 속도를 5000으로 설정
	ProjectileComponent->InitialSpeed = 5000.f;
	// 발사체의 최대 속도를 5000으로 설정
	ProjectileComponent->MaxSpeed = 5000.f;
	// 발사체의 튕김(?) 여부를 true로 설정
	ProjectileComponent->bShouldBounce = true;
	// 발사체의 튕김 정도 설정
	ProjectileComponent->Bounciness = 0.5f;
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Life Span 설정을 통해 액터를 삭제하는 방법
	// SetLifeSpan(2);
	// InitialLifeSpan = 2;

	//// 타이머를 통해 액터를 삭제하는 방법
	//FTimerHandle DieTimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(DieTimerHandle, this, &ABulletActor::OnDie, 2.f);
	//// GetWorldTimerManager().SetTimer()
 

	// Lambda 함수 & 무명 함수
	// auto FuncName = [메모리 상에서 캡처할 대상](함수의 매개변수)->반환타입{구현}
	// auto : 자료형을 지금이 아니라 런타임에 결정
	/*캡처한 대상은 함수에서 사용할 수 있다
	int number = 1;
	auto myPrint = [number]()->void{int b = number;};*/
	/*auto MyPlus = [this](int a, int b)->int{ return a + b; };
	PRINT_LOG(TEXT("%d"), MyPlus(10, 20));*/



	// 람다식을 활용해 액터를 삭제하는 방법
	FTimerHandle DieTimerHandle;
	GetWorldTimerManager().SetTimer(DieTimerHandle, FTimerDelegate::CreateLambda([this]()->void{this->Destroy();}), 2.f, false);
}

// Called every frame
void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABulletActor::OnDie()
{
	Destroy();
}