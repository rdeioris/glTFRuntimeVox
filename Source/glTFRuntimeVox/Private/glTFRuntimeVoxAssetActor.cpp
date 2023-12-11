// Copyright 2023, Roberto De Ioris.

#include "glTFRuntimeVoxAssetActor.h"

// Sets default values
AglTFRuntimeVoxAssetActor::AglTFRuntimeVoxAssetActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AssetRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AssetRoot"));
	RootComponent = AssetRoot;

	FrameIndex = 0;
}

void AglTFRuntimeVoxAssetActor::ManageAttachments(USceneComponent* CurrentParentComponent, const TArray<int32>& CurrentChildren)
{
	for (const int32 ChildId : CurrentChildren)
	{
		if (ShapesMap.Contains(ChildId))
		{
			const int32 ModelIndex = ShapesMap[ChildId];
			if (ModelsMap.Contains(ModelIndex))
			{
				UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(this, MakeUniqueObjectName(this, USceneComponent::StaticClass(), *FString::Printf(TEXT("nSHP_%u"), ChildId)));
				if (StaticMeshComponent)
				{
					StaticMeshComponent->SetupAttachment(CurrentParentComponent);
					StaticMeshComponent->RegisterComponent();
					AddInstanceComponent(StaticMeshComponent);
					StaticMeshComponent->SetStaticMesh(ModelsMap[ModelIndex]);
					ReceiveOnStaticMeshComponentCreated(StaticMeshComponent);
				}
			}
		}
		else if (NodesMap.Contains(ChildId))
		{
			NodesMap[ChildId]->AttachToComponent(CurrentParentComponent, FAttachmentTransformRules::KeepRelativeTransform);
			ManageAttachments(NodesMap[ChildId], UglTFRuntimeVoxFunctionLibrary::GetVoxNodeChildrenId(Asset, ChildId));
		}
	}
}

// Called when the game starts or when spawned
void AglTFRuntimeVoxAssetActor::BeginPlay()
{
	Super::BeginPlay();

	if (!Asset)
	{
		return;
	}

	NodesMap.Empty();
	ShapesMap.Empty();
	ModelsMap.Empty();

	for (int32 ModelIndex = 0; ModelIndex < UglTFRuntimeVoxFunctionLibrary::GetNumVoxModels(Asset); ModelIndex++)
	{
		FglTFRuntimeMeshLOD RuntimeLOD;
		if (UglTFRuntimeVoxFunctionLibrary::LoadVoxModelAsRuntimeLOD(Asset, ModelIndex, RuntimeLOD, VoxConfig, StaticMeshConfig.MaterialsConfig))
		{
			UStaticMesh* StaticMesh = Asset->LoadStaticMeshFromRuntimeLODs({ RuntimeLOD }, StaticMeshConfig);
			if (StaticMesh)
			{
				ModelsMap.Add(ModelIndex, StaticMesh);
			}
		}
	}

	TArray<int32> NodesId = UglTFRuntimeVoxFunctionLibrary::GetVoxNodesId(Asset);

	for (const int32 NodeId : NodesId)
	{
		const FString NodeName = UglTFRuntimeVoxFunctionLibrary::GetVoxNodeName(Asset, NodeId);

		const int32 ModelIndex = UglTFRuntimeVoxFunctionLibrary::GetVoxNodeModelIndex(Asset, NodeId, FrameIndex);
		if (ModelIndex < 0)
		{
			USceneComponent* SceneComponent = NewObject<USceneComponent>(this, MakeUniqueObjectName(this, USceneComponent::StaticClass(), *NodeName));
			if (SceneComponent)
			{
				SceneComponent->SetupAttachment(GetRootComponent());
				SceneComponent->RegisterComponent();
				AddInstanceComponent(SceneComponent);
				FTransform Transform = UglTFRuntimeVoxFunctionLibrary::GetVoxNodeTransform(Asset, NodeId, FrameIndex);
				Transform.ScaleTranslation(VoxConfig.VoxelSize);
				SceneComponent->SetRelativeTransform(Transform);
			}
			NodesMap.Add(NodeId, SceneComponent);
		}
		else
		{
			ShapesMap.Add(NodeId, ModelIndex);
		}
	}

	TArray<int32> RootNodes = UglTFRuntimeVoxFunctionLibrary::GetVoxRootNodesId(Asset);
	ManageAttachments(GetRootComponent(), RootNodes);
}

// Called every frame
void AglTFRuntimeVoxAssetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AglTFRuntimeVoxAssetActor::ReceiveOnStaticMeshComponentCreated_Implementation(UStaticMeshComponent* StaticMeshComponent)
{

}
