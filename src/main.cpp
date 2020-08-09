﻿#include "Events.h"
#include "Hooks.h"
#include "Settings.h"
#include "version.h"

#ifdef SKYRIMVR
#include "SKSE/Interfaces.h"
#include "SKSE/API.h"
#endif

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::Install();
		break;
	}
}


extern "C" DLLEXPORT bool APIENTRY SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	SKSE::Logger::OpenRelative(FOLDERID_Documents, L"\\My Games\\Skyrim Special Edition\\SKSE\\ImWalkinHere.log");
	SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
	SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
	SKSE::Logger::UseLogStamp(true);
	SKSE::Logger::TrackTrampolineStats(true);

	_MESSAGE("ImWalkinHere v%s", WLKN_VERSION_VERSTRING);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "ImWalkinHere";
	a_info->version = WLKN_VERSION_MAJOR;

	if (a_skse->IsEditor()) {
		_FATALERROR("Loaded in editor, marking as incompatible!");
		return false;
	}

	auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		_FATALERROR("Unsupported runtime version %s!", ver.GetString().c_str());
		return false;
	}

	return true;
}


extern "C" DLLEXPORT bool APIENTRY SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	_MESSAGE("ImWalkinHere loaded");

	if (!SKSE::Init(a_skse)) {
		return false;
	}

	if (!Settings::LoadSettings()) {
		_FATALERROR("Failed to load settings!");
		return false;
	}

	if (!SKSE::AllocTrampoline(1 << 4)) {
		return false;
	}

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Hooks::Install();

	return true;
}
