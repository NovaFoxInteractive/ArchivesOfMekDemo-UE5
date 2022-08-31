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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Components, meta=(AllowPrivateAccess="true"))
	class UStaticMeshComponent* PickupMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Components, meta=(AllowPrivateAccess="true"))
	class USphereComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= Components, meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* PromptWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess="true"))
	TSubclassOf<class AItem> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess="true"))
	int32 Amount;

	bool bUpdateActor;

	class ACharacterBase* CharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Pickup, meta=(AllowPrivateAccess="true"))
	float PickupDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= UI, meta=(AllowPrivateAccess="true"))
	float WidgetDistanceAboveMesh;
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void SphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void SphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void PickupToggle(bool bCanPickup);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool PickupInput();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UStaticMeshComponent* GetPickupMesh() const { return PickupMesh; }
	FORCEINLINE void UpdateAmount(const int32 NewAmount) { Amount = NewAmount; }

};
