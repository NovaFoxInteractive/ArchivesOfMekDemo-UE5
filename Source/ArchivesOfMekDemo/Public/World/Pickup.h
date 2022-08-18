// Copyright NovaFox Interactive L.L.C 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class ARCHIVESOFMEKDEMO_API APickup : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Components, meta=(AllowPrivateAccess="true"))
	class UInteractionComponent* InteractionComponent;
	
public:	
	// Sets default values for this actor's properties
	APickup();

	void InitializePickup(const TSubclassOf<class UItem> ItemClass, const int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent)
	void AlignWithGround();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	class UItem* ItemTemplate;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	class UItem* Item;

	UFUNCTION()
	void OnItemModified();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION()
	void OnTakePickup(class ACharacterBase* Character);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
