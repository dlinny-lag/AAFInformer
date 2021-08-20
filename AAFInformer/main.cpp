#include <shlobj.h>
#include "Shlwapi.h"
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"
#include "f4se/GameForms.h"
#include "PluginAPIExport.hpp"

#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163

F4SEPapyrusInterface* g_papyrus = nullptr;

bool RegisterExportingFunctions(VirtualMachine* vm)
{
	PluginAPIExport::Register(vm);
	return true;
}

extern "C"
{
	bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\AAFInformer.log");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PluginAPIExport::pluginName;
		info->version = PluginAPIExport::pluginVersionInt;

		if(f4se->runtimeVersion < REQUIRED_RUNTIME)
		{
			_ERROR("Unsupported runtime version %08X (expected %08X or higher)", f4se->runtimeVersion, REQUIRED_RUNTIME);
			return false;
		}

		g_papyrus = (F4SEPapyrusInterface *)f4se->QueryInterface(kInterface_Papyrus);
		if(!g_papyrus)
		{
			_ERROR("Failed to get F4SEPapyrusInterface");
			return false;
		}

		return Proc::PositionsHolder::StartXmlParsing();
	}

	bool F4SEPlugin_Load(const F4SEInterface * f4se)
	{
		g_papyrus->Register(RegisterExportingFunctions);
		return true;
	}

};
