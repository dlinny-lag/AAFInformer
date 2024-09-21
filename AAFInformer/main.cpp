#include <shlobj.h>
#include "Shlwapi.h"
#include "f4se/PluginAPI.h"
#include "f4se_common/f4se_version.h"
#include "f4se/PapyrusVM.h"
#include "f4se/GameForms.h"
#include "PluginAPIExport.hpp"
#include "FurnitureCache.h"

#if F4SE_PRE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_163
#endif
#if _F4SE_DECLARATIVE_LOAD
	#define REQUIRED_RUNTIME RUNTIME_VERSION_1_10_984
#endif

#ifndef REQUIRED_RUNTIME
	#error Invalid project configuration
#endif

F4SEPapyrusInterface* g_papyrus = nullptr;
F4SEMessagingInterface* g_messaging = nullptr;
PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

bool RegisterExportingFunctions(VirtualMachine* vm)
{
	PluginAPIExport::Register(vm);
	return true;

}

void MessageHandler(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == F4SEMessagingInterface::kMessage_PreLoadGame ||
		msg->type == F4SEMessagingInterface::kMessage_GameLoaded)
	{
		FurnitureCache::ReBuildCache();
	}
}

void InitLogs()
{
	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Fallout4\\F4SE\\AAFInformer.log");
}


extern "C"
{
#if F4SE_PRE_DECLARATIVE_LOAD
	__declspec(dllexport) bool F4SEPlugin_Query(const F4SEInterface * f4se, PluginInfo * info)
	{
		InitLogs();

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PluginAPIExport::pluginName;
		info->version = PluginAPIExport::pluginVersionInt;
		_MESSAGE("F4SEPlugin_Query successful.");

		return true;
	}
#endif

#if _F4SE_DECLARATIVE_LOAD
	__declspec(dllexport) F4SEPluginVersionData F4SEPlugin_Version =
	{
		F4SEPluginVersionData::kVersion,
		
		PluginAPIExport::pluginVersionInt,
		"AAF Informer",
		"Dlinny_Lag",

		F4SEPluginVersionData::kAddressIndependence_AddressLibrary_1_10_980,
		F4SEPluginVersionData::kStructureIndependence_1_10_980Layout,
		{ REQUIRED_RUNTIME, 0 },
		0,
		0,
		0,
		{0}
	};

#endif

	__declspec(dllexport) bool F4SEPlugin_Load(const F4SEInterface * f4se)
	{
#if _F4SE_DECLARATIVE_LOAD
		// logs was not initialized at F4SEPlugin_Query
		InitLogs();
#endif
		g_pluginHandle = f4se->GetPluginHandle();
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
		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging)
		{
			_ERROR("Failed to get F4SEMessagingInterface");
			return false;
		}

		const bool retVal = Proc::PositionsHolder::StartXmlParsing();
		if (!retVal)
		{
			_ERROR("Failed to initialize XML data collecting");
			return true; // do not block game startup
		}

		g_messaging->RegisterListener(g_pluginHandle, "F4SE", MessageHandler);
		g_papyrus->Register(RegisterExportingFunctions);

		return true;
	}

};
