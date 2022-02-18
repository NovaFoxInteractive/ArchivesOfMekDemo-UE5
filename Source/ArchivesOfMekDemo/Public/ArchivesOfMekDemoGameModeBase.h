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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Difficulty, meta=(AllowPrivateAccess = "true"))
	EDifficulty Difficulty = EDifficulty::ED_Normal;

public:
	FORCEINLINE EDifficulty GetDifficulty() const { return Difficulty; }

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(const EDifficulty NewDifficulty);
};
