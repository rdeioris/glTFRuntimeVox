// Copyright 2023, Roberto De Ioris.


#include "glTFRuntimeVoxFunctionLibrary.h"
#if ENGINE_MAJOR_VERSION >= 5
#include "ColorSpace.h"
#else
#define FUintVector FUintVector4
#endif

struct FglTFRuntimeVoxNode
{
	FString Name;
	uint32 LayerId = 0;
	TMap<uint32, FTransform> Transforms;
	TArray<uint32> Children;
	TMap<uint32, uint32> Models;
	TMap<FString, FString> Attributes;
	bool bHidden = false;
};

struct FglTFRuntimeVoxCacheData : FglTFRuntimePluginCacheData
{
	int32 Version = 0;
	TArray<FUintVector> Sizes;
	TArray<TArray<uint32>> Models;
	TArray<uint32> Palette;
	TMap<uint32, FglTFRuntimeVoxNode> Nodes;
};

namespace glTFRuntimeVox
{
	const uint32 DefaultPalette[256] =
	{
		0x00000000, 0xffffffff, 0xffccffff, 0xff99ffff, 0xff66ffff, 0xff33ffff, 0xff00ffff, 0xffffccff, 0xffccccff, 0xff99ccff, 0xff66ccff, 0xff33ccff, 0xff00ccff, 0xffff99ff, 0xffcc99ff, 0xff9999ff,
		0xff6699ff, 0xff3399ff, 0xff0099ff, 0xffff66ff, 0xffcc66ff, 0xff9966ff, 0xff6666ff, 0xff3366ff, 0xff0066ff, 0xffff33ff, 0xffcc33ff, 0xff9933ff, 0xff6633ff, 0xff3333ff, 0xff0033ff, 0xffff00ff,
		0xffcc00ff, 0xff9900ff, 0xff6600ff, 0xff3300ff, 0xff0000ff, 0xffffffcc, 0xffccffcc, 0xff99ffcc, 0xff66ffcc, 0xff33ffcc, 0xff00ffcc, 0xffffcccc, 0xffcccccc, 0xff99cccc, 0xff66cccc, 0xff33cccc,
		0xff00cccc, 0xffff99cc, 0xffcc99cc, 0xff9999cc, 0xff6699cc, 0xff3399cc, 0xff0099cc, 0xffff66cc, 0xffcc66cc, 0xff9966cc, 0xff6666cc, 0xff3366cc, 0xff0066cc, 0xffff33cc, 0xffcc33cc, 0xff9933cc,
		0xff6633cc, 0xff3333cc, 0xff0033cc, 0xffff00cc, 0xffcc00cc, 0xff9900cc, 0xff6600cc, 0xff3300cc, 0xff0000cc, 0xffffff99, 0xffccff99, 0xff99ff99, 0xff66ff99, 0xff33ff99, 0xff00ff99, 0xffffcc99,
		0xffcccc99, 0xff99cc99, 0xff66cc99, 0xff33cc99, 0xff00cc99, 0xffff9999, 0xffcc9999, 0xff999999, 0xff669999, 0xff339999, 0xff009999, 0xffff6699, 0xffcc6699, 0xff996699, 0xff666699, 0xff336699,
		0xff006699, 0xffff3399, 0xffcc3399, 0xff993399, 0xff663399, 0xff333399, 0xff003399, 0xffff0099, 0xffcc0099, 0xff990099, 0xff660099, 0xff330099, 0xff000099, 0xffffff66, 0xffccff66, 0xff99ff66,
		0xff66ff66, 0xff33ff66, 0xff00ff66, 0xffffcc66, 0xffcccc66, 0xff99cc66, 0xff66cc66, 0xff33cc66, 0xff00cc66, 0xffff9966, 0xffcc9966, 0xff999966, 0xff669966, 0xff339966, 0xff009966, 0xffff6666,
		0xffcc6666, 0xff996666, 0xff666666, 0xff336666, 0xff006666, 0xffff3366, 0xffcc3366, 0xff993366, 0xff663366, 0xff333366, 0xff003366, 0xffff0066, 0xffcc0066, 0xff990066, 0xff660066, 0xff330066,
		0xff000066, 0xffffff33, 0xffccff33, 0xff99ff33, 0xff66ff33, 0xff33ff33, 0xff00ff33, 0xffffcc33, 0xffcccc33, 0xff99cc33, 0xff66cc33, 0xff33cc33, 0xff00cc33, 0xffff9933, 0xffcc9933, 0xff999933,
		0xff669933, 0xff339933, 0xff009933, 0xffff6633, 0xffcc6633, 0xff996633, 0xff666633, 0xff336633, 0xff006633, 0xffff3333, 0xffcc3333, 0xff993333, 0xff663333, 0xff333333, 0xff003333, 0xffff0033,
		0xffcc0033, 0xff990033, 0xff660033, 0xff330033, 0xff000033, 0xffffff00, 0xffccff00, 0xff99ff00, 0xff66ff00, 0xff33ff00, 0xff00ff00, 0xffffcc00, 0xffcccc00, 0xff99cc00, 0xff66cc00, 0xff33cc00,
		0xff00cc00, 0xffff9900, 0xffcc9900, 0xff999900, 0xff669900, 0xff339900, 0xff009900, 0xffff6600, 0xffcc6600, 0xff996600, 0xff666600, 0xff336600, 0xff006600, 0xffff3300, 0xffcc3300, 0xff993300,
		0xff663300, 0xff333300, 0xff003300, 0xffff0000, 0xffcc0000, 0xff990000, 0xff660000, 0xff330000, 0xff0000ee, 0xff0000dd, 0xff0000bb, 0xff0000aa, 0xff000088, 0xff000077, 0xff000055, 0xff000044,
		0xff000022, 0xff000011, 0xff00ee00, 0xff00dd00, 0xff00bb00, 0xff00aa00, 0xff008800, 0xff007700, 0xff005500, 0xff004400, 0xff002200, 0xff001100, 0xffee0000, 0xffdd0000, 0xffbb0000, 0xffaa0000,
		0xff880000, 0xff770000, 0xff550000, 0xff440000, 0xff220000, 0xff110000, 0xffeeeeee, 0xffdddddd, 0xffbbbbbb, 0xffaaaaaa, 0xff888888, 0xff777777, 0xff555555, 0xff444444, 0xff222222, 0xff111111
	};

	FVector4 GetColor(const uint32 ColorIndex, const uint32* Palette, const ETextureColorSpace ColorSpaceType, const float GammaCorrection)
	{
		const uint32 Color = Palette[ColorIndex];
		const uint8 R = Color & 0xFF;
		const uint8 G = (Color >> 8) & 0xFF;
		const uint8 B = (Color >> 16) & 0xFF;
		const uint8 A = (Color >> 24) & 0xFF;

		FVector4 LinearColor;

		LinearColor.X = static_cast<float>(R) / 255.0;
		LinearColor.Y = static_cast<float>(G) / 255.0;
		LinearColor.Z = static_cast<float>(B) / 255.0;
		LinearColor.W = static_cast<float>(A) / 255.0;

#if ENGINE_MAJOR_VERSION >= 5
		UE::Color::FColorSpace ColorSpace = UE::Color::FColorSpace(static_cast<UE::Color::EColorSpace>(ColorSpaceType));
		LinearColor *= ColorSpace.GetRgbToXYZ().TransformFVector4(LinearColor);
#endif

		LinearColor.X = FMath::Pow(LinearColor.X, GammaCorrection);
		LinearColor.Y = FMath::Pow(LinearColor.Y, GammaCorrection);
		LinearColor.Z = FMath::Pow(LinearColor.Z, GammaCorrection);

		return LinearColor;
	}

	bool IsVoxelSurrounded(const TArray<uint32>& Voxels, const uint32 X, const uint32 Y, const uint32 Z, const FUintVector Size)
	{
		// exclude borders
		if (X == 0 || X >= Size.X - 1 || Y == 0 || Y >= Size.Y - 1 || Z == 0 || Z >= Size.Z - 1)
		{
			return false;
		}

		auto Exists = [Voxels](const uint32 XYZ) -> bool
			{
				for (const uint32 Voxel : Voxels)
				{
					if ((Voxel & 0x00FFFFFF) == XYZ)
					{
						return true;
					}
				}

				return false;
			};

		const uint32 Top = ((Z + 1) << 16) | (Y << 8) | X;
		if (Exists(Top))
		{
			const uint32 Bottom = ((Z - 1) << 16) | (Y << 8) | X;
			if (Exists(Bottom))
			{
				const uint32 Right = (Z << 16) | (Y << 8) | (X + 1);
				if (Exists(Right))
				{
					const uint32 Left = (Z << 16) | (Y << 8) | (X - 1);
					if (Exists(Left))
					{
						const uint32 Front = (Z << 16) | ((Y - 1) << 8) | X;
						if (Exists(Front))
						{
							const uint32 Back = (Z << 16) | ((Y + 1) << 8) | X;
							if (Exists(Back))
							{
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}

	void AddVoxelInstance(const uint32 X, const uint32 Y, const uint32 Z, const uint32 Color, const TArray<uint32>& Palette, const FglTFRuntimeVoxConfig& VoxConfig, const FUintVector Size, FTransform& Transform, FLinearColor& LinearColor)
	{
		const float Scale = VoxConfig.VoxelSize;
		float XScaled = Y * Scale - (Size.Y * 0.5 * Scale) + (Y * VoxConfig.Padding);
		float YScaled = X * Scale - (Size.X * 0.5 * Scale) + (X * VoxConfig.Padding);
		float ZScaled = Z * Scale - (Size.Z * 0.5 * Scale) + (Z * VoxConfig.Padding);

		Transform.SetLocation(FVector(XScaled, YScaled, ZScaled));

		LinearColor = FLinearColor(GetColor(Color, Palette.GetData(), VoxConfig.ColorSpace, VoxConfig.GammaCorrection));
	}

	void AddVoxel(FglTFRuntimePrimitive& Primitive, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Color, const TArray<uint32>& Palette, const FglTFRuntimeVoxConfig& VoxConfig, const FUintVector Size)
	{
		const float Scale = VoxConfig.VoxelSize;
		float XScaled = Y * Scale - (Size.Y * 0.5 * Scale) + (Y * VoxConfig.Padding);
		float YScaled = X * Scale - (Size.X * 0.5 * Scale) + (X * VoxConfig.Padding);
		float ZScaled = Z * Scale - (Size.Z * 0.5 * Scale) + (Z * VoxConfig.Padding);

		const int32 Base = Primitive.Positions.Num();

		/* front face */
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled + Scale));

		/* back face */
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled + Scale));

		/* top face */
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled + Scale));

		/* bottom face */
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled));

		/* right face */
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled + Scale, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled, YScaled + Scale, ZScaled + Scale));

		/* left face */
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled));
		Primitive.Positions.Add(FVector(XScaled, YScaled, ZScaled + Scale));
		Primitive.Positions.Add(FVector(XScaled + Scale, YScaled, ZScaled + Scale));

		/* front triangles */
		Primitive.Indices.Add(Base + 0);
		Primitive.Indices.Add(Base + 1);
		Primitive.Indices.Add(Base + 2);

		Primitive.Indices.Add(Base + 2);
		Primitive.Indices.Add(Base + 3);
		Primitive.Indices.Add(Base + 0);

		/* back triangles */
		Primitive.Indices.Add(Base + 4);
		Primitive.Indices.Add(Base + 5);
		Primitive.Indices.Add(Base + 6);

		Primitive.Indices.Add(Base + 6);
		Primitive.Indices.Add(Base + 7);
		Primitive.Indices.Add(Base + 4);

		/* top triangles */
		Primitive.Indices.Add(Base + 8);
		Primitive.Indices.Add(Base + 9);
		Primitive.Indices.Add(Base + 10);

		Primitive.Indices.Add(Base + 10);
		Primitive.Indices.Add(Base + 11);
		Primitive.Indices.Add(Base + 8);

		/* bottom triangles */
		Primitive.Indices.Add(Base + 12);
		Primitive.Indices.Add(Base + 13);
		Primitive.Indices.Add(Base + 14);

		Primitive.Indices.Add(Base + 14);
		Primitive.Indices.Add(Base + 15);
		Primitive.Indices.Add(Base + 12);

		/* right triangles */
		Primitive.Indices.Add(Base + 16);
		Primitive.Indices.Add(Base + 17);
		Primitive.Indices.Add(Base + 18);

		Primitive.Indices.Add(Base + 18);
		Primitive.Indices.Add(Base + 19);
		Primitive.Indices.Add(Base + 16);

		/* left triangles */
		Primitive.Indices.Add(Base + 20);
		Primitive.Indices.Add(Base + 21);
		Primitive.Indices.Add(Base + 22);

		Primitive.Indices.Add(Base + 22);
		Primitive.Indices.Add(Base + 23);
		Primitive.Indices.Add(Base + 20);

		const FVector4 LinearColor = GetColor(Color, Palette.GetData(), VoxConfig.ColorSpace, VoxConfig.GammaCorrection);
		for (int32 Index = 0; Index < 24; Index++)
		{
			Primitive.Colors.Add(LinearColor);
		}
	}

	void ParseChunk_SIZE(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 12)
		{
			return;
		}

		FUintVector Size;
		Size.X = *(reinterpret_cast<const uint32*>(Data));
		Size.Y = *(reinterpret_cast<const uint32*>(Data + 4));
		Size.Z = *(reinterpret_cast<const uint32*>(Data + 8));

		RuntimeVoxCacheData->Sizes.Add(Size);
	}

	void ParseChunk_XYZI(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 4)
		{
			return;
		}

		const uint32 NumVoxels = *(reinterpret_cast<const uint32*>(Data));

		if (DataLen < 4 + (sizeof(uint32) * NumVoxels))
		{
			return;
		}

		TArray<uint32> Voxels;
		Voxels.AddUninitialized(NumVoxels);

		for (uint32 VoxelIndex = 0; VoxelIndex < NumVoxels; VoxelIndex++)
		{
			Voxels[VoxelIndex] = *(reinterpret_cast<const uint32*>(Data + 4 + (sizeof(uint32) * VoxelIndex)));
		}

		RuntimeVoxCacheData->Models.Add(MoveTemp(Voxels));
	}

	void ParseChunk_RGBA(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 256 * sizeof(uint32))
		{
			return;
		}

		FMemory::Memcpy(RuntimeVoxCacheData->Palette.GetData() + 1, Data, 255 * sizeof(uint32));
	}

	FString GetString(const uint8* Data, int64& Offset, const uint32 DataLen)
	{
		if (Offset + 4 > DataLen)
		{
			Offset = -1;
			return "";
		}

		const uint32 KeyLen = *(reinterpret_cast<const uint32*>(Data + Offset));
		Offset += 4;
		TArray<uint8> KeyBytes;
		KeyBytes.AddUninitialized(KeyLen);
		KeyBytes.Add(0);

		if (Offset + KeyLen > DataLen)
		{
			Offset = -1;
			return "";
		}

		FMemory::Memcpy(KeyBytes.GetData(), Data + Offset, KeyLen);

		const FString Key = UTF8_TO_TCHAR(KeyBytes.GetData());

		Offset += KeyLen;

		return Key;
	}

	TMap<FString, FString> GetDict(const uint8* Data, int64& Offset, const uint32 DataLen)
	{
		TMap<FString, FString> Dict;

		if (Offset + 4 > DataLen)
		{
			Offset = -1;
			return Dict;
		}

		const uint32 KeyValueNum = *(reinterpret_cast<const uint32*>(Data + Offset));
		Offset += 4;

		for (uint32 KeyValueIndex = 0; KeyValueIndex < KeyValueNum; KeyValueIndex++)
		{
			const FString Key = GetString(Data, Offset, DataLen);

			if (Offset < 0)
			{
				return Dict;
			}

			const FString Value = GetString(Data, Offset, DataLen);

			if (Offset < 0)
			{
				return Dict;
			}

			Dict.Add(Key, Value);
		}

		return Dict;
	}

	void ParseChunk_nTRN(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 4)
		{
			return;
		}

		const uint32 NodeId = *(reinterpret_cast<const uint32*>(Data));
		int64 Offset = 4;

		FglTFRuntimeVoxNode Node;

		Node.Attributes = GetDict(Data, Offset, DataLen);
		if (Offset < 0)
		{
			return;
		}

		if (Node.Attributes.Contains("_name"))
		{
			Node.Name = Node.Attributes["_name"];
		}
		else
		{
			Node.Name = FString::Printf(TEXT("nTRN_%u"), NodeId);
		}

		if (Offset + 16 <= DataLen)
		{
			const uint32 ChildNodeId = *(reinterpret_cast<const uint32*>(Data + Offset));
			Node.Children.Add(ChildNodeId);

			Node.LayerId = *(reinterpret_cast<const uint32*>(Data + Offset + 8));

			const uint32 FramesNum = *(reinterpret_cast<const uint32*>(Data + Offset + 12));
			Offset += 16;

			for (uint32 FrameIndex = 0; FrameIndex < FramesNum; FrameIndex++)
			{

				uint32 FrameId = 0;
				FTransform Transform = FTransform::Identity;

				TMap<FString, FString> FrameDict = GetDict(Data, Offset, DataLen);

				if (Offset < 0)
				{
					return;
				}

				if (FrameDict.Contains("_r"))
				{
					const uint8 R = FCString::Atoi(*(FrameDict["_r"]));
					const uint32 FirstRowIndex = R & 0x03;
					const uint32 SecondRowIndex = (R >> 2) & 0x03;
					if (FirstRowIndex < 3 && SecondRowIndex < 3)
					{
						int32 ThirdRowIndex = 0;
						switch (FirstRowIndex + SecondRowIndex)
						{
						case 1:
							ThirdRowIndex = 2;
							break;
						case 2:
							ThirdRowIndex = 1;
							break;
						case 3:
							ThirdRowIndex = 0;
						default:
							break;
						}
						const int32 FirstRowSign = (R >> 4) & 0x01;
						const int32 SecondRowSign = (R >> 5) & 0x01;
						const int32 ThirdRowSign = (R >> 6) & 0x01;

						FBasisVectorMatrix BasisMatrix(FVector(0, 1, 0), FVector(1, 0, 0), FVector(0, 0, 1), FVector::ZeroVector);

						FMatrix RotationMatrix;
						RotationMatrix.M[0][0] = 0; RotationMatrix.M[1][0] = 0; RotationMatrix.M[2][0] = 0; RotationMatrix.M[3][0] = 0;
						RotationMatrix.M[0][1] = 0; RotationMatrix.M[1][1] = 0; RotationMatrix.M[2][1] = 0; RotationMatrix.M[3][1] = 0;
						RotationMatrix.M[0][2] = 0; RotationMatrix.M[1][2] = 0; RotationMatrix.M[2][2] = 0; RotationMatrix.M[3][2] = 0;
						RotationMatrix.M[0][3] = 0; RotationMatrix.M[1][3] = 0; RotationMatrix.M[2][3] = 0; RotationMatrix.M[3][3] = 1;
						RotationMatrix.M[FirstRowIndex][0] = 1 * (FirstRowSign ? -1 : 1);
						RotationMatrix.M[SecondRowIndex][1] = 1 * (SecondRowSign ? -1 : 1);
						RotationMatrix.M[ThirdRowIndex][2] = 1 * (ThirdRowSign ? -1 : 1);

						Transform = FTransform(BasisMatrix * RotationMatrix * BasisMatrix.Inverse());
					}
				}

				if (FrameDict.Contains("_t"))
				{
					FVector Location;
					TArray<FString> LocationString;
					if (FrameDict["_t"].ParseIntoArray(LocationString, TEXT(" ")) == 3)
					{
						Location.X = FCString::Atoi(*(LocationString[1]));
						Location.Y = FCString::Atoi(*(LocationString[0]));
						Location.Z = FCString::Atoi(*(LocationString[2]));

						Transform.SetLocation(Location);
					}
				}

				if (FrameDict.Contains("_f"))
				{
					FrameId = FCString::Atoi(*(FrameDict["_f"]));
				}

				Node.Transforms.Add(FrameId, Transform);
			}

		}

		RuntimeVoxCacheData->Nodes.Add(NodeId, Node);
	}

	void ParseChunk_nSHP(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 4)
		{
			return;
		}

		const uint32 NodeId = *(reinterpret_cast<const uint32*>(Data));

		FglTFRuntimeVoxNode Node;

		int64 Offset = 4;
		Node.Attributes = GetDict(Data, Offset, DataLen);

		if (Offset < 0)
		{
			return;
		}

		if (Node.Attributes.Contains("_name"))
		{
			Node.Name = Node.Attributes["_name"];
		}
		else
		{
			Node.Name = FString::Printf(TEXT("nSHP_%u"), NodeId);
		}

		if (Offset + 4 <= DataLen)
		{
			const uint32 NumModels = *(reinterpret_cast<const uint32*>(Data + Offset));
			Offset += 4;

			for (uint32 ModelIndex = 0; ModelIndex < NumModels; ModelIndex++)
			{
				if (Offset + 4 > DataLen)
				{
					return;
				}

				const uint32 ModelId = *(reinterpret_cast<const uint32*>(Data + Offset));
				Offset += 4;

				uint32 FrameId = 0;

				TMap<FString, FString> ModelDict = GetDict(Data, Offset, DataLen);
				if (Offset < 0)
				{
					return;
				}

				if (ModelDict.Contains("_f"))
				{
					FrameId = FCString::Atoi(*(ModelDict["_f"]));
				}

				Node.Models.Add(FrameId, ModelId);
			}
		}

		RuntimeVoxCacheData->Nodes.Add(NodeId, Node);
	}

	void ParseChunk_nGRP(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const uint32 DataLen)
	{
		if (DataLen < 4)
		{
			return;
		}

		const uint32 NodeId = *(reinterpret_cast<const uint32*>(Data));

		FglTFRuntimeVoxNode Node;

		int64 Offset = 4;
		Node.Attributes = GetDict(Data, Offset, DataLen);

		if (Offset < 0)
		{
			return;
		}

		if (Node.Attributes.Contains("_name"))
		{
			Node.Name = Node.Attributes["_name"];
		}
		else
		{
			Node.Name = FString::Printf(TEXT("nGRP_%u"), NodeId);
		}

		if (Offset + 4 <= DataLen)
		{
			const uint32 NumChildren = *(reinterpret_cast<const uint32*>(Data + Offset));
			Offset += 4;

			for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ChildIndex++)
			{
				if (Offset + 4 > DataLen)
				{
					return;
				}

				const uint32 ChildId = *(reinterpret_cast<const uint32*>(Data + Offset));
				Offset += 4;

				Node.Children.Add(ChildId);
			}
		}

		RuntimeVoxCacheData->Nodes.Add(NodeId, Node);
	}

	int64 ParseChunk(TSharedRef<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData, const uint8* Data, const int64 DataLen)
	{
		constexpr int32 IDSize = 4;
		constexpr int32 HeaderSize = 12;

		if (DataLen < HeaderSize)
		{
			return -1;
		}

		uint32 ChunkSize = *(reinterpret_cast<const uint32*>(Data + 4));
		uint32 ChildChunkSize = *(reinterpret_cast<const uint32*>(Data + 8));


		if (!FMemory::Memcmp(Data, "SIZE", IDSize))
		{
			ParseChunk_SIZE(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}
		else if (!FMemory::Memcmp(Data, "XYZI", IDSize))
		{
			ParseChunk_XYZI(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}
		else if (!FMemory::Memcmp(Data, "RGBA", IDSize))
		{
			ParseChunk_RGBA(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}
		else if (!FMemory::Memcmp(Data, "nTRN", IDSize))
		{
			ParseChunk_nTRN(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}
		else if (!FMemory::Memcmp(Data, "nSHP", IDSize))
		{
			ParseChunk_nSHP(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}
		else if (!FMemory::Memcmp(Data, "nGRP", IDSize))
		{
			ParseChunk_nGRP(RuntimeVoxCacheData, Data + HeaderSize, ChunkSize);
		}

		int64 Offset = 0;

		while (Offset < ChildChunkSize)
		{
			const int64 NewChunkSize = ParseChunk(RuntimeVoxCacheData, Data + HeaderSize + ChunkSize + Offset, ChildChunkSize - Offset);
			if (NewChunkSize < 0)
			{
				return -1;
			}
			Offset += NewChunkSize;
		}

		return HeaderSize + ChunkSize;
	}

	TSharedPtr<FglTFRuntimeVoxCacheData> GetCacheData(UglTFRuntimeAsset* Asset)
	{
		if (Asset->GetParser()->PluginsCacheData.Contains("Vox"))
		{
			if (Asset->GetParser()->PluginsCacheData["Vox"] && Asset->GetParser()->PluginsCacheData["Vox"]->bValid)
			{
				return StaticCastSharedPtr<FglTFRuntimeVoxCacheData>(Asset->GetParser()->PluginsCacheData["Vox"]);
			}
		}

		if (!Asset->GetParser()->PluginsCacheData.Contains("Vox"))
		{
			Asset->GetParser()->PluginsCacheData.Add("Vox", MakeShared<FglTFRuntimeVoxCacheData>());
		}

		TArray64<uint8> ArchiveBlob;

		const TArray64<uint8>* Blob = nullptr;

		if (Asset->IsArchive())
		{
			for (const FString& Name : Asset->GetArchiveItems())
			{
				if (Name.EndsWith(".vox"))
				{
					if (!Asset->GetParser()->GetBlobByName(Name, ArchiveBlob))
					{
						return nullptr;
					}

					Blob = &ArchiveBlob;
					break;
				}
			}
		}
		else
		{
			Blob = &(Asset->GetParser()->GetBlob());
		}

		if (!Blob)
		{
			return nullptr;
		}

		const int64 BlobSize = Blob->Num();

		if (BlobSize < 8)
		{
			return nullptr;
		}

		if ((*Blob)[0] != 'V' || (*Blob)[1] != 'O' || (*Blob)[2] != 'X' || (*Blob)[3] != ' ')
		{
			return nullptr;
		}

		TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = StaticCastSharedPtr<FglTFRuntimeVoxCacheData>(Asset->GetParser()->PluginsCacheData["Vox"]);

		RuntimeVoxCacheData->Version = *(reinterpret_cast<const int32*>(&((*Blob)[4])));

		RuntimeVoxCacheData->Palette.Append(DefaultPalette, 256);

		if (ParseChunk(RuntimeVoxCacheData.ToSharedRef(), Blob->GetData() + 8, BlobSize - 8) < 0)
		{
			return nullptr;
		}

		RuntimeVoxCacheData->bValid = true;

		return RuntimeVoxCacheData;
	}
}

bool UglTFRuntimeVoxFunctionLibrary::LoadVoxModelAsInstances(UglTFRuntimeAsset* Asset, const int32 ModelIndex, TArray<FTransform>& Transforms, TArray<FLinearColor>& Colors, const FglTFRuntimeVoxConfig& VoxConfig)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return false;
	}

	if (!RuntimeVoxCacheData->Models.IsValidIndex(ModelIndex))
	{
		return false;
	}

	if (!RuntimeVoxCacheData->Sizes.IsValidIndex(ModelIndex))
	{
		return false;
	}

	Transforms.Empty(RuntimeVoxCacheData->Models[ModelIndex].Num());
	Colors.Empty(RuntimeVoxCacheData->Models[ModelIndex].Num());

	for (const uint32 Voxel : RuntimeVoxCacheData->Models[ModelIndex])
	{
		uint8 Color = Voxel >> 24;
		uint8 Z = (Voxel >> 16) & 0xFF;
		uint8 Y = (Voxel >> 8) & 0xFF;
		uint8 X = Voxel & 0xFF;
		if (VoxConfig.bRemoveHiddenVoxels)
		{
			if (glTFRuntimeVox::IsVoxelSurrounded(RuntimeVoxCacheData->Models[ModelIndex], X, Y, Z, RuntimeVoxCacheData->Sizes[ModelIndex]))
			{
				continue;
			}
		}
		FTransform Transform = FTransform::Identity;
		FLinearColor LinearColor = FLinearColor::White;

		glTFRuntimeVox::AddVoxelInstance(X, Y, Z, Color, RuntimeVoxCacheData->Palette, VoxConfig, RuntimeVoxCacheData->Sizes[ModelIndex], Transform, LinearColor);

		Transforms.Add(Transform);
		Colors.Add(LinearColor);
	}

	return true;
}

UVolumeTexture* UglTFRuntimeVoxFunctionLibrary::LoadVoxModelAsVolumeTexture(UglTFRuntimeAsset* Asset, const int32 ModelIndex, const FglTFRuntimeVoxConfig& VoxConfig, const FglTFRuntimeImagesConfig& ImagesConfig, const FglTFRuntimeTextureSampler& Sampler)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return nullptr;
	}

	if (!RuntimeVoxCacheData->Models.IsValidIndex(ModelIndex))
	{
		return nullptr;
	}

	if (!RuntimeVoxCacheData->Sizes.IsValidIndex(ModelIndex))
	{
		return nullptr;
	}

	TArray<FglTFRuntimeMipMap> Mips;

	TArray64<uint8> Pixels;
	Pixels.AddZeroed(RuntimeVoxCacheData->Sizes[ModelIndex].X * RuntimeVoxCacheData->Sizes[ModelIndex].Z * RuntimeVoxCacheData->Sizes[ModelIndex].Y * 4);

	for (const uint32 Voxel : RuntimeVoxCacheData->Models[ModelIndex])
	{
		uint8 Color = Voxel >> 24;
		uint8 Z = (Voxel >> 16) & 0xFF;
		uint8 Y = (Voxel >> 8) & 0xFF;
		uint8 X = Voxel & 0xFF;
		if (VoxConfig.bRemoveHiddenVoxels)
		{
			if (glTFRuntimeVox::IsVoxelSurrounded(RuntimeVoxCacheData->Models[ModelIndex], X, Y, Z, RuntimeVoxCacheData->Sizes[ModelIndex]))
			{
				continue;
			}
		}

		const int64 Offset = (Y * RuntimeVoxCacheData->Sizes[ModelIndex].X * RuntimeVoxCacheData->Sizes[ModelIndex].Z * 4) + (((RuntimeVoxCacheData->Sizes[ModelIndex].Z-1) - Z) * RuntimeVoxCacheData->Sizes[ModelIndex].X * 4) + (X * 4);
		const FVector4 LinearColor = glTFRuntimeVox::GetColor(Color, RuntimeVoxCacheData->Palette.GetData(), VoxConfig.ColorSpace, VoxConfig.GammaCorrection);
		Pixels[Offset] = LinearColor.Z * 255;
		Pixels[Offset + 1] = LinearColor.Y * 255;
		Pixels[Offset + 2] = LinearColor.X * 255;
		Pixels[Offset + 3] = LinearColor.W * 255;
	}

	FglTFRuntimeMipMap Mip(-1, EPixelFormat::PF_B8G8R8A8, RuntimeVoxCacheData->Sizes[ModelIndex].X, RuntimeVoxCacheData->Sizes[ModelIndex].Z, Pixels);

	Mips.Add(Mip);

	return Asset->GetParser()->BuildVolumeTexture(GetTransientPackage(), Mips, RuntimeVoxCacheData->Sizes[ModelIndex].Y, ImagesConfig, Sampler);
}

bool UglTFRuntimeVoxFunctionLibrary::LoadVoxModelAsRuntimeLOD(UglTFRuntimeAsset* Asset, const int32 ModelIndex, FglTFRuntimeMeshLOD& RuntimeLOD, const FglTFRuntimeVoxConfig& VoxConfig, const FglTFRuntimeMaterialsConfig& MaterialsConfig)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return false;
	}

	if (!RuntimeVoxCacheData->Models.IsValidIndex(ModelIndex))
	{
		return false;
	}

	if (!RuntimeVoxCacheData->Sizes.IsValidIndex(ModelIndex))
	{
		return false;
	}

	FglTFRuntimePrimitive Primitive;
	FglTFRuntimeMaterial RuntimeMaterial;

	Primitive.Material = Asset->GetParser()->BuildMaterial(-1, FString::Printf(TEXT("ModelMaterial_%d"), ModelIndex), RuntimeMaterial, MaterialsConfig, true);

	for (const uint32 Voxel : RuntimeVoxCacheData->Models[ModelIndex])
	{
		uint8 Color = Voxel >> 24;
		uint8 Z = (Voxel >> 16) & 0xFF;
		uint8 Y = (Voxel >> 8) & 0xFF;
		uint8 X = Voxel & 0xFF;
		if (VoxConfig.bRemoveHiddenVoxels)
		{
			if (glTFRuntimeVox::IsVoxelSurrounded(RuntimeVoxCacheData->Models[ModelIndex], X, Y, Z, RuntimeVoxCacheData->Sizes[ModelIndex]))
			{
				continue;
			}
		}
		glTFRuntimeVox::AddVoxel(Primitive, X, Y, Z, Color, RuntimeVoxCacheData->Palette, VoxConfig, RuntimeVoxCacheData->Sizes[ModelIndex]);
	}

	RuntimeLOD.bHasVertexColors = true;
	RuntimeLOD.Primitives.Add(MoveTemp(Primitive));
	return true;
}

FIntVector UglTFRuntimeVoxFunctionLibrary::GetVoxModelSize(UglTFRuntimeAsset* Asset, const int32 ModelIndex)
{
	FIntVector Size = FIntVector::ZeroValue;

	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return Size;
	}

	if (!RuntimeVoxCacheData->Sizes.IsValidIndex(ModelIndex))
	{
		return Size;
	}

	Size.X = RuntimeVoxCacheData->Sizes[ModelIndex].X;
	Size.Y = RuntimeVoxCacheData->Sizes[ModelIndex].Y;
	Size.Z = RuntimeVoxCacheData->Sizes[ModelIndex].Z;
	return Size;
}

int32 UglTFRuntimeVoxFunctionLibrary::GetNumVoxModels(UglTFRuntimeAsset* Asset)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return 0;
	}

	return RuntimeVoxCacheData->Sizes.Num();
}

int32 UglTFRuntimeVoxFunctionLibrary::GetVoxVersion(UglTFRuntimeAsset* Asset)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return 0;
	}

	return RuntimeVoxCacheData->Version;
}

int32 UglTFRuntimeVoxFunctionLibrary::GetNumVoxNodes(UglTFRuntimeAsset* Asset)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return 0;
	}

	return RuntimeVoxCacheData->Nodes.Num();
}

TArray<int32> UglTFRuntimeVoxFunctionLibrary::GetVoxNodesId(UglTFRuntimeAsset* Asset)
{
	TArray<int32> Ids;
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return Ids;
	}

	for (const TPair<uint32, FglTFRuntimeVoxNode>& Node : RuntimeVoxCacheData->Nodes)
	{
		Ids.Add(Node.Key);
	}

	return Ids;
}

TArray<int32> UglTFRuntimeVoxFunctionLibrary::GetVoxRootNodesId(UglTFRuntimeAsset* Asset)
{
	TArray<int32> Ids;
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return Ids;
	}

	for (const TPair<uint32, FglTFRuntimeVoxNode>& Node : RuntimeVoxCacheData->Nodes)
	{
		bool bRoot = true;

		for (const TPair<uint32, FglTFRuntimeVoxNode>& OtherNode : RuntimeVoxCacheData->Nodes)
		{
			if (OtherNode.Value.Children.Contains(Node.Key))
			{
				bRoot = false;
				break;
			}
		}

		if (bRoot)
		{
			Ids.Add(Node.Key);
		}
	}

	return Ids;
}

TArray<int32> UglTFRuntimeVoxFunctionLibrary::GetVoxNodeChildrenId(UglTFRuntimeAsset* Asset, const int32 NodeId)
{
	TArray<int32> Ids;
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return Ids;
	}

	if (RuntimeVoxCacheData->Nodes.Contains(NodeId))
	{
		for (const uint32 ChildId : RuntimeVoxCacheData->Nodes[NodeId].Children)
		{
			Ids.Add(ChildId);
		}
	}

	return Ids;
}

int32 UglTFRuntimeVoxFunctionLibrary::GetVoxNodeModelIndex(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return INDEX_NONE;
	}

	if (RuntimeVoxCacheData->Nodes.Contains(NodeId))
	{
		if (RuntimeVoxCacheData->Nodes[NodeId].Models.Contains(FrameIndex))
		{
			return RuntimeVoxCacheData->Nodes[NodeId].Models[FrameIndex];
		}
	}

	return INDEX_NONE;
}

FString UglTFRuntimeVoxFunctionLibrary::GetVoxNodeName(UglTFRuntimeAsset* Asset, const int32 NodeId)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return "";
	}

	if (RuntimeVoxCacheData->Nodes.Contains(NodeId))
	{
		return RuntimeVoxCacheData->Nodes[NodeId].Name;
	}

	return "";
}

FTransform UglTFRuntimeVoxFunctionLibrary::GetVoxNodeTransform(UglTFRuntimeAsset* Asset, const int32 NodeId, const int32 FrameIndex)
{
	TSharedPtr<FglTFRuntimeVoxCacheData> RuntimeVoxCacheData = glTFRuntimeVox::GetCacheData(Asset);
	if (!RuntimeVoxCacheData)
	{
		return FTransform::Identity;
	}

	if (RuntimeVoxCacheData->Nodes.Contains(NodeId))
	{
		if (RuntimeVoxCacheData->Nodes[NodeId].Transforms.Contains(FrameIndex))
		{
			return RuntimeVoxCacheData->Nodes[NodeId].Transforms[FrameIndex];
		}
	}

	return FTransform::Identity;
}