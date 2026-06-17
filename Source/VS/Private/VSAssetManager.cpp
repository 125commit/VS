// LAvid


#include "VSAssetManager.h"
#include "VSGameplayTags.h"

UVSAssetManager& UVSAssetManager::Get()
{
	check(GEngine);
	UVSAssetManager* AssetManager =  Cast<UVSAssetManager>(GEngine->AssetManager);
	return *AssetManager;
}

void UVSAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FVSGameplayTags::Get().InitializeNativeGameplayTags();
}
