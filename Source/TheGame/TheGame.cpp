// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheGame.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogTheGame);

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, TheGame, "TheGame" );


//
//
// std::string to_hex_str(uint8 hex_val)
// {
// 	if (hex_val == 0)
// 		return "0x0";
// 	else if (hex_val < 0)
// 		hex_val = -hex_val;
//
// 	std::stringstream ss;
// 	ss << "0x" << std::hex << hex_val;
// 	return ss.str();
// }