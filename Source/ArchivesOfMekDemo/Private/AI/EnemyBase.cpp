// Copyright NovaFox Interactive L.L.C 2021


#include "AI/EnemyBase.h"

#include "AI/EnemyControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CharacterBase.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AEnemyBase::AEnemyBase():
	Health(100.f), MaxHealth(100.f),
	HealthBarDisplayTime(1.5f),
	bInAttackRange(false),
	BaseDamage(20.f),
	bCanAttack(true), AttackWaitTime(1.f),
	bCanHitReact(true),
	HitReactTimeMin(.5f), HitReactTimeMax(3.f),
	AttackLFast(TEXT("AttackLFast")), AttackRFast(TEXT("AttackRFast")),
	AttackL(TEXT("AttackL")), AttackR(TEXT("AttackR")),
	LeftWeaponSocket(TEXT("FX_Trail_L_01")), RightWeaponSocket(TEXT("FX_Trail_R_01")),
	bStunned(false), StunChance(0.5f),
	bDying(false), DeathTime(4.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroShpere"));
	AgroSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRange"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftWeaponBox"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("LeftWeaponBone"));
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightWeaponBox"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("RightWeaponBone"));
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::AgroSphereOverlap);
	
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemyBase::CombatRangeEndOverlap);
	
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnRightWeaponOverlap);

	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	WorldRef = GetWorld();

	EnemyController = Cast<AEnemyControllerBase>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);

		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), false);
	}
	
}

#if WITH_EDITOR
void AEnemyBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if(ChangedPropertyName == GET_MEMBER_NAME_CHECKED(AEnemyBase, StunChance))
		StunChance = FMath::Clamp(StunChance, 0.f, 1.f);
}
#endif

void AEnemyBase::AgroSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor) return;

	if (const auto Character = Cast<ACharacterBase>(OtherActor))
		EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
	
}

void AEnemyBase::CombatRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		bInAttackRange = true;
		if(EnemyController)
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
	}
}

void AEnemyBase::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;

	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		bInAttackRange = false;
		if(EnemyController)
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), bInAttackRange);
	}
}

void AEnemyBase::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr) return;

	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		DoDamage(Character);
		SpawnBlood(Character, LeftWeaponSocket);
	}
}

void AEnemyBase::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor == nullptr) return;

	if (const auto Character = Cast<ACharacterBase>(OtherActor))
	{
		DoDamage(Character);
		SpawnBlood(Character, RightWeaponSocket);
	}
}

void AEnemyBase::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyBase::DeactivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBase::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemyBase::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemyBase::DoDamage(ACharacterBase* Character)
{
	UGameplayStatics::ApplyDamage(Character, BaseDamage, EnemyController, this, UDamageType::StaticClass());
	if(Character->GetMeleeImpactSound())
		UGameplayStatics::PlaySoundAtLocation(this, Character->GetMeleeImpactSound(), GetActorLocation());
}

void AEnemyBase::SpawnBlood(ACharacterBase* Victim, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket{ GetMesh()->GetSocketByName(SocketName) };
	if(TipSocket)
	{
		const FTransform SocketTransform{ TipSocket->GetSocketTransform(GetMesh()) };
		if (Victim->GetBloodParticles())
			UGameplayStatics::SpawnEmitterAtLocation(WorldRef, Victim->GetBloodParticles(), SocketTransform);
	}
}

FName AEnemyBase::GetAttackSectionName() const
{
	FName SectionName;
	switch (const int32 Section{FMath::RandRange(1, 4)})
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}
	return SectionName;
}

void AEnemyBase::Die()
{
	if(bDying) return;
	bDying = true;
	
	HideHealthBar();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if(AnimInstance && DeathMontage)
		AnimInstance->Montage_Play(DeathMontage);

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), true);
		EnemyController->StopMovement();
	}
}

void AEnemyBase::FinishDeath()
{
	GetMesh()->bPauseAnims = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemyBase::DestroyEnemy, DeathTime);
}

void AEnemyBase::DestroyEnemy()
{
	Destroy();
}

void AEnemyBase::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), Stunned);
	}
}

void AEnemyBase::PlayHitMontage(FName Section, float PlayRate)
{
	if(bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}

		bCanHitReact = false;
		const float HitReactTime{FMath::FRandRange(HitReactTimeMin, HitReactTimeMax)};
		GetWorldTimerManager().SetTimer(HitReactTimer, this, &AEnemyBase::ResetHitReactTimer, HitReactTime);
	}
}

void AEnemyBase::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
	bCanAttack = false;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AEnemyBase::ResetCanAttack, AttackWaitTime);

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), false);
	}
}

void AEnemyBase::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &AEnemyBase::HideHealthBar, HealthBarDisplayTime);
}

void AEnemyBase::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemyBase::ResetCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"), true);
	}
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (ImpactSound)
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	ShowHealthBar();

	// Determine whether hit stuns
	const float Stunned = FMath::FRandRange(0.f, 1.f);
	if (Stunned <= StunChance)
	{
		//Stun the Enemy
		PlayHitMontage(FName(TEXT("HitReactFront")));
		SetStunned(true);
	}
	
	if (EnemyController)
		EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), DamageCauser);
	
	
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();
	}
	else
		Health -= DamageAmount;

	return DamageAmount;
}

