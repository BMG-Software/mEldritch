#include "core.h"
#include "objcjunk.h"
#include "simplestring.h"

#include <string.h>

#import <Corefoundation/CFString.h>
#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSFileManager.h>

static char UserLocalAppDataPath[ 4096 ] = "";
static char UserPersonalPath[ 4096 ] = "";

SimpleString ObjCJunk::GetUserLocalAppDataPath()
{
	if ( !*UserLocalAppDataPath )
	{
		NSArray* pPaths = NSSearchPathForDirectoriesInDomains( NSApplicationSupportDirectory, NSUserDomainMask, YES );
		if( [pPaths count] > 0 )
		{
			NSString* pPath = [pPaths objectAtIndex:0];
			strncpy( UserLocalAppDataPath, [pPath UTF8String], sizeof( UserLocalAppDataPath ) );
		}
		[pPaths release];
	}
	return SimpleString( UserLocalAppDataPath );
}

SimpleString ObjCJunk::GetUserPersonalPath()
{
	if ( !*UserPersonalPath )
	{
		CFStringGetCString( (CFStringRef)NSHomeDirectory() , UserPersonalPath , sizeof(UserPersonalPath) , kCFStringEncodingUTF8 );
	}
	return SimpleString( UserPersonalPath );
}

SimpleString ObjCJunk::GetUserDirectory()
{
	SimpleString AppSupportDir = GetUserLocalAppDataPath();
	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
    const SimpleString RetVal = AppSupportDir + SimpleString( "/Eldritch/" );
    NSString* pFullPath = [NSString stringWithUTF8String:RetVal.CStr()];
    NSFileManager* pFileManager = [[NSFileManager alloc] init];
    if( ![pFileManager fileExistsAtPath:pFullPath])
    {
        [pFileManager createDirectoryAtPath:pFullPath withIntermediateDirectories:YES attributes:nil error:nil];
    }

    [pPool drain];

    return RetVal;
}
