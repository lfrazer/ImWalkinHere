#pragma once


class Settings
{
public:
	using bSetting = Json2Settings::bSetting;


	Settings() = delete;

	static bool LoadSettings(bool a_dumpParse = false);


	static inline bSetting disableDialogueCollision{ "disableDialogueCollision", true };
	static inline bSetting disableAllyCollision{ "disableAllyCollision", true };
	static inline bSetting disableSummonCollision{ "disableSummonCollision", true };
	static inline bSetting disableAllySummonCollision{ "disableAllySummonCollision", true };

private:
	static constexpr std::string_view FILE_NAME = "Data\\SKSE\\Plugins\\ImWalkinHere.json";
};
