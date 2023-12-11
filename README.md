# glTFRuntimeVox
MagicaVoxel loader for glTFRuntime
![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/94ac53f9-32c6-49e2-b9cd-2df35ea57592)

## Quickstart

The ```AglTFRuntimeVoxAssetActor``` Class is available for quickly loading VOX scenes:
![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/566de179-4b67-449f-b07c-7c257629a7ee)

Ensure the set the 'as blob' flag in the loader config (even when loading from urls!)

The VoxConfig structure allows you to define the voxel size as well as distance between voxels (padding), colorspace and gamma correction:

![image](https://github.com/rdeioris/glTFRuntimeVox/assets/2234592/7c3b6082-2816-42eb-8a58-1d4eb8212525)

The "Remove Hidden Voxels" option is an optimization avoiding "internal" voxel to be generated (internal voxels are those one fully surrounded by other volxels)

