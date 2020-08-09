#pragma once

#ifdef SKYRIMVR
#include "ForceInclude.h"
#endif

#include "RE/Skyrim.h"
#include "REL/Relocation.h"

#ifndef SKYRIMVR
#include "SKSE/SKSE.h"
#endif

#include "Json2Settings.h"

#include <atomic>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

#define DLLEXPORT __declspec(dllexport)

template <
	class T,
	class = std::enable_if_t<
		std::is_pointer_v<T>>>
using not_null = T;
