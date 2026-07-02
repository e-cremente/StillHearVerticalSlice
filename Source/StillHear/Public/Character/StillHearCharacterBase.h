#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "StillHearCharacterBase.generated.h"

UCLASS()
class STILLHEAR_API AStillHearCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

#pragma region UPROPERTY
public:
	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// This variable will need to be moved in the MainCharacter, as AI Characters don't need it since they read it from data asset
	// I will not do it for now because I assume it will break the mantis for how we currently have it - Edo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterStatistics")
	float BaseSpeed;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> StartingAbilities;

	// Prevents granting StartingAbilities again on a subsequent PossessedBy (e.g. AI controller re-possession),
	// which would otherwise create duplicate ability specs and cause GameplayEvents to trigger them twice
	bool bStartingAbilitiesGranted = false;
#pragma endregion

#pragma region CONSTRUCTOR
public:
	AStillHearCharacterBase();
#pragma endregion 

#pragma region UFUNCTIONS
public:
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	TArray<FGameplayAbilitySpecHandle> GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void SendAbilitiesChangedEvent();

protected:
	UFUNCTION(BlueprintCallable, Category = "AbilitySystem")
	void SendGameplayEventToSelf(FGameplayTag EventTag);
#pragma endregion 
	
#pragma region METHODS
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	// Called when the character physically uncrouches
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
	// Gameplay tag event handlers
	virtual void HandleDeath(const FGameplayTag DeathTag, int32 NewCount) {};
#pragma endregion 
};
