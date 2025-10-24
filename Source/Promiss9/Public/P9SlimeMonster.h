#pragma once

#include "CoreMinimal.h"
#include "P9Monster.h"
#include "P9SlimeMonster.generated.h"


UCLASS()
class PROMISS9_API AP9SlimeMonster : public AP9Monster
{
    GENERATED_BODY()

public:
    AP9SlimeMonster();

protected:
    virtual void BeginPlay() override;
};