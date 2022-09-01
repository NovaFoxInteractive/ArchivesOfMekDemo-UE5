// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Structs/WeaponData.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FWeaponData WeaponData;
	
};
