// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyTPSGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MYTPSGAME_API AMyTPSGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	


public:
	AMyTPSGameGameModeBase();

	virtual void BeginPlay() override;

	UPROPERTY()
	class ASpawnManager* SpawnManager;

	// ������ ������ ���� ���� �÷��̾��� ������ ����.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Level = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NeedExp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 CurrentExp = 0;

	void LevelUp();
	void AddExp(int32 amount);
	void LevelUpProcess();
};
