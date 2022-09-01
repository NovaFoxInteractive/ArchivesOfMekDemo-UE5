// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Structs/ArmorData.h"
#include "Armor.generated.h"

/**
 * 
 */
UCLASS()
class ARCHIVESOFMEKDEMO_API AArmor : public AItem
{
	GENERATED_BODY()

public:
	AArmor();

protected:
	FArmorData ArmorData;
	
};
