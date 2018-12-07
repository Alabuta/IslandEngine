/********************************************************************************************************************************
****
****    Source code of Island Engine.
****    Copyright (C) 2009 - 2017 Crusoe's Island LLC.
****
****    Description: main header file - the beginning and the application end.
****
********************************************************************************************************************************/
#pragma once

#ifndef CRUS_ENGINE_H               // Include guard "engine.h"
#define CRUS_ENGINE_H

/**
@mainpage Island Engine
*/
#include <map>
#include <random>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <future>
#include <array>

#include <vector>
#include <numeric>

#include <iostream>

#include "System\CrusSystem.h"
#include "System\CrusWindow.h"


#include "Renderer\CrusRender.h"
#include "Renderer\CrusProgram.h"
#include "Renderer\CrusTexture.h"

#include "Interface\CrusTextOut.h"

#include "Game\CrusObject.h"

#include "Interface\CrusGrid.h"

#include "Game\CrusBounds.h"
#include "Game\CrusRect.h"
#include "Game\CrusSprite.h"
#include "Renderer\CrusUV.h"

#include "Manager\CrusMeshLoader.h"

#endif // CRUS_ENGINE_H