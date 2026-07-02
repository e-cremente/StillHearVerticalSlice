#include "Character/StillHearCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayAbilitySystem/Tags/GameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySystem/Attributes/BasicAttributeSet.h"
#include "GameplayAbilitySystem/Component/StillHearAbilitySystemComponent.h"

// Sets default values
AStillHearCharacterBase::AStillHearCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Add the Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UStillHearAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

UAbilitySystemComponent* AStillHearCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TArray<FGameplayAbilitySpecHandle> AStillHearCharacterBase::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	if (!AbilitySystemComponent)
		return TArray<FGameplayAbilitySpecHandle>();

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	
	for (const TSubclassOf Ability : AbilitiesToGrant)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));
		AbilityHandles.Add(SpecHandle);
	}

	SendAbilitiesChangedEvent();
	return AbilityHandles;
}

void AStillHearCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove)
{
	if (!AbilitySystemComponent)
		return;

	for (FGameplayAbilitySpecHandle AbilityHandle : AbilityHandlesToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void AStillHearCharacterBase::SendAbilitiesChangedEvent()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}

// Called when the game starts or when spawned
void AStillHearCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AStillHearCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (!bStartingAbilitiesGranted)
		{
			GrantAbilities(StartingAbilities);
			bStartingAbilitiesGranted = true;
		}
	}
}

void AStillHearCharacterBase::SendGameplayEventToSelf(const FGameplayTag EventTag)
{
	FGameplayEventData EventData;
	EventData.EventTag = EventTag;
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		EventData.EventTag,
		EventData
	);
}

// Called every frame
void AStillHearCharacterBase::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStillHearCharacterBase::OnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (!AbilitySystemComponent)
		return;

	if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		AbilitySystemComponent->AddLooseGameplayTag(TAG_Status_Falling);
	}
	else if (PrevMovementMode == MOVE_Falling)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(TAG_Status_Falling);
	}
}

void AStillHearCharacterBase::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Notify GAS that the character has physically uncrouched
	SendGameplayEventToSelf(TAG_Event_MainCharacter_EndCrouch);
}

