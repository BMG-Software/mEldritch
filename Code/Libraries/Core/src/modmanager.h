#ifndef MODMANAGER_H
#define MODMANAGER_H

struct SModDescription
{
	SModDescription( SimpleString Name, SimpleString Path )
	:	m_Name( Name )
	,	m_Version( "" )
	,	m_WebsiteURL( "" )
	,	m_DownloadURL( "" )
	,	m_Description( "" )
	,	m_Image( "" )
	,	m_Author( "" )
	,	m_InitialLevel( "" )
	,	m_Path( Path )
	,	m_Space( Path )
	{}

	SModDescription()
	{
		SModDescription( "", "" );
	}

	SimpleString m_Name;
	SimpleString m_Version;
	SimpleString m_WebsiteURL;
	SimpleString m_DownloadURL;
	SimpleString m_Description;
	SimpleString m_Image;
	SimpleString m_Author;
	SimpleString m_InitialLevel;

	SimpleString m_Path;
	HashedString m_Space;
};

class ModManager
{
public:
	static ModManager*		GetInstance();
	static void				DeleteInstance();

	bool InitializeMods( const char* ModCfgName );
	bool AddMod( const char* ModPath );

	// load one config file:
	//	- first try the packed config file from the packstream
	//	- then the unpacked config file rom the packstream
	//	- then the packed config file from thje directory
	//	- then the unpacked config file from the directory.
	bool LoadConfigFile( const char* ConfigFile );

	// open a packstream:
	// iterate backwards through m_Mods until we find the file. We load
	// packed after unpacked. If the file is not found in a mod, try the
	// default dir
	PackStream OpenPackStream( const char* FileName );

	static PackStream OpenStream( const char* FileName )
	{
		return GetInstance()->OpenPackStream( FileName );
	}

	const Array< SModDescription >& GetMods() const { return m_Mods; }

	const SModDescription* FindMod( const SimpleString& ModName ) const;

private:
	ModManager(); // load mods.cfg
	~ModManager();

	static ModManager* m_Instance;

	Array< SModDescription > m_Mods;
};

#endif /* MODMANAGER_H */
