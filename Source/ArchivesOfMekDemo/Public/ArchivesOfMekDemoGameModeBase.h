// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright NovaFox Interactive L.L.C 2021


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GlobalEnum/EDifficulty.h"
#include "ArchivesOfMekDemoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API AArchivesOfMekDemoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Difficulty)
	EDifficulty Difficulty = EDifficulty::ED_Normal;
};
