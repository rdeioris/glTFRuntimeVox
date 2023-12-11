# glTFRuntimeVox
MagicaVoxel loader for glTFRuntime
![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/94ac53f9-32c6-49e2-b9cd-2df35ea57592)

Unreal 4.27, 5.0, 5.1, 5.2 and 5.3 are supported.

## Quickstart

Starting from a c++ project (with glTFRuntime enabled), clone the repository into the Plugins/ directory of your project (create the directory if it does not exist) and regenrate the solution.

After compilation the ```AglTFRuntimeVoxAssetActor``` Class will be available for quickly loading Vox scenes:
![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/566de179-4b67-449f-b07c-7c257629a7ee)

Ensure the set the 'as blob' flag in the loader config (even when loading from urls!)

The VoxConfig structure allows you to define the voxel size as well as distance between voxels (padding), colorspace and gamma correction:

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/7c3b6082-2816-42eb-8a58-1d4eb8212525)

The "Remove Hidden Voxels" option is an optimization avoiding "internal" voxel to be generated (internal voxels are those one fully surrounded by other volxels)

## Model and Nodes

A Vox scene contains one or more models (collection of voxels). Those models are instantiated with nodes (a single model can be instantiated multiple times in various positions and orientations).

You can load a Vox model as a RuntimeLOD (https://github.com/rdeioris/glTFRuntime-docs/blob/master/LOD.md) ready to become a StaticMesh:

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/1586b4cb-e7df-4547-8f48-e9234b4568e5)

Or as a VolumeTexture (https://docs.unrealengine.com/4.26/en-US/RenderingAndGraphics/Textures/VolumeTextures/UsingVolumeTextures/) to be used as a source for raymarching/marchingcubes algorithms

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/8d179e5f-0ae0-4366-9649-1c817351dbd3)

Or as a series of instances in an InstancedStaticMeshComponent (the function will return an array of transforms and colors, you are free to use any shape for representing a single voxel):

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/08537c06-2994-48d6-9276-0707ad04d29e)

You can get the size (number of voxels for each axis) of a model using the ```FIntVector GetVoxModelSize(UglTFRuntimeAsset* Asset, const int32 ModelIndex)``` method.

Each node has a unique Id (an int32). Given the Id you can call the following methods:

```
TArray<int32> GetVoxNodeChildrenId(UglTFRuntimeAsset* Asset, const int32 NodeId)
int32 GetVoxNodeModelIndex(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0)
FString GetVoxNodeName(UglTFRuntimeAsset* Asset, const int32 NodeId)
FTransform GetVoxNodeTransform(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0)
TMap<FString, FString> GetVoxNodeAttributes(UglTFRuntimeAsset* Asset, const int32 NodeId)
TArray<int32> GetVoxNodeFrameIndices(UglTFRuntimeAsset* Asset, const int32 NodeId)
```

## Animations

A Vox scene can be composed of multple animation frames. Each frame represent the state (Transform and Model) of a Node in the timeline.

The get the list of frame indices you can call ```TArray<int32> GetVoxFrameIndices(UglTFRuntimeAsset* Asset)```

You can use those indices for loading a whole ```AglTFRuntimeVoxAssetActor``` for each Frame (just specify the FrameIndex property) or to manually select a Transform and a Model using ```int32 GetVoxNodeModelIndex(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0)``` and ```FTransform GetVoxNodeTransform(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex = 0)```

Given that the "animation" concept does not play well with static meshes, you generally want to have actors/components preloaded with frames and just switch them at specific framerate:

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/3a81cc63-425b-402b-b977-800c09ee2b9f)

## Saving as Unreal assets

## Usage in Unreal Engine 4

## TODO/WIP

* Layers support
* Multiple palettes support
* Cameras support
* Palette Notes
