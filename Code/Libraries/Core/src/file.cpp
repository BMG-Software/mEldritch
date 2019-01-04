#include "core.h"
#include "file.h"

#include <stdio.h>
#include <string.h>

#if BUILD_WINDOWS
#include <direct.h>	// For directory functions
#include <shlobj.h>
#include <windows.h>
#endif

#if BUILD_LINUX || BUILD_MAC
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <glob.h>
#include <pwd.h>
#endif

#if BUILD_MAC
#include "objcjunk.h"
#define GLOB_ONLYDIR 0
#endif

#if BUILD_WINDOWS

// everybody supports Utf8 filenames - except for windows.

static bool CPToUtf8( const char* InCP, char* OutUtf8, uint OutSize )
{
	DWORD InLen = strlen( InCP );
	wchar_t *UniBuf = new wchar_t[ InLen + 1 ];
	uint OK = MultiByteToWideChar( CP_ACP, 0, InCP, InLen + 1, UniBuf, InLen + 1 );
	if ( OK )
	{
		OK = WideCharToMultiByte( CP_UTF8, 0, UniBuf, OK, OutUtf8, OutSize, NULL, NULL );
	}
	delete[] UniBuf;
	return OK != 0;
}

static bool Utf8ToCP( const char* InUtf8, char* OutCP, uint OutSize )
{
	DWORD InLen = strlen( InUtf8 );
	wchar_t *UniBuf = new wchar_t[ InLen + 1 ];
	uint OK = MultiByteToWideChar( CP_UTF8, 0, InUtf8, InLen + 1, UniBuf, InLen + 1 );
	if ( OK )
	{
		OK = WideCharToMultiByte( CP_ACP, 0, UniBuf, OK, OutCP, OutSize, NULL, NULL );
	}
	delete[] UniBuf;
	return OK != 0;
}

#endif

// This unit is a mess, a mix of standard and Windows-only functions, many with special-case
// purposes and some that are unsafe to use in-game and are only ever meant to run in tools.

// Cross-platform verified.
// Match upper and lower case letters and slashes
static bool Match( char c1, char c2 )
{
	if( c1 == c2 )
	{
		return true;
	}
	if( ( c1 == '\\' && c2 == '/' ) ||
		( c1 == '/' && c2 == '\\' ) )
	{
		return true;
	}
	if( ( c1 >= 'A' && c1 <= 'Z' ) && ( c1 == ( c2 - 32 ) ) )
	{
		return true;
	}
	if( ( c1 >= 'a' && c1 <= 'z' ) && ( c1 == ( c2 + 32 ) ) )
	{
		return true;
	}
	return false;
}

// Cross-platform verified.
bool FileUtil::Exists( const char* Filename )
{
	FILE* pFile = NULL;
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Filename ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	if ( Utf8ToCP( Filename, CPBuf, CPBufLen ) )
	{
		FOPEN( pFile, CPBuf, "rb" );
	}
	else
	{
		FOPEN( pFile, Filename, "rb" );
	}
	delete[] CPBuf;
#else
	FOPEN( pFile, Filename, "rb" );
#endif
	if( pFile )
	{
		fclose( pFile );
		return true;
	}
	return false;
}

// Cross-platform verified.
bool FileUtil::PathExists( const char* Path )
{
	SimpleString CurrentDir = GetWorkingDirectory();
	if( !ChangeWorkingDirectory( Path ) )
	{
		return false;
	}
	else
	{
		ChangeWorkingDirectory( CurrentDir.CStr() );
		return true;
	}
}

bool FileUtil::IsFile( const char* Path )
{
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	DWORD FileAttributes = 0;
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		FileAttributes = GetFileAttributes( CPBuf );
	}
	else
	{
		FileAttributes = GetFileAttributes( Path );
	}
	delete[] CPBuf;

	if ( FileAttributes != 0xFFFFFFFF )
	{
		// isn't there an easier way? Is this even correct?
		DWORD DontWantAttrs = FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_REPARSE_POINT | 0xFFFFC000;
		return ( FileAttributes & DontWantAttrs ) == 0;
	}
	return false;
#else
	struct stat FileInfo;
	if ( stat( Path, &FileInfo) == 0 )
	{
		return S_ISREG( FileInfo.st_mode );
	}
	return false;
#endif
}

bool FileUtil::IsDir( const char* Path )
{
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	DWORD FileAttributes = 0;
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		FileAttributes = GetFileAttributes( CPBuf );
	}
	else
	{
		FileAttributes = GetFileAttributes( Path );
	}
	delete[] CPBuf;

	if ( FileAttributes != 0xFFFFFFFF )
	{
		return (FileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	return false;
#else
	struct stat FileInfo;
	if ( stat( Path, &FileInfo) == 0 )
	{
		return S_ISDIR( FileInfo.st_mode );
	}
	return false;
#endif
}

bool FileUtil::IsAbsolutePath( const char* Path )
{
#if BUILD_WINDOWS
	bool ok = ( *Path >= 'A' ) && ( *Path <= 'Z' );
	ok = ok && ( Path[1] == ':' );
	ok = ok && ( ( Path[2] == '\\' ) || ( Path[2] == '/' ) || ( Path[2] == '\0' ) );
	return ok;
#else
	return *Path == '/';
#endif
}

bool IsToplevel( const char* Path )
{
#if BUILD_WINDOWS
	bool ok = ( *Path >= 'A' ) && ( *Path <= 'Z' );
	ok = ok && ( Path[1] == ':' );
	if ( Path[2] != '\0' )
	{
		ok = ok && ( ( Path[2] == '\\' ) || ( Path[2] == '/' ) || ( Path[3] == '\0' ) );
	}
	return ok;
#else
	return strcmp( Path, "/" ) == 0;
#endif
}

// Cross-platform verified.
uint FileUtil::Size( const char* Filename )
{
	FILE* pFile = NULL;
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Filename ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	if ( Utf8ToCP( Filename, CPBuf, CPBufLen ) )
	{
		FOPEN( pFile, CPBuf, "rb" );
	}
	else
	{
		FOPEN( pFile, Filename, "rb" );
	}
	delete[] CPBuf;
#else
	FOPEN( pFile, Filename, "rb" );
#endif
	if( pFile )
	{
		fseek( pFile, 0, SEEK_END );
		uint RetVal = ftell( pFile );
		fclose( pFile );
		return RetVal;
	}
	return 0;
}

// Cross-platform verified.
// Compare filenames, case-insensitive, / and \ equivalent
bool FileUtil::Compare( const char* Filename1, const char* Filename2 )
{
	const char* Iter1 = Filename1;
	const char* Iter2 = Filename2;
	ASSERT( Iter1 );
	ASSERT( Iter2 );
	while( *Iter1 && *Iter2 )
	{
		if( !Match( *Iter1, *Iter2 ) )
		{
			return false;
		}
		++Iter1;
		++Iter2;
	}
	return true;
}

SimpleString FileUtil::Normalize( const char* Path )
{
	ASSERT( Path );

	uint Length = (uint)strlen( Path ) + 1;
	char* NormalizedPath = new char[ Length ];
	char* c;

	strcpy_s( NormalizedPath, Length, Path );

	// replace all path separators
	for( c = NormalizedPath; *c; ++c )
	{
		if( *c == '\\' )
		{
			*c = '/';
		}
	}

	// remove /./
	char* From = NormalizedPath;
	for( c = NormalizedPath; *From; ++c )
	{
		if ( ( *c == '/' ) && ( *(c+1) == '.' ) && ( *(c+2) == '/' ) )
		{
			From += 2;
		}
		*c = *From++;
	}

	// normalize /path/blu/.. to /path
	char* To = NormalizedPath;
	for( c = NormalizedPath; *c; ++c )
	{
		if ( ( *c == '/' ) && ( *(c+1) == '.' ) && ( *(c+2) == '.' ) && ( *(c+3) != '.' ) )
		{
			To = c - 1;
			while ( *To != '/' && To > NormalizedPath )
			{
				--To;
			}
			c += 3;
		}
		*To++ = *c;
	}

	// remove trailing slashes
	*To = '\0';
	c = To - 1;
	while ( ( *c == '/' ) && ( c > NormalizedPath ) )
	{
		*c-- = '\0';
	}

	SimpleString RetVal( NormalizedPath );	// Causes an extra allocation, which bugs me, but I don't want to make SimpleString's char* accessible
	delete NormalizedPath;
	return RetVal;
}

// Cross-platform verified.
void FileUtil::MakePath( const char* Path )
{
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		_mkdir( CPBuf );
	}
	else
	{
		_mkdir( Path );
	}
	delete[] CPBuf;
#else
	mkdir( Path, 0777 );
#endif
}

// Cross-platform verified.
void FileUtil::RecursiveMakePath( const char* Path )
{
	SimpleString Filepath = Path;
	SimpleString SplitPath;
	SimpleString Remainder;

	int DescendDepth = 0;
	while( FileUtil::SplitLeadingFolder( Filepath.CStr(), SplitPath, Remainder ) )
	{
		if( !FileUtil::PathExists( SplitPath.CStr() ) )
		{
			FileUtil::MakePath( SplitPath.CStr() );
		}
		FileUtil::ChangeWorkingDirectory( SplitPath.CStr() );
		Filepath = Remainder;
		DescendDepth++;
	}

	while( DescendDepth > 0 )
	{
		FileUtil::ChangeWorkingDirectory( ".." );
		--DescendDepth;
	}
}

// Cross-platform verified.
void FileUtil::RemovePath( const char* Path )
{
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		_rmdir( CPBuf );
	}
	else
	{
		_rmdir( Path );
	}
	delete[] CPBuf;
#else
	rmdir( Path );
#endif
}

// Cross-platform verified.
SimpleString FileUtil::GetWorkingDirectory()
{
	char WorkingDir[ 256 ];
#if BUILD_WINDOWS
	char Utf8Buf[ sizeof( WorkingDir ) * 3 ];
	_getcwd( WorkingDir, 256 );
	if ( Utf8ToCP( WorkingDir, Utf8Buf, sizeof( Utf8Buf ) ) )
	{
		return SimpleString( Utf8Buf );
	}
	else
	{
		return SimpleString( WorkingDir );
	}
#else
	getcwd( WorkingDir, 256 );
	return SimpleString( WorkingDir );
#endif
}

// Cross-platform verified.
bool FileUtil::ChangeWorkingDirectory( const char* Path )
{
	ASSERT( Path );
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		int Ok = _chdir( CPBuf );
		delete[] CPBuf;
		return Ok == 0;
	}
	return ( _chdir( Path ) == 0 );
#else
	return ( chdir( Path ) == 0 );
#endif
}

// Mixing in some Windows calls with all the POSIX stuff in the
// rest of this. Dur. Based on http://www.codeguru.com/forum/showthread.php?t=239271
void FileUtil::RecursiveRemoveDirectory( const char* Path )
{
#if BUILD_WINDOWS
	HANDLE          FileHandle;
	SimpleString	FilePath;
	SimpleString	FilePattern = SimpleString( Path ) + "\\*.*";
	WIN32_FIND_DATA FileInformation;

	FileHandle = FindFirstFile( FilePattern.CStr(), &FileInformation );
	if( FileHandle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( FileInformation.cFileName[0] != '.' )	// Don't recurse up tree
			{
				FilePath = SimpleString( Path ) + "\\" + FileInformation.cFileName;

				if( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					// Delete subdirectory
					RecursiveRemoveDirectory( FilePath.CStr() );
				}
				else
				{
					// Set file attributes so we can delete
					if( SetFileAttributes( FilePath.CStr(), FILE_ATTRIBUTE_NORMAL ) )
					{
						DeleteFile( FilePath.CStr() );
					}
				}
			}
		}
		while( TRUE == FindNextFile( FileHandle, &FileInformation ) );

		FindClose( FileHandle );

		// Set directory attributes so we can delete
		if( SetFileAttributes( Path, FILE_ATTRIBUTE_NORMAL ) )
		{
			RemoveDirectory( Path );
		}
	}
#else
	// TODO PORT LATER (not currently used)
	Unused( Path );
	WARN;
#endif
}

// Cross-platform verified.
bool FileUtil::RemoveFile( const char* Path )
{
#if BUILD_WINDOWS
	uint CPBufLen = strlen( Path ) + 1;
	char *CPBuf = new char[ CPBufLen ];
	bool Result = false;
	if ( Utf8ToCP( Path, CPBuf, CPBufLen ) )
	{
		Result = FALSE != DeleteFile( CPBuf );
	}
	else
	{
		Result = FALSE != DeleteFile( Path );
	}
	delete[] CPBuf;
	return Result;
#else
	return 0 == unlink( Path );
#endif
}

bool FileUtil::Copy( const char* InPath, const char* OutPath, bool FailIfExists )
{
#if BUILD_WINDOWS
	// TODO UTF8ify later
	return FALSE != CopyFile( InPath, OutPath, FailIfExists );
#else
	// TODO PORT LATER (not currently used)
	Unused( InPath );
	Unused( OutPath );
	Unused( FailIfExists );
	WARN;
	return false;
#endif
}

bool FileUtil::Move( const char* InPath, const char* OutPath )
{
#if BUILD_WINDOWS
	// TODO UTF8ify later
	return FALSE != MoveFile( InPath, OutPath );
#else
	// TODO PORT LATER (not currently used)
	Unused( InPath );
	Unused( OutPath );
	WARN;
	return false;
#endif
}

// Hacks used to clean up session folders; non-recursive (only returns folders in immediate path)
void FileUtil::GetAllFiles( const char* Path, const char* Extension, Array< SimpleString >& OutFiles )
{
#if BUILD_WINDOWS
	HANDLE          FileHandle;
	char			Buffer[ 3 * MAX_PATH ];
	SimpleString	FilePattern = SimpleString( Path ) + "/*";
	if ( Extension )
	{
		FilePattern = FilePattern + "." + Extension;
	}
	WIN32_FIND_DATA FileInformation;

	char *Pattern = new char[ FilePattern.Length() + 1 ];
	Utf8ToCP( FilePattern.CStr(), Pattern, FilePattern.Length() + 1 );

	FileHandle = FindFirstFile( Pattern, &FileInformation );
	if( FileHandle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( !( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) )
			{
				CPToUtf8( FileInformation.cFileName, Buffer, sizeof( Buffer ) );
				if( Buffer[0] != '.' )	// Don't recurse up tree
				{
					if( 0 == ( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
					{
						OutFiles.PushBack( SimpleString( Buffer ) );
					}
				}
			}
		}
		while( TRUE == FindNextFile( FileHandle, &FileInformation ) );

		FindClose( FileHandle );
	}

	delete[] Pattern;
#else
	glob_t			GlobData;
	SimpleString	FilePath;
	SimpleString	FilePattern = FileUtil::Normalize( ( SimpleString( Path ) + "/*" ).CStr() );
	uint			StripOffset = strcmp( Path, "/" ) ? strlen( Path ) + 1 : 1;

	if ( Extension )
	{
		FilePattern = FilePattern + "." + Extension;
	}
	struct stat 	FileInformation;

	if ( glob( FilePattern.CStr(), 0, NULL, &GlobData ) == 0 )
	{
		for ( uint Count = 0; Count < GlobData.gl_pathc; ++Count )
		{
			FilePath = SimpleString( GlobData.gl_pathv[Count] );
			if ( ( stat( FilePath.CStr(), &FileInformation ) == 0 ) && S_ISREG( FileInformation.st_mode ) )
			{
				FilePath = SimpleString( FilePath.CStr() + StripOffset );
				OutFiles.PushBack( FilePath.CStr() );
			}
		}
		globfree( &GlobData );
	}
#endif
}

void FileUtil::GetAllFiles( const char* Path, Array< SimpleString >& OutFiles )
{
	GetAllFiles( Path, "*", OutFiles );
}

#if BUILD_WINDOWS
static void GetAllDrives(  Array< SimpleString >& OutFolders )
{
	DWORD DriveMask = GetLogicalDrives();
	for ( int Drive = 0; Drive < 32; ++Drive )
	{
		if ( DriveMask & ( 1 << Drive ) )
		{
			char DriveName[ 16 ];
			sprintf( DriveName, "%c:", 'A' + Drive );
			UINT DriveType = GetDriveType( DriveName );
			if ( DriveType == DRIVE_REMOVABLE || DriveType == DRIVE_FIXED || DriveType == DRIVE_REMOTE || DriveType == DRIVE_RAMDISK )
			{
				OutFolders.PushBack( DriveName );
			}
		}
	}
}
#endif

void FileUtil::GetAllFolders( const char* Path, Array< SimpleString >& OutFolders, bool AppendPath )
{
#if BUILD_WINDOWS
	if ( !strcmp( Path, "/" ) )
	{
		GetAllDrives( OutFolders );
		return;
	}

	HANDLE          FileHandle;
	char			Buffer[ 3 * MAX_PATH ];
	SimpleString	FilePath;
	SimpleString	FilePattern = SimpleString( Path ) + "/*.*";
	WIN32_FIND_DATA FileInformation;

	char *Pattern = new char[ FilePattern.Length() + 1 ];
	Utf8ToCP( FilePattern.CStr(), Pattern, FilePattern.Length() + 1 );

	FileHandle = FindFirstFile( Pattern, &FileInformation );
	if( FileHandle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( !( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) )
			{
				CPToUtf8( FileInformation.cFileName, Buffer, sizeof( Buffer ) );
				if( Buffer[0] != '.' )	// Don't recurse up tree
				{
					if( AppendPath )
					{
						FilePath = SimpleString( Path ) + "/" + Buffer;
					}
					else
					{
						FilePath = Buffer;
					}

					if( FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
					{
						FilePath = Normalize( FilePath.CStr() );
						OutFolders.PushBack( FilePath.CStr() );
					}
				}
			}
		}
		while( TRUE == FindNextFile( FileHandle, &FileInformation ) );

		FindClose( FileHandle );
	}

	delete[] Pattern;
#else
	glob_t			GlobData;
	SimpleString	FilePath;
	SimpleString	FilePattern = FileUtil::Normalize( ( SimpleString( Path ) + "/*" ).CStr() );
	struct stat 	FileInformation;
	uint			StripOffset = strcmp( Path, "/" ) ? strlen( Path ) + 1 : 1;

	if ( glob( FilePattern.CStr(), GLOB_ONLYDIR, NULL, &GlobData ) == 0 )
	{
		for ( uint Count = 0; Count < GlobData.gl_pathc; ++Count )
		{
			FilePath = SimpleString( GlobData.gl_pathv[Count] );
			if ( ( stat( FilePath.CStr(), &FileInformation ) == 0 ) && S_ISDIR( FileInformation.st_mode ) )
			{
				if ( !AppendPath )
				{
					// strip path pattern
					FilePath = SimpleString( FilePath.CStr() + StripOffset );
				}
				OutFolders.PushBack( FilePath.CStr() );
			}
		}
		globfree( &GlobData );
	}
#endif
}

// Cross-platform verified.
const char* FileUtil::StripLeadingUpFolders( const char* Path )
{
	ASSERT( Path );
	while( *Path )
	{
		if( Match( *Path, '/' ) && Match( *( Path + 1 ), '/' ) )
		{
			Path += 2;
		}
		else if( Match( *Path, '.' ) && Match( *( Path + 1 ), '.' ) && Match( *( Path + 2 ), '/' ) )
		{
			Path += 3;
		}
		else
		{
			return Path;
		}
	}
	return Path;
}

// Cross-platform verified.
const char* FileUtil::StripLeadingFolder( const char* Path, const char* Folder )
{
	ASSERT( Path );
	ASSERT( Folder );
	const char* OriginalPath = Path;
	while( *Path && *Folder )
	{
		if( Match( *Path, *Folder ) )
		{
			++Path;
			++Folder;
		}
		else
		{
			return OriginalPath;	// Folder wasn't matched, don't modify
		}
	}
	if( Match( *Path, '/' ) )
	{
		return Path + 1;
	}
	else
	{
		return OriginalPath;
	}
}

// Cross-platform verified.
const char* FileUtil::StripLeadingFolders( const char* Path )
{
	ASSERT( Path );

	const char* LastPathChar = NULL;
	while( *Path )
	{
		if( Match( *Path, '/' ) )
		{
			LastPathChar = Path;
		}
		++Path;
	}

	// LastPathChar is now at the position of the last path character
	if( LastPathChar )
	{
		return LastPathChar + 1;
	}
	else
	{
		return Path;	// No path characters, just return the original string
	}
}

const char* FileUtil::StripAllButExtension( const char* Path )
{
	ASSERT( Path );
	uint Index = (uint)strlen( Path );

	while ( Path[ Index ] != '.' && Index > 0 )
	{
		--Index;
	}

	if ( Index == 0 )
	{
		return &Path[ strlen( Path ) + 1 ];
	}
	return &Path[ Index + 1 ];
}

SimpleString FileUtil::StripFilename( const char* Path )
{
	ASSERT( Path );

	uint Length = (uint)strlen( Path ) + 1;
	char* PathCopy = new char[ Length ];
	strcpy_s( PathCopy, Length, Path );

	char* PathCopyIter = PathCopy;
	char* LastSlash = NULL;
	while( *PathCopyIter )
	{
		if( Match( *PathCopyIter, '/' ) )
		{
			LastSlash = PathCopyIter;
		}
		++PathCopyIter;
	}

	// LastSlash is now at the position of the last dot
	if( LastSlash )
	{
		*LastSlash = '\0';
	}

	return PathCopy;
}

// Cross-platform verified.
SimpleString FileUtil::StripExtension( const char* Path )
{
	ASSERT( Path );

	uint Length = (uint)strlen( Path ) + 1;
	char* PathCopy = new char[ Length ];
	strcpy_s( PathCopy, Length, Path );

	char* PathCopyIter = PathCopy;
	char* LastDot = NULL;
	while( *PathCopyIter )
	{
		if( Match( *PathCopyIter, '.' ) )
		{
			LastDot = PathCopyIter;
		}
		++PathCopyIter;
	}

	// LastDot is now at the position of the last dot
	if( LastDot )
	{
		*LastDot = '\0';
	}

	SimpleString RetVal( PathCopy );
	delete PathCopy;
	return RetVal;
}

// Cross-platform verified.
SimpleString FileUtil::StripExtensions( const char* Path )
{
	ASSERT( Path );

	uint Length = (uint)strlen( Path ) + 1;
	char* PathCopy = new char[ Length ];
	strcpy_s( PathCopy, Length, Path );

	char* PathCopyIter = PathCopy;
	char* FirstDot = NULL;
	while( *PathCopyIter && !FirstDot )
	{
		if( Match( *PathCopyIter, '.' ) )
		{
			FirstDot = PathCopyIter;
		}
		++PathCopyIter;
	}

	// FirstDot is now at the position of the first dot
	if( FirstDot )
	{
		*FirstDot = '\0';
	}

	SimpleString RetVal( PathCopy );
	delete PathCopy;
	return RetVal;
}

// Cross-platform verified.
// This probably isn't super robust, but will work fine for a basic path.
bool FileUtil::SplitLeadingFolder( const char* Path, SimpleString& OutFolder, SimpleString& OutRemainder )
{
	ASSERT( Path );

	SimpleString PathCopy( Path );
	char* FolderPath = PathCopy.MutableCStr();
	char* IterStr = PathCopy.MutableCStr();

	while( *IterStr )
	{
		if( Match( *IterStr, '/' ) )
		{
			// Split here
			*IterStr = '\0';
			OutFolder = FolderPath;
			OutRemainder = IterStr + 1;
			return true;
		}
		++IterStr;
	}

	return false;
}

// Cross-platform verified.
bool FileUtil::HasExtension( const char* Path, const char* Extension )
{
	uint Length = (uint)strlen( Path );
	const char* ExtDot = Path + Length;

	while ( ( *ExtDot != '.' ) && ( ExtDot > Path ) )
	{
		--ExtDot;
	}
	if ( *ExtDot == '.' )
	{
		++ExtDot;
		return ( strcmp( ExtDot, Extension ) == 0 );
	}
	return false;
}

// Cross-platform verified.
// TODO Should this also use unicode?
SimpleString FileUtil::GetUserPersonalPath()
{
#if BUILD_WINDOWS
	char Path[ MAX_PATH ];
	char Utf8Buf[ 3 * sizeof( Path ) ];
	if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, Path ) ) )
	{
		if ( Utf8ToCP( Path, Utf8Buf, sizeof( Utf8Buf ) ) )
		{
			return Normalize( Utf8Buf );
		}
		else
		{
			return Normalize( Path );
		}
	}
	return SimpleString( "." );
#elif BUILD_LINUX
	const char *Path = getenv("HOME");
	if ( !Path )
	{
		struct passwd *pw = getpwuid(getuid());
		if ( pw )
		{
			Path = pw->pw_dir;
		}
	}
	if ( Path )
	{
		return SimpleString( Path );
	}
	else
	{
		return SimpleString( "." );
	}
#else // BUILD_MAC
	return ObjCJunk::GetUserPersonalPath();
#endif
}

// Cross-platform verified.
// TODO Should this also use unicode?
SimpleString FileUtil::GetUserLocalAppDataPath()
{
#if BUILD_WINDOWS
	char Path[ MAX_PATH ];
	char Utf8Buf[ 3 * sizeof( Path ) ];
	if( SUCCEEDED( SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, Path ) ) )
	{
		if ( Utf8ToCP( Path, Utf8Buf, sizeof( Utf8Buf ) ) )
		{
			return Normalize( Utf8Buf );
		}
		else
		{
			return Normalize( Path );
		}
	}
	return SimpleString( "." );
#elif BUILD_LINUX
	return GetUserPersonalPath() + "/.local/share";
#else // BUILD_MAC
	return ObjCJunk::GetUserLocalAppDataPath();
#endif
}

// Cross-platform verified.
char CharToChar( char Char )	// PROBLEM, CODE REVIEWER? :D
{
	Char = Char & 0xf;
	if( Char < 10 )
	{
		return '0' + Char;
	}
	else
	{
		return 'A' + ( Char - 10 );
	}
}

// Cross-platform verified.
SimpleString FileUtil::SanitizeFilename( const SimpleString& Filename )
{
	Array<char> SanitizedFilename;
	Filename.FillArray( SanitizedFilename, true );
	for( uint CharIndex = 0; CharIndex < SanitizedFilename.Size(); ++CharIndex )
	{
		if( SanitizedFilename[ CharIndex ] == '\\' ||
			SanitizedFilename[ CharIndex ] == '/' ||
			SanitizedFilename[ CharIndex ] == ':' ||
			SanitizedFilename[ CharIndex ] == '*' ||
			SanitizedFilename[ CharIndex ] == '?' ||
			SanitizedFilename[ CharIndex ] == '"' ||
			SanitizedFilename[ CharIndex ] == '<' ||
			SanitizedFilename[ CharIndex ] == '>' ||
			SanitizedFilename[ CharIndex ] == '|' )
		{
			char Char = SanitizedFilename[ CharIndex ];

			SanitizedFilename.Remove( CharIndex );
			SanitizedFilename.Insert( CharToChar( Char & 0xf ), CharIndex );
			SanitizedFilename.Insert( CharToChar( Char >> 4 ), CharIndex );
			SanitizedFilename.Insert( '%', CharIndex );
		}
	}

	return SimpleString( SanitizedFilename );
}

bool TryShellExecute( const char* Filename, const char* Operation )
{
#if BUILD_WINDOWS
	// TODO Utf8ify later
	HINSTANCE Result = ShellExecute(
		NULL,			// hWnd
		Operation,		// Operation (runas requests elevated access)
		Filename,		// File
		NULL,			// Parameters (or arguments to program)
		NULL,			// Directory
		SW_SHOWNORMAL	// Show flags (this makes sense in the context of opening a file in some editor)
		);

	const INT ExecuteResult = PtrToInt( Result );
	if( ExecuteResult > 32 )
	{
		return true;
	}
	else
	{
		PRINTF( "ShellExecute failed with code %d\n", ExecuteResult );
		return false;
	}
#else
	// TODO PORT LATER (not currently used)
	Unused( Filename );
	Unused( Operation );
	WARN;
	return false;
#endif
}

bool FileUtil::Launch( const char* Filename )
{
#if BUILD_WINDOWS
	if( TryShellExecute( Filename, "runas" ) )
	{
		return true;
	}

	if( TryShellExecute( Filename, "open" ) )
	{
		return true;
	}

	return false;
#else
	// TODO PORT LATER (not currently used)
	Unused( Filename );
	WARN;
	return false;
#endif
}

#if BUILD_WINDOWS
// TODO PORT LATER (not currently used)
bool FileUtil::GetSaveFile( const HWND& hWnd, const SimpleString& Desc, const SimpleString& Ext, SimpleString& OutFileName )
{
	// HACK, because string functions don't like dealing with \0 as part of a string.
	const SimpleString Filter = SimpleString::PrintF( "%s (*.%s)0*.%s0All Files (*.*)0*.*0", Desc.CStr(), Ext.CStr(), Ext.CStr() );
	Filter.Replace( '0', '\0' );

	OPENFILENAME OpenFileName;
	const uint MaxFilenameSize = 256;
	char FilenameBuffer[ MaxFilenameSize ];
	memset( &OpenFileName, 0, sizeof( OpenFileName ) );
	FilenameBuffer[0] = '\0';
	OpenFileName.lStructSize = sizeof( OpenFileName );
	OpenFileName.hwndOwner = hWnd;
	OpenFileName.lpstrFilter = Filter.CStr();
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = FilenameBuffer;
	OpenFileName.nMaxFile = MaxFilenameSize;
	OpenFileName.lpstrInitialDir = ".";
	OpenFileName.Flags = OFN_ENABLESIZING | OFN_OVERWRITEPROMPT;
	OpenFileName.lpstrDefExt = Ext.CStr();

	const SimpleString WorkingDirectory = FileUtil::GetWorkingDirectory();
	const BOOL Success = GetSaveFileName( &OpenFileName );
	FileUtil::ChangeWorkingDirectory( WorkingDirectory.CStr() );

	if( Success )
	{
		OutFileName = OpenFileName.lpstrFile;
		return true;
	}
	else
	{
		const DWORD Error = CommDlgExtendedError();
		PRINTF( "Error getting save file name: %d\n", Error );	// See cderr.h for meaning.
		return false;
	}
}
#endif

#if BUILD_WINDOWS
// TODO PORT LATER (not currently used)
bool FileUtil::GetLoadFile( const HWND& hWnd, const SimpleString& Desc, const SimpleString& Ext, SimpleString& OutFileName )
{
	// HACK, because string functions don't like dealing with \0 as part of a string.
	const SimpleString Filter = SimpleString::PrintF( "%s (*.%s)0*.%s0All Files (*.*)0*.*0", Desc.CStr(), Ext.CStr(), Ext.CStr() );
	Filter.Replace( '0', '\0' );

	OPENFILENAME OpenFileName;
	const uint MaxFilenameSize = 256;
	char FilenameBuffer[ MaxFilenameSize ];
	memset( &OpenFileName, 0, sizeof( OpenFileName ) );
	FilenameBuffer[0] = '\0';
	OpenFileName.lStructSize = sizeof( OpenFileName );
	OpenFileName.hwndOwner = hWnd;
	OpenFileName.lpstrFilter = Filter.CStr();
	OpenFileName.nFilterIndex = 1;
	OpenFileName.lpstrFile = FilenameBuffer;
	OpenFileName.nMaxFile = MaxFilenameSize;
	OpenFileName.lpstrInitialDir = ".";
	OpenFileName.Flags = OFN_ENABLESIZING | OFN_FILEMUSTEXIST;
	OpenFileName.lpstrDefExt = Ext.CStr();

	const SimpleString WorkingDirectory = FileUtil::GetWorkingDirectory();
	const BOOL Success = GetOpenFileName( &OpenFileName );
	FileUtil::ChangeWorkingDirectory( WorkingDirectory.CStr() );

	if( Success )
	{
		OutFileName = OpenFileName.lpstrFile;
		return true;
	}
	else
	{
		const DWORD Error = CommDlgExtendedError();
		PRINTF( "Error getting load file name: %d\n", Error );	// See cderr.h for meaning.
		return false;
	}
}
#endif
