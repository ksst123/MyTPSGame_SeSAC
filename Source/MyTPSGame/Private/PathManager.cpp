// Fill out your copyright notice in the Description page of Project Settings.


#include "PathManager.h"
#include "Kismet/GameplayStatics.h"
#include "WayPoint.h"

// Sets default values
APathManager::APathManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APathManager::BeginPlay()
{
	Super::BeginPlay();
	

	// 레벨에 존재하는 WayPoint들을 다 가져와서
	// 가져온 WayPoint들을 목록에 넣는다.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWayPoint::StaticClass(), WayPoints);

	
}

// Called every frame
void APathManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

