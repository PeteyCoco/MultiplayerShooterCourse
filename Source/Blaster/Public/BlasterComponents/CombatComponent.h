// Copyright Peter Carsten Collins (2024)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABlasterCharacter;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABlasterCharacter;

//~ Begin UActorComponent interface
protected:
	virtual void BeginPlay() override;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//~ End UActorComponent interface

public:
	void EquipWeapon(AWeapon* WeaponToEquip);

private:
	TObjectPtr<ABlasterCharacter> Character;
	TObjectPtr<AWeapon> EquippedWeapon;
};
