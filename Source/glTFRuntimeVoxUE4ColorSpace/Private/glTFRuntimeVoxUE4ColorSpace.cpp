// Copyright 2023, Roberto De Ioris.

#include "glTFRuntimeVoxUE4ColorSpace.h"

#define LOCTEXT_NAMESPACE "FglTFRuntimeVoxUE4ColorSpaceModule"

void FglTFRuntimeVoxUE4ColorSpaceModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FglTFRuntimeVoxUE4ColorSpaceModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FglTFRuntimeVoxUE4ColorSpaceModule, glTFRuntimeVoxUE4ColorSpace)