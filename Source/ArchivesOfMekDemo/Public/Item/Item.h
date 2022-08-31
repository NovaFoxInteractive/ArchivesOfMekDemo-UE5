// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/ItemData.h"
#include "Item.generated.h"


UCLASS()
class ARCHIVESOFMEKDEMO_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	virtual bool UseItem_Int();
	
	UFUNCTION(BlueprintNativeEvent)
	bool UseItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FItemData ItemData;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FItemData GetItemData() const { return ItemData; }

};
