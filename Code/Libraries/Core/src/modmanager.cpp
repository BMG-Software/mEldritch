#include "core.h"
#include "customassert.h"
#include "allocator.h"
#include "array.h"
#include "simplestring.h"
#include "packstream.h"
#include "modmanager.h"
#include "hashedstring.h"
#include "file.h"
#include "configmanager.h"
#include "filestream.h"
#include "frameworkutil.h"
#include "printmanager.h"

ModManager* ModManager::m_Instance = NULL;

ModManager::ModManager()
:	m_Mods()
{
}

ModManager::~ModManager()
{
	m_Mods.Clear();
}

ModManager* ModManager::GetInstance()
{
	if ( !m_Instance )
	{
		m_Instance = new ModManager();
	}
	return m_Instance;
}

void ModManager::DeleteInstance()
{
	if ( m_Instance )
	{
		delete( m_Instance );
	}
}

bool ModManager::InitializeMods( const char* ModCfgName )
{
	if( FileUtil::Exists( ModCfgName ) )
	{
		const FileStream ModCfgStream = FileStream( ModCfgName, FileStream::EFM_Read );
		ConfigManager::Load( ModCfgStream );

		SimpleString Prefix = FileUtil::StripFilename( ModCfgName ) + "/";

		STATICHASH( mEldritchMods );
		STATICHASH( NumMods );
		uint NumMods = ConfigManager::GetInt( sNumMods, 0, smEldritchMods );

		for( uint ModIndex = 0; ModIndex < NumMods; ++ModIndex )
		{
			const char* ModName = ConfigManager::GetSequenceString( "Mods%d", ModIndex, NULL, smEldritchMods );
			if ( ModName )
			{
				AddMod( (Prefix + ModName + "/").CStr() );
			}
		}
		return true;
	}
	return false;
}

bool ModManager::AddMod( const char* ModPath )
{
	if ( !FileUtil::IsDir( ModPath ) )
	{
		PRINTF( "Could not add Mod from path %s: Path does not exist\n", ModPath );
		return false;
	}
	SimpleString ModConfig = SimpleString( ModPath ) + "mod.config";
	if ( ! FileUtil::Exists( ModConfig.CStr() ) )
	{
		PRINTF( "Could not add Mod from path %s: mod.config does not exist\n", ModPath );
		return false;
	}
	ConfigManager::PushEnvironment();
	ConfigManager::Load( FileStream( ModConfig.CStr(), FileStream::EFM_Read ) );
	PackStream::StaticAddPackageSpace( ModPath );

	STATICHASH( ModName );
	const char* ModName = ConfigManager::GetString( sModName, NULL, HashedString::NullString );
	ConfigManager::RemoveContext( "" );

	if ( !ModName )
	{
		PRINTF( "Could not add Mod from path %s: Mod name does not exist\n", ModPath );
		return false;
	}

	SimpleString ModContext = SimpleString( "Mod." ) + ModName;
	MAKEHASH( ModContext );

	const char* ModVersion = ConfigManager::GetString( "Version", NULL, sModContext );

	if ( !ModName )
	{
		PRINTF( "Could not add Mod from path %s: Mod version does not exist\n", ModPath );
		return false;
	}

	STATICHASH( NumPackages );
	uint NumPackages = ConfigManager::GetInt( sNumPackages, 0, sModContext );
	for ( uint i = 0; i < NumPackages; ++i )
	{
		SimpleString PkgPath = SimpleString( ModPath ) + ConfigManager::GetSequenceString( "Package%d", i, NULL, sModContext );
		PackStream::StaticAddPackageFile( PkgPath.CStr() );
	}

	STATICHASH( NumConfigFiles );
	uint NumConfigFiles = ConfigManager::GetInt( sNumConfigFiles, 0, sModContext );
	for ( uint i = 0; i < NumConfigFiles; ++i )
	{
		LoadConfigFile( ConfigManager::GetSequenceString( "ConfigFile%d", i, NULL, sModContext ) );
	}

	// clean up mod loader stuff
	PackStream::StaticMakeCurrentPackageSpace();
	ConfigManager::MergeEnvironment();

	SModDescription& TheMod = m_Mods.PushBack( SModDescription( ModName, ModPath ) );

	TheMod.m_Name = ModName;
	TheMod.m_Version = ModVersion;
	TheMod.m_WebsiteURL = ConfigManager::GetString( "WebsiteURL", "", sModContext );
	TheMod.m_DownloadURL = ConfigManager::GetString( "DownloadURL", "", sModContext );
	TheMod.m_Description = ConfigManager::GetString( "Description", "", sModContext );
	TheMod.m_Image = ConfigManager::GetString( "Image", "", sModContext );
	TheMod.m_Author = ConfigManager::GetString( "Author", "", sModContext );
	TheMod.m_InitialLevel = ConfigManager::GetString( "InitialLevel", "", sModContext );

	PRINTF( "Added Mod \"%s\" from path %s\n", ModName, ModPath );
	PRINTF( ".Version:      %s\n", TheMod.m_Version != "" ? TheMod.m_Version.CStr() : "-" );
	PRINTF( ".WebsiteURL:   %s\n", TheMod.m_WebsiteURL != "" ? TheMod.m_WebsiteURL.CStr() : "-" );
	PRINTF( ".DownloadURL:  %s\n", TheMod.m_DownloadURL != "" ? TheMod.m_DownloadURL.CStr() : "-" );
	PRINTF( ".Description:  %s\n", TheMod.m_Description != "" ? TheMod.m_Description.CStr() : "-" );
	PRINTF( ".Image:        %s\n", TheMod.m_Image != "" ? TheMod.m_Image.CStr() : "-" );
	PRINTF( ".Author:       %s\n", TheMod.m_Author != "" ? TheMod.m_Author.CStr() : "-" );
	PRINTF( ".InitialLevel: %s\n", TheMod.m_InitialLevel != "" ? TheMod.m_InitialLevel.CStr() : "-" );

	return true;
}

bool ModManager::LoadConfigFile( const char* ConfigFile )
{
	const char* Prefix = PackStream::StaticGetCurrentPrefix();

	bool Loaded = false;
	// find name of uncompiled config file
	SimpleString FileName = ConfigFile;
	int Where = FileName.Find( ".ccf" );
	if ( Where >= 0 and FileName.Length() - Where == 4 )
	{
		FileName = FileName.Replace( Where, ".config" );
	}
	else
	{ 	// special case for syncer config file...
		Where = FileName.Find( ".pcf" );
		if ( Where >= 0 and FileName.Length() - Where == 4 )
		{
			FileName = FileName.Replace( Where, ".syncerconfig" );
			if ( FileUtil::Exists( FileName.CStr() ) )
			{
				ConfigManager::Load( FileStream( FileName.CStr(), FileStream::EFM_Read ) );
			}
		}
		else
		{
			// either ConfigFile already is the uncompiled version, or something we don't understand.
			FileName = "";
		}
	}

	// load compiled config file from packstream
	if ( PackStream::StaticFileExists( ConfigFile, true ) )
	{
		ConfigManager::Load( PackStream( ConfigFile, PackStream::PSO_PackedOnly ) );
		PackStream::StaticMakeCurrentPackageSpace( Prefix );
		Loaded = true;
	}
	// load uncompiled config file from packstream
	if ( ( FileName != "" ) && PackStream::StaticFileExists( FileName.CStr(), true ) )
	{
		ConfigManager::Load( PackStream( FileName.CStr(), PackStream::PSO_PackedOnly ) );
		PackStream::StaticMakeCurrentPackageSpace( Prefix );
		Loaded = true;
	}

	SimpleString RealConfigFile = SimpleString( Prefix ) + ConfigFile;
	SimpleString RealFileName = SimpleString( Prefix ) + FileName;

	// try compiled config file from disc
	if ( FileUtil::Exists( RealConfigFile.CStr() ) )
	{
		ConfigManager::Load( FileStream( RealConfigFile.CStr(), FileStream::EFM_Read ) );
		PackStream::StaticMakeCurrentPackageSpace( Prefix );
		Loaded = true;
	}
	// also try uncompiled config file
	if ( ( FileName != "" ) && FileUtil::Exists( RealFileName.CStr() ) )
	{
		ConfigManager::Load( FileStream( RealFileName.CStr(), FileStream::EFM_Read ) );
		PackStream::StaticMakeCurrentPackageSpace( Prefix );
		Loaded = true;
	}
	return Loaded;
}

PackStream ModManager::OpenPackStream( const char* FileName )
{
	for ( int i = m_Mods.Size() - 1; i >= 0; --i )
	{
		SModDescription& TheMod = m_Mods[ i ];
		if ( PackStream::StaticMakeCurrentPackageSpace( TheMod.m_Space ) )
		{
			if ( PackStream::StaticFileExists( FileName ) )
			{
				return PackStream( FileName, PackStream::PSO_PackedLast );
				// TODO PackStream::StaticMakeCurrentPackageSpace();
			}
		}
	}
	// if the file was not found in a mod, open it from eldritch
	PackStream::StaticMakeCurrentPackageSpace();
	return PackStream( FileName, PackStream::PSO_PackedLast );
}

const SModDescription* ModManager::FindMod( const SimpleString& ModName ) const
{
	for ( uint ModIter = 0; ModIter < m_Mods.Size(); ++ModIter )
	{
		if( m_Mods[ModIter].m_Name == ModName )
		{
			return &m_Mods[ModIter];
		}
	}
	return NULL;
}
