// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef ADVGAMESPROGRAMMING_PlayerCharacter_generated_h
#error "PlayerCharacter.generated.h already included, missing '#pragma once' in PlayerCharacter.h"
#endif
#define ADVGAMESPROGRAMMING_PlayerCharacter_generated_h

#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_SPARSE_DATA
#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_RPC_WRAPPERS
#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_RPC_WRAPPERS_NO_PURE_DECLS
#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesAPlayerCharacter(); \
	friend struct Z_Construct_UClass_APlayerCharacter_Statics; \
public: \
	DECLARE_CLASS(APlayerCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/AdvGamesProgramming"), NO_API) \
	DECLARE_SERIALIZER(APlayerCharacter)


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_INCLASS \
private: \
	static void StaticRegisterNativesAPlayerCharacter(); \
	friend struct Z_Construct_UClass_APlayerCharacter_Statics; \
public: \
	DECLARE_CLASS(APlayerCharacter, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/AdvGamesProgramming"), NO_API) \
	DECLARE_SERIALIZER(APlayerCharacter)


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API APlayerCharacter(const FObjectInitializer& ObjectInitializer); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(APlayerCharacter) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APlayerCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APlayerCharacter); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APlayerCharacter(APlayerCharacter&&); \
	NO_API APlayerCharacter(const APlayerCharacter&); \
public:


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API APlayerCharacter(APlayerCharacter&&); \
	NO_API APlayerCharacter(const APlayerCharacter&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, APlayerCharacter); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(APlayerCharacter); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(APlayerCharacter)


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	FORCEINLINE static uint32 __PPO__LookSensitivity() { return STRUCT_OFFSET(APlayerCharacter, LookSensitivity); } \
	FORCEINLINE static uint32 __PPO__SprintMultiplier() { return STRUCT_OFFSET(APlayerCharacter, SprintMultiplier); }


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_10_PROLOG
#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_SPARSE_DATA \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_RPC_WRAPPERS \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_INCLASS \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_PRIVATE_PROPERTY_OFFSET \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_SPARSE_DATA \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_INCLASS_NO_PURE_DECLS \
	Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> ADVGAMESPROGRAMMING_API UClass* StaticClass<class APlayerCharacter>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID Week5StartingPoint_Source_AdvGamesProgramming_PlayerCharacter_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
