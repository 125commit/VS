// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "VSAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class VS_API UVSAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UVSAssetManager& Get();
	
protected:
	virtual void StartInitialLoading() override;
};
