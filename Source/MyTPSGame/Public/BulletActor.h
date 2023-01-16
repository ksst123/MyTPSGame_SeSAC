// Fill out your copyright notice in the Description page of Project Settings.

#pragma once // 중복으로 헤더가 include 되는 것을 막아줌

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletActor.generated.h"

UCLASS()
class MYTPSGAME_API ABulletActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sphere Collision Component
	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	// 외형 컴포넌트
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	// Projectile Component
	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileComponent;

	// 액터 삭제 함수
	UFUNCTION()
	void OnDie();
};
