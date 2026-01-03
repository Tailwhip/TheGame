// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <stdexcept>
// #include "Logging/StructuredLog.h" // LOGMTF(...)
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTheGame, All, All);

#if _MSC_VER
    #define FUNC_NAME    TEXT(__FUNCTION__)
#else // FIXME - GCC?
    #define FUNC_NAME    TEXT(__func__)
#endif
    
#define TRACE(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_CYAN );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTheGame, Log, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define TRACESTATIC(Format, ...) \
{ \
    SET_WARN_COLOR(COLOR_CYAN);\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTheGame, Log, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define TRACEWARN(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_YELLOW );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTheGame, Log, TEXT("**WARNING** %s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define TRACEERROR(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_RED );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTheGame, Log, TEXT("**ERROR** %s() : %s"), FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define SCREENDEBUG(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::BLUE, Msg); \
}
