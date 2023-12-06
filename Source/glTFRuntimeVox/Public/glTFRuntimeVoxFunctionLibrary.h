// Copyright 2023, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeAsset.h"
#include "Engine/TextureDefines.h"
#include "glTFRuntimeVoxFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FglTFRuntimeVoxConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|Vox")
	float VoxelSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|Vox")
	ETextureColorSpace ColorSpace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|Vox")
	float GammaCorrection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|Vox")
	bool bRemoveHiddenVoxels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|Vox")
	float Padding;

	FglTFRuntimeVoxConfig()
	{
		VoxelSize = 100.0f;
		ColorSpace = ETextureColorSpace::TCS_sRGB;
		GammaCorrection = 2.2f;
		bRemoveHiddenVoxels = false;
		Padding = 0;
	}
};

/**
 * 
 */
UCLASS()
class GLTFRUNTIMEVOX_API UglTFRuntimeVoxFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "MaterialsConfig", AutoCreateRefTerm = "VoxConfig,MaterialsConfig"), Category = "glTFRuntime|Vox")
	static bool LoadVoxModelAsRuntimeLOD(UglTFRuntimeAsset* Asset, const int32 ModelIndex, FglTFRuntimeMeshLOD& RuntimeLOD, const FglTFRuntimeVoxConfig& VoxConfig, const FglTFRuntimeMaterialsConfig& MaterialsConfig);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "VoxConfig"), Category = "glTFRuntime|Vox")
	static bool LoadVoxModelAsInstances(UglTFRuntimeAsset* Asset, const int32 ModelIndex, TArray<FTransform>& Transforms, TArray<FLinearColor>& Colors, const FglTFRuntimeVoxConfig& VoxConfig);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static FIntVector GetVoxModelSize(UglTFRuntimeAsset* Asset, const int32 ModelIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static int32 GetVoxVersion(UglTFRuntimeAsset* Asset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static int32 GetNumVoxModels(UglTFRuntimeAsset* Asset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static int32 GetNumVoxNodes(UglTFRuntimeAsset* Asset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static TArray<int32> GetVoxNodesId(UglTFRuntimeAsset* Asset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static TArray<int32> GetVoxRootNodesId(UglTFRuntimeAsset* Asset);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static TArray<int32> GetVoxNodeChildrenId(UglTFRuntimeAsset* Asset, const int32 NodeId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static int32 GetVoxNodeModelIndex(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static FString GetVoxNodeName(UglTFRuntimeAsset* Asset, const int32 NodeId);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "glTFRuntime|Vox")
	static FTransform GetVoxNodeTransform(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0);
};
