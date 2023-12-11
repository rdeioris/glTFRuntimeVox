// Copyright 2023, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "glTFRuntimeVoxFunctionLibrary.h"
#include "glTFRuntimeVoxAssetActor.generated.h"

UCLASS()
class GLTFRUNTIMEVOX_API AglTFRuntimeVoxAssetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AglTFRuntimeVoxAssetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "glTFRuntime|Vox")
	UglTFRuntimeAsset* Asset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "glTFRuntime|Vox")
	int32 FrameIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "glTFRuntime|Vox")
	FglTFRuntimeVoxConfig VoxConfig;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "glTFRuntime|Vox")
	FglTFRuntimeStaticMeshConfig StaticMeshConfig;

	UFUNCTION(BlueprintNativeEvent, Category = "glTFRuntime|Vox", meta = (DisplayName = "On StaticMeshComponent Created"))
	void ReceiveOnStaticMeshComponentCreated(UStaticMeshComponent* StaticMeshComponent);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "glTFRuntime|Vox")
	USceneComponent* AssetRoot;

protected:
	TMap<int32, USceneComponent*> NodesMap;
	TMap<int32, int32> ShapesMap;
	TMap<int32, UStaticMesh*> ModelsMap;

	void ManageAttachments(USceneComponent* ParentComponent, const TArray<int32>& Children);
};
