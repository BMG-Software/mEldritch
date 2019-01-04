#include "core.h"
#include "mathcore.h"
#include "configmanager.h"
#include "file.h"
#include "filestream.h"
#include "array.h"
#include "simplestring.h"

#include <stdio.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#if !BUILD_WINDOWS

#include "bmpheader.h"

struct RECT {
	long left, top;
	long right, bottom;
};

#endif

struct Surface {
	Surface(uint Width, uint Height)
	:	m_Width( Width )
	,	m_Height( Height )
	{
		m_Data = new unsigned char[ Width * Height ];
	}

	~Surface()
	{
		delete[] m_Data;
	}

	uint GetWidth() const { return m_Width; }
	uint GetHeight() const { return m_Height; }
	void Clear( unsigned char Byte )
	{
		memset( m_Data, Byte, m_Width * m_Height );
	}
	unsigned char* GetPointerAt( uint X, uint Y ) const
	{
		return &m_Data[ Y * m_Width + X ];
	}

	uint m_Width, m_Height;
	unsigned char* m_Data;
};

// Defines properties about a particular glyph or code point in a particular locale in a font.
struct SFGCodePointProps
{
	SFGCodePointProps()
	:	m_CodePoint( 0 )
	,	m_U1( 0.0f )
	,	m_U2( 0.0f )
	,	m_V1( 0.0f )
	,	m_V2( 0.0f )
	,	m_A( 0 )
	,	m_B( 0 )
	,	m_C( 0 )
	,	m_Width( 0.0f )
	{
	}

	unicode_t	m_CodePoint;
	float		m_U1;
	float		m_U2;
	float		m_V1;
	float		m_V2;
	int			m_A;
	uint		m_B;
	int			m_C;
	float		m_Width;
};

void ComputeWidth( SFGCodePointProps& CodePointProps )
{
	uint Width = 1 + Max( 0, CodePointProps.m_A ) + CodePointProps.m_B + Max( 0, CodePointProps.m_C );
	CodePointProps.m_Width = static_cast<float>( Width );
}

// Defines properties about a substitution for a glyph.
// Currently only supports image substitutions.
struct SGlyphSubstitute
{
	enum ESubType
	{
		EST_None,
		EST_Icon,
	};

	SGlyphSubstitute()
	:	m_SubType( EST_None )
	,	m_CodePoint( 0 )
	,	m_IconName()
	,	m_SpacingLeft()
	,	m_SpacingRight()
	{
	}

	ESubType		m_SubType;
	unicode_t		m_CodePoint;
	SimpleString	m_IconName;
	int				m_SpacingLeft;
	int				m_SpacingRight;
	int				m_HeightOffset;
};

// Defines properties about a specific instantiated substitution.
struct SBitmapSubstitute
{
	SBitmapSubstitute()
	:	m_X( 0 )
	,	m_Y( 0 )
	,	m_Width( 0 )
	,	m_Height( 0 )
	,	m_Pixels( NULL )
	,	m_HeightOffset( 0 )
	{
	}

	int		m_X;
	int		m_Y;
	int		m_Width;
	int		m_Height;
	byte*	m_Pixels;
	int		m_HeightOffset;
};

// Defines properties about a particular locale for the font (e.g. Latin alphabet).
struct SFGLocaleProps
{
	SFGLocaleProps()
	:	m_TextureWidth( 0 )
	,	m_TextureHeight( 0 )
	,	m_BitmapTextureName()
	,	m_LocaleTag()
	,	m_LocaleContext()
	,	m_ImageFilename()
	,	m_RuntimeImageFilename()
	,	m_Languages()
	,	m_CodePointProps()
	,	m_BitmapSubstitutes()
	{
	}

	int							m_TextureWidth;
	int							m_TextureHeight;
	SimpleString				m_BitmapTextureName;	// For bitmap fonts only, passed through without wrangling
	SimpleString				m_LocaleTag;
	SimpleString				m_LocaleContext;		// Config context
	SimpleString				m_ImageFilename;
	SimpleString				m_RuntimeImageFilename;	// Same as m_ImageFilename with leading ../Intermediate/ trimmed, for use as a relative path at runtime.
	Array<SimpleString>			m_Languages;
	Array<SFGCodePointProps>	m_CodePointProps;		// Populated in DrawFont/SaveBitmapFont, of course. >_<
	Array<SBitmapSubstitute>	m_BitmapSubstitutes;	// Locale-specific; can be in different places on different texture sheets.
};

// Defines base properties about a font. This is the root global data.
struct SFGProps
{
	SFGProps()
	:	m_OutputFontFilename()
	,	m_OutputImageTemplate()
	,	m_OutputImagePathname()
	,	m_BitmapFont( false )
	,	m_PointSize( 0 )
	,	m_Antialias( false )
	,	m_Face()
	,	m_Filename()
	,	m_Height( 0 )
	,	m_LocaleProps()
	,	m_GlyphSubstitutes()
	,	m_AlphaScalar( 0.0f )
	,	m_AlphaExponent( 0.0f )
	,	m_BitmapabcA( 0 )
	,	m_BitmapabcB( 0 )
	,	m_BitmapabcC( 0 )
	{
	}

	SimpleString			m_OutputFontFilename;
	SimpleString			m_OutputImageTemplate;
	SimpleString			m_OutputImagePathname;
	bool					m_BitmapFont;			// If true, we're using provided image(s) instead of drawing a new font sheet. (HACK for Eldritch, remove later.)
	int						m_PointSize;
	bool					m_Antialias;
	SimpleString			m_Face;
	SimpleString			m_Filename;
	int						m_Height;
	Array<SFGLocaleProps>	m_LocaleProps;
	Array<SGlyphSubstitute>	m_GlyphSubstitutes;		// Not locale-specific. They should generally live in the private use area (U+E000–U+F8FF).

	// For anti-aliased fonts
	float					m_AlphaScalar;
	float					m_AlphaExponent;

	// For bitmap fonts only.
	int						m_BitmapabcA;
	int						m_BitmapabcB;
	int						m_BitmapabcC;
};

// Create an in-memory surface from a bitmap. Only supports 32-bit BMP files.
// TODO: Why didn't I just use a Surface for this and then blit to the target?
void CreateBitmapSubstitute( const SimpleString& Filename, SBitmapSubstitute& OutSub )
{
	SimpleString FixedFilename = Filename;
	if( !FixedFilename.Contains( "../Raw/" ) )
	{
		FixedFilename = SimpleString::PrintF( "../Raw/%s", Filename.CStr() );
	}

	{
		FileStream Stream( FixedFilename.CStr(), FileStream::EFM_Read );

		BITMAPFILEHEADER BMPHeader;
		BITMAPINFOHEADER BMPInfo;

		Stream.Read( sizeof( BITMAPFILEHEADER ), &BMPHeader );
		Stream.Read( sizeof( BITMAPINFOHEADER ), &BMPInfo );

		OutSub.m_Width	= BMPInfo.biWidth;
		OutSub.m_Height	= BMPInfo.biHeight;
		const uint Size	= OutSub.m_Width * OutSub.m_Height * 4;
		OutSub.m_Pixels	= new byte[ Size ];	// TODO LOC LATER: Ever free this memory. Or not, it doesn't really matter.

		Stream.Read( Size, OutSub.m_Pixels );
	}
}

// Big huge side effect: initializes the code point properties, because
// we need to actually have the font loaded to get those values.
void DrawFont( Surface& LocaleSurface, SFGLocaleProps& LocaleProps, SFGProps& Props )
{
	const int PixelsY = 110;		// pixels per logical inch... this is derived from the caller's monitor in the original version of this

	unsigned char *FontBuffer = NULL;
	stbtt_fontinfo FontInfo;
	SimpleString FontFileName;

	if ( Props.m_Filename != "" )
	{
		FontFileName = Props.m_Filename;
	}
	else
	{
		FontFileName = Props.m_Face + ".ttf";
	}

	// load font
	if ( !FileUtil::Exists( FontFileName.CStr() ) )
	{
		SimpleString OtherFileName = FontFileName.ToLower();
		if ( !FileUtil::Exists( OtherFileName.CStr() ) )
		{
			PRINTF( "Can't open file \"%s\" for input\n", FontFileName.CStr() );
			exit( EXIT_FAILURE );
		}
		else
		{
			FontFileName = OtherFileName;
		}
	}

	FileStream FontFile ( FontFileName.CStr(), FileStream::EFM_Read );
	FontBuffer = new unsigned char[ FontFile.Size() ];
	FontFile.Read( FontFile.Size(), FontBuffer );

	stbtt_InitFont(&FontInfo, FontBuffer, 0);

	float Scale = stbtt_ScaleForPixelHeight( &FontInfo, (int)( 0.5f + static_cast<float>( Props.m_PointSize * PixelsY ) / 72.0f ) );

	const uint Width	= LocaleSurface.GetWidth();
	const uint Height	= LocaleSurface.GetHeight();

	RECT DrawRect;
	DrawRect.left	= 1;
	DrawRect.right	= Width;
	DrawRect.top	= 1;
	DrawRect.bottom	= Height;

	const float RecW = 1.0f / static_cast<float>( Width );
	const float RecH = 1.0f / static_cast<float>( Height );

	int Descent, Ascent, LineGap;
	stbtt_GetFontVMetrics(&FontInfo, &Ascent, &Descent, &LineGap);

	Props.m_Height = ceil( ( Ascent - Descent) * Scale );
	const int BaseLine = ceil ( ( Ascent ) * Scale );

	int Glyph;
	int LeftSpacing, Advance, GlyphWidth, YOfs, FontWidth;
	int Left;

	LocaleSurface.Clear( 0 );

	FOR_EACH_ARRAY( CodePointPropsIter, LocaleProps.m_CodePointProps, SFGCodePointProps )
	{
		SFGCodePointProps&	CodePointProps	= CodePointPropsIter.GetValue();
		const unicode_t		CodePoint		= CodePointProps.m_CodePoint;

		// Look for a substitute glyph
		SGlyphSubstitute::ESubType CurrentSubType = SGlyphSubstitute::EST_None;
		FOR_EACH_ARRAY( GlyphSubIter, Props.m_GlyphSubstitutes, SGlyphSubstitute )
		{
			SGlyphSubstitute& GlyphSub = GlyphSubIter.GetValue();
			if( GlyphSub.m_CodePoint == CodePoint )
			{
				CurrentSubType = GlyphSub.m_SubType;
				ASSERT( CurrentSubType == SGlyphSubstitute::EST_Icon );

				SBitmapSubstitute& BitmapSub = LocaleProps.m_BitmapSubstitutes.PushBack();
				CreateBitmapSubstitute( GlyphSub.m_IconName, BitmapSub );

				LeftSpacing = GlyphSub.m_SpacingLeft;
				Advance = BitmapSub.m_Width + GlyphSub.m_SpacingRight;

				BitmapSub.m_HeightOffset = GlyphSub.m_HeightOffset;
				break;
			}
		}

		if( CurrentSubType == SGlyphSubstitute::EST_None )
		{
			int X0, X1, Y0, Y1;
			Glyph = stbtt_FindGlyphIndex(&FontInfo, CodePoint);
			if ( Glyph == 0 )
			{
				printf( "Warning: no glyph found for char %lu\n", CodePoint );
			}
			stbtt_GetGlyphBitmapBox(&FontInfo, Glyph, Scale, Scale, &X0, &Y0, &X1, &Y1);
			stbtt_GetGlyphHMetrics(&FontInfo, Glyph, &Advance, &LeftSpacing);
			LeftSpacing = X0; //ceil( LeftSpacing * Scale );
			GlyphWidth = X1 - X0;
			Advance = ceil( Advance * Scale );
			YOfs = BaseLine + Y0;
		}

		// Add 1 pixel at the edges and between each font to prevent any kind of bleeding
		FontWidth = 1;
		FontWidth -= Max( 0, LeftSpacing );
		//FontWidth += Max( 0, GlyphWidth );
		FontWidth += Max( 0, Advance );

		Left = DrawRect.left;

		// Accomodate leading overhang
		if( LeftSpacing < 0 )
		{
			DrawRect.left -= LeftSpacing;
		}

		// Drop to the next line if there's not enough room
		if( DrawRect.left + FontWidth > DrawRect.right )
		{
			Left = DrawRect.left = 1;
			DrawRect.top += Props.m_Height + 1;
		}

		if ( DrawRect.top + Props.m_Height + 1 >= DrawRect.bottom )
		{
			PRINTF( "Texture is not large enough to contain all specified glyphs.\n" );
			exit( EXIT_FAILURE );
		}

		if( CurrentSubType == SGlyphSubstitute::EST_None )
		{
			stbtt_MakeCodepointBitmap(&FontInfo, LocaleSurface.GetPointerAt( DrawRect.left, DrawRect.top + YOfs ),
									FontWidth, Props.m_Height, LocaleSurface.m_Width,
									Scale, Scale, CodePoint);
		}
		else if( CurrentSubType == SGlyphSubstitute::EST_Icon )
		{
			// Don't actually draw the icon now. Just mark its spot in the surface, and copy it in later when exporting the TGA.
			ASSERT( LocaleProps.m_BitmapSubstitutes.Size() );
			SBitmapSubstitute& BitmapSub = LocaleProps.m_BitmapSubstitutes.Last();
			BitmapSub.m_X = DrawRect.left + LeftSpacing;
			BitmapSub.m_Y = ( DrawRect.top + BaseLine ) - BitmapSub.m_Height + BitmapSub.m_HeightOffset;
		}

		CodePointProps.m_U1	= ( static_cast<float>( Left ) - 0.5f ) * RecW;
		CodePointProps.m_U2	= ( static_cast<float>( Left + FontWidth ) - 0.5f ) * RecW;
		// Using Direct3D-style (top-to-bottom) UVs
		CodePointProps.m_V1	= ( static_cast<float>( DrawRect.top ) - 0.5f ) * RecH;
		CodePointProps.m_V2	= ( static_cast<float>( DrawRect.top + Props.m_Height ) + 0.5f ) * RecH;
		CodePointProps.m_A	= LeftSpacing;
		CodePointProps.m_B	= GlyphWidth;
		CodePointProps.m_C	= Advance - GlyphWidth - LeftSpacing;
		ComputeWidth( CodePointProps );

		DrawRect.left += FontWidth;

		// Accomodate leading overhang again
		if( LeftSpacing < 0 )
		{
			DrawRect.left += LeftSpacing;
		}
	}

	delete[] FontBuffer;
}

// Save target surface to a non-RLE 32-bit TGA.
void SaveTGA( const IDataStream& Stream, const Surface& WindowSurface, const SFGLocaleProps& LocaleProps, const SFGProps& Props )
{
	struct STGAHeader
	{
		byte	m_SizeOfIDField;
		byte	m_ColorMapType;
		byte	m_ImageType;		// I'll be concerned with 2 and 10 only

		// Broken up into bytes because of word alignment issues
		// TODO LOC LATER: I could also #pragma pack(push, 2) like I do in bmp-format.h
		byte	m_ColorMapOriginLo;
		byte	m_ColorMapOriginHi;
		byte	m_ColorMapLengthLo;
		byte	m_ColorMapLengthHi;
		byte	m_ColorMapBitDepth;

		uint16	m_OriginX;
		uint16	m_OriginY;
		uint16	m_Width;
		uint16	m_Height;
		byte	m_BitDepth;
		byte	m_ImageDescriptor;	// See TGA documentation
	};

	STGAHeader TGAHeader;

	memset( &TGAHeader, 0, sizeof( STGAHeader ) );
	TGAHeader.m_ImageType		= 2;
	TGAHeader.m_Width			= static_cast<uint16>( WindowSurface.GetWidth() );
	TGAHeader.m_Height			= static_cast<uint16>( WindowSurface.GetHeight() );
	TGAHeader.m_BitDepth		= 32;
	TGAHeader.m_ImageDescriptor	= 8;

	Stream.Write( sizeof( TGAHeader ), &TGAHeader );

	const int Width		= WindowSurface.GetWidth();
	const int Height	= WindowSurface.GetHeight();
	byte r, g, b, a;
	STATICHASH( ColorR );
	STATICHASH( ColorG );
	STATICHASH( ColorB );
	r = (byte)ConfigManager::GetInt( sColorR, 255 );
	g = (byte)ConfigManager::GetInt( sColorG, 255 );
	b = (byte)ConfigManager::GetInt( sColorB, 255 );
	for( int j = Height - 1; j >= 0; --j )
	{
		for( int i = 0; i < Width; ++i )
		{
			// Paste in bitmap substitutes as needed
			bool UsedBitmapSub = false;
			FOR_EACH_ARRAY( BitmapSubIter, LocaleProps.m_BitmapSubstitutes, SBitmapSubstitute )
			{
				const SBitmapSubstitute& Bitmap = BitmapSubIter.GetValue();
				if( i >= Bitmap.m_X &&
					i < Bitmap.m_X + Bitmap.m_Width &&
					j >= Bitmap.m_Y &&
					j < Bitmap.m_Y + Bitmap.m_Height )
				{
					int LocalX = i - Bitmap.m_X;
					int LocalY = Bitmap.m_Height - ( j - Bitmap.m_Y ) - 1;

					if(
						LocalX >= 0 &&
						LocalX < Bitmap.m_Width &&
						LocalY >= 0 &&
						LocalY < Bitmap.m_Height )
					{
						UsedBitmapSub = true;
						byte* const pPixel = Bitmap.m_Pixels + LocalY * Bitmap.m_Width * 4 + LocalX * 4;
						Stream.Write( 4, pPixel );
						break;
					}
				}
			}

			if( !UsedBitmapSub )
			{
				// Use inverse blue of window surface as alpha
				// HACK: Gamma correct the alpha to bolden up antialiasing
				const byte	RawAlpha		= *( WindowSurface.GetPointerAt( i, j ) );
				const float	Alpha			= static_cast<float>( RawAlpha ) / 255.0f;
				const float AdjustedAlpha	= Pow( ( Alpha * Props.m_AlphaScalar ), Props.m_AlphaExponent );
				const float FinalAlpha		= Saturate( Props.m_Antialias ? AdjustedAlpha : Alpha );
				a = static_cast<byte>( FinalAlpha * 255.0f );

				Stream.WriteUInt8( b );
				Stream.WriteUInt8( g );
				Stream.WriteUInt8( r );
				Stream.WriteUInt8( a );
			}
		}
	}
}

void SaveCodePointProps( const IDataStream& Stream, const SFGCodePointProps& CodePointProps )
{
	// Just save the whole struct inline, it's densely packed.
	// On the runtime end, the first member (the unicode value)
	// will be serialized first to use as the key.
	Stream.Write( sizeof( SFGCodePointProps ), &CodePointProps );
}

void SaveLocaleProps( const IDataStream& Stream, const SFGLocaleProps& LocaleProps )
{
	// Write locale tag
	Stream.WriteHashedString( LocaleProps.m_LocaleTag );

	// Write relative image filename
	Stream.WriteString( LocaleProps.m_RuntimeImageFilename );

	// Write language array
	Stream.WriteUInt32( LocaleProps.m_Languages.Size() );
	FOR_EACH_ARRAY( LanguageIter, LocaleProps.m_Languages, SimpleString )
	{
		const SimpleString& Language = LanguageIter.GetValue();
		Stream.WriteHashedString( Language );
	}

	// Write code points
	Stream.WriteUInt32( LocaleProps.m_CodePointProps.Size() );
	FOR_EACH_ARRAY( CodePointPropsIter, LocaleProps.m_CodePointProps, SFGCodePointProps )
	{
		const SFGCodePointProps& CodePointProps = CodePointPropsIter.GetValue();
		SaveCodePointProps( Stream, CodePointProps );
	}
}

void SaveProps( const IDataStream& Stream, const SFGProps& Props )
{
	// Write global font properties
	Stream.WriteInt32( Props.m_Height );

	// Then write per-locale properties
	Stream.WriteUInt32( Props.m_LocaleProps.Size() );
	FOR_EACH_ARRAY( LocalePropsIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		const SFGLocaleProps& LocaleProps = LocalePropsIter.GetValue();
		SaveLocaleProps( Stream, LocaleProps );
	}
}

void PopulateBitmapCodePoints( const SFGProps& Props, SFGLocaleProps& LocaleProps )
{
	const float	fCharWidth	= static_cast<float>( Max( 0, Props.m_BitmapabcA ) + Props.m_BitmapabcB + Max( 0, Props.m_BitmapabcC ) );
	const float fCharHeight	= static_cast<float>( Props.m_Height );
	const uint	CharsPerRow	= static_cast<uint>( LocaleProps.m_TextureWidth / ( fCharWidth + 1.0f ) );

	uint	RowChar	= 0;
	float	X		= 1.0f;
	float	Y		= 1.0f;

	FOR_EACH_ARRAY( CodePointPropsIter, LocaleProps.m_CodePointProps, SFGCodePointProps )
	{
		SFGCodePointProps&	CodePointProps	= CodePointPropsIter.GetValue();

		CodePointProps.m_U1			= ( X - .5f )					/ LocaleProps.m_TextureWidth;
		CodePointProps.m_U2			= ( X + fCharWidth + 0.5f )		/ LocaleProps.m_TextureWidth;
		CodePointProps.m_V1			= ( Y - .5f )					/ LocaleProps.m_TextureHeight;
		CodePointProps.m_V2			= ( Y + fCharHeight + 0.5f )	/ LocaleProps.m_TextureHeight;
		CodePointProps.m_A			= Props.m_BitmapabcA;
		CodePointProps.m_B			= Props.m_BitmapabcB;
		CodePointProps.m_C			= Props.m_BitmapabcC;
		ComputeWidth( CodePointProps );

		if( ++RowChar == CharsPerRow )
		{
			RowChar	= 0;
			X		= 1.0f;
			Y		+= fCharHeight + 1.0f;
		}
		else
		{
			X		+= fCharWidth + 1.0f;
		}
	}
}

void PopulateBitmapCodePoints( const SFGProps& Props )
{
	FOR_EACH_ARRAY( LocaleIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		SFGLocaleProps& LocaleProps = LocaleIter.GetValue();
		PopulateBitmapCodePoints( Props, LocaleProps );
	}
}

void PassThroughBitmapFilenames( SFGLocaleProps& LocaleProps )
{
	LocaleProps.m_RuntimeImageFilename = LocaleProps.m_BitmapTextureName;
}

void PassThroughBitmapFilenames( const SFGProps& Props )
{
	FOR_EACH_ARRAY( LocaleIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		SFGLocaleProps& LocaleProps = LocaleIter.GetValue();
		PassThroughBitmapFilenames( LocaleProps );
	}
}

// As a reminder, here's what I do with filename wrangling:
// 0. Given a target font name (../Baked/Fonts/foo.fnp) and an
// image template name (../Intermediate/Textures/Fonts/foo.tga).
// Font name does not need any wrangling. The following rules
// are for image filename wrangling only:
// 1. Insert a locale suffix (e.g. -Latin) before the extension
// (../Intermediate/Textures/Fonts/foo-Latin.tga)
// 2. Insert _NODXT and _DXT5 suffixes for no-compress and antialiased
// font sheets respectively. Does not apply to bitmap fonts.
// (../Intermediate/Textures/Fonts/foo-Latin_NODXT.tga)
// 3. Replace the .tga extension with .dds unless the font is a no-compress
// font or a bitmap font.
// (../Intermediate/Textures/Fonts/foo-Latin_NODXT.dds)
// 4. Trim the leading ../Intermediate/ in the version of the filenames
// that get saved into the .fnp, to produce a relative path for runtime.
// (Textures/Fonts/foo-Latin_NODXT.dds)

void WrangleLocaleFilenames( SFGLocaleProps& LocaleProps, const SFGProps& Props )
{
	ASSERT( !Props.m_BitmapFont );

	const SimpleString& ImageFilenameTemplate = Props.m_OutputImageTemplate;
	SimpleString Name = FileUtil::StripExtension( ImageFilenameTemplate.CStr() );;
	SimpleString Extension = FileUtil::StripAllButExtension( ImageFilenameTemplate.CStr() );

	// 1. Add locale suffix
	Name = SimpleString::PrintF( "%s-%s", Name.CStr(), LocaleProps.m_LocaleTag.CStr() );

	// 2. Insert suffixes for settings
	SimpleString SettingsName = Name;
	Name += "_NODXT";

	// 3. Replace the runtime extension
	Extension = ".tga";

	// 4. Trim ../Intermediate/ for runtime
	SimpleString Intermediate;
	SimpleString Remainder;
	Name.Split( 16, Intermediate, Remainder );

	LocaleProps.m_ImageFilename			= Name + ".tga";
	if ( Props.m_OutputImagePathname != "" )
	{
		LocaleProps.m_RuntimeImageFilename	= Props.m_OutputImagePathname + "/" + FileUtil::StripLeadingFolders( LocaleProps.m_ImageFilename.CStr() );
	}
	else
	{
		LocaleProps.m_RuntimeImageFilename	= LocaleProps.m_ImageFilename;
	}
}

void WrangleFilenames( const SFGProps& Props )
{
	// Shouldn't be wrangling image filenames for bitmap fonts.
	ASSERT( !Props.m_BitmapFont );

	FOR_EACH_ARRAY( LocaleIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		SFGLocaleProps& LocaleProps = LocaleIter.GetValue();
		WrangleLocaleFilenames( LocaleProps, Props );
	}
}

void AddUnicodeRange( const unicode_t RangeLow, const unicode_t RangeHigh, Array<SFGCodePointProps>& CodePointProps )
{
	for( unicode_t CodePoint = RangeLow; CodePoint <= RangeHigh; ++CodePoint )
	{
		SFGCodePointProps& Props = CodePointProps.PushBack();
		Props.m_CodePoint = CodePoint;
	}
}

void AddLocales( SFGProps& Props )
{
	STATICHASH( NumLocales );
	const uint NumLocales = ConfigManager::GetInt( sNumLocales );

	if( NumLocales == 0 )
	{
		// Add default locale if none is specified.
		SFGLocaleProps& LocaleProps = Props.m_LocaleProps.PushBack();
		LocaleProps.m_LocaleTag = "Latin";
	}
	else
	{
		for( uint LocaleIndex = 0; LocaleIndex < NumLocales; ++LocaleIndex )
		{
			SFGLocaleProps& LocaleProps	= Props.m_LocaleProps.PushBack();

			const SimpleString Locale	= ConfigManager::GetSequenceString( "Locale%d", LocaleIndex, "" );
			LocaleProps.m_LocaleContext	= Locale;

			MAKEHASH( Locale );
			STATICHASH( Tag );
			LocaleProps.m_LocaleTag		= ConfigManager::GetString( sTag, "", sLocale );
		}
	}
}

// Load glyph substitute configuration data into the target props struct.
void AddGlyphSubstitutes( SFGProps& Props )
{
	STATICHASH( NumSubstitutes );
	const uint NumSubstitutes = ConfigManager::GetInt( sNumSubstitutes );

	for( uint SubstituteIndex = 0; SubstituteIndex < NumSubstitutes; ++SubstituteIndex )
	{
		SGlyphSubstitute NewSubstitute;

		const SimpleString SubstituteDef = ConfigManager::GetSequenceString( "Substitute%d", SubstituteIndex, "" );
		MAKEHASH( SubstituteDef );

		STATICHASH( CodePoint );
		const uint			CodePoint		= ConfigManager::GetInt( sCodePoint, 0, sSubstituteDef );
		const SimpleString	CodePointString	= ConfigManager::GetString( sCodePoint, "", sSubstituteDef );
		NewSubstitute.m_CodePoint = Pick( CodePoint, CodePointString.GetCodePoint() );

		NewSubstitute.m_SubType = SGlyphSubstitute::EST_Icon;	// This is the only substitution I support anymore.

		STATICHASH( Image );
		NewSubstitute.m_IconName = ConfigManager::GetString( sImage, NULL, sSubstituteDef );

		STATICHASH( SpacingLeft );
		NewSubstitute.m_SpacingLeft = ConfigManager::GetInt( sSpacingLeft, 0, sSubstituteDef );

		STATICHASH( SpacingRight );
		NewSubstitute.m_SpacingRight = ConfigManager::GetInt( sSpacingRight, 0, sSubstituteDef );

		STATICHASH( HeightOffset );
		NewSubstitute.m_HeightOffset = ConfigManager::GetInt( sHeightOffset, 0, sSubstituteDef );

		Props.m_GlyphSubstitutes.PushBack( NewSubstitute );
	}
}

void AddLanguages( SFGLocaleProps& LocaleProps )
{
	MAKEHASHFROM( Locale, LocaleProps.m_LocaleContext );

	STATICHASH( NumLanguages );
	const uint NumLanguages = ConfigManager::GetInt( sNumLanguages, 0, sLocale );

	if( NumLanguages == 0 )
	{
		// Add default language if none is specified.
		LocaleProps.m_Languages.PushBack( "English" );
	}
	else
	{
		for( uint LanguageIndex = 0; LanguageIndex < NumLanguages; ++LanguageIndex )
		{
			const SimpleString Language = ConfigManager::GetSequenceString( "Language%d", LanguageIndex, "", sLocale );
			LocaleProps.m_Languages.PushBack( Language );
		}
	}
}

void AddCodePoints( SFGLocaleProps& LocaleProps )
{
	MAKEHASHFROM( Locale, LocaleProps.m_LocaleContext );

	STATICHASH( NumCodePointRanges );
	const uint NumCodePointRanges = ConfigManager::GetInt( sNumCodePointRanges, 0, sLocale );

	if( NumCodePointRanges == 0 )
	{
		// Add default characters if none are specified.
		AddUnicodeRange( 0x20, 0x7f, LocaleProps.m_CodePointProps );
		AddUnicodeRange( 0xa0, 0xff, LocaleProps.m_CodePointProps );
		AddUnicodeRange( 0x09, 0x09, LocaleProps.m_CodePointProps );	// Tab
	}
	else
	{
		for( uint RangeIndex = 0; RangeIndex < NumCodePointRanges; ++RangeIndex )
		{
			const SimpleString	RangeLowCode	= ConfigManager::GetSequenceString( "CodePointRange%dLow", RangeIndex, "", sLocale );
			const SimpleString	RangeHighCode	= ConfigManager::GetSequenceString( "CodePointRange%dHigh", RangeIndex, "", sLocale );

			const unicode_t		RangeLow		= RangeLowCode.GetCodePoint();
			const unicode_t		RangeHigh		= RangeHighCode.GetCodePoint();

			AddUnicodeRange( RangeLow, RangeHigh, LocaleProps.m_CodePointProps );
		}
	}
}

void PopulateLocaleProps( SFGLocaleProps& LocaleProps )
{
	MAKEHASHFROM( Locale, LocaleProps.m_LocaleContext );

	// For backward compatibility and simplicity, dimensions can
	// be defined in the main section or overridden per locale.
	STATICHASH( Width );
	STATICHASH( Height );
	LocaleProps.m_TextureWidth	= ConfigManager::GetInt( sWidth, ConfigManager::GetInt( sWidth ), sLocale );
	LocaleProps.m_TextureHeight	= ConfigManager::GetInt( sHeight, ConfigManager::GetInt( sHeight ), sLocale );

	STATICHASH( Texture );
	LocaleProps.m_BitmapTextureName = ConfigManager::GetString( sTexture, ConfigManager::GetString( sTexture, "" ), sLocale );

	AddLanguages( LocaleProps );
	AddCodePoints( LocaleProps );
}

void PopulateProps( SFGProps& Props )
{
	STATICHASH( BitmapFont );
	Props.m_BitmapFont = ConfigManager::GetBool( sBitmapFont );

	STATICHASH( Size );
	Props.m_PointSize = ConfigManager::GetInt( sSize, 12 );

	STATICHASH( Face );
	Props.m_Face = ConfigManager::GetString( sFace, "Arial" );

	STATICHASH( Filename );
	Props.m_Filename = ConfigManager::GetString( sFilename, "" );

	STATICHASH( Antialias );
	Props.m_Antialias = ConfigManager::GetBool( sAntialias );

	STATICHASH( abcA );
	Props.m_BitmapabcA = ConfigManager::GetInt( sabcA );

	STATICHASH( abcB );
	Props.m_BitmapabcB = ConfigManager::GetInt( sabcB );

	STATICHASH( abcC );
	Props.m_BitmapabcC = ConfigManager::GetInt( sabcC );

	STATICHASH( CharHeight );
	Props.m_Height = ConfigManager::GetInt( sCharHeight );

	STATICHASH( AlphaScalar );
	Props.m_AlphaScalar = ConfigManager::GetFloat( sAlphaScalar, 2.0f );

	STATICHASH( AlphaExponent );
	Props.m_AlphaExponent = ConfigManager::GetFloat( sAlphaExponent, 1.0f );

	AddLocales( Props );
	FOR_EACH_ARRAY( LocaleIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		PopulateLocaleProps( LocaleIter.GetValue() );
	}

	AddGlyphSubstitutes( Props );
}

void DrawAndSaveImages( SFGProps& Props )
{
	FOR_EACH_ARRAY( LocaleIter, Props.m_LocaleProps, SFGLocaleProps )
	{
		SFGLocaleProps& LocaleProps = LocaleIter.GetValue();

		Surface LocaleSurface( LocaleProps.m_TextureWidth, LocaleProps.m_TextureHeight );
		DrawFont( LocaleSurface, LocaleProps, Props );

		// Save bitmaps as we process locales, but only save .fnp at the end.
		SaveTGA( FileStream( LocaleProps.m_ImageFilename.CStr(), FileStream::EFM_Write ), LocaleSurface, LocaleProps, Props );
	}
}

int main( int argc, char *argv[] )
{
	if ( argc < 4 || argc > 5)
	{
		PRINTF( "Syntax: FontGenerator <infile> <outfontfile> <outimagefile> [ <runtimeimagepath> ]\n" );
		return 0;
	}

	const char*	InFile			= argv[1];
	ASSERT( InFile );
	const char*	OutFontFile		= argv[2];
	ASSERT( OutFontFile );
	const char*	OutImageFile	= argv[3];	// NOTE: The actual output filename(s) will be modified per locale and according to settings.
	ASSERT( OutImageFile );

	const char* RuntimeImagePath = "";
	if ( argc == 5 )
	{
		RuntimeImagePath = argv[4];
	}

	ConfigManager::Load( FileStream( InFile, FileStream::EFM_Read ) );

	SFGProps Props;
	Props.m_OutputFontFilename	= OutFontFile;
	Props.m_OutputImageTemplate	= OutImageFile;
	Props.m_OutputImagePathname = RuntimeImagePath;
	PopulateProps( Props );
	ASSERT( Props.m_LocaleProps.Size() );

	if( Props.m_BitmapFont )
	{
		PopulateBitmapCodePoints( Props );
		PassThroughBitmapFilenames( Props );
	}
	else
	{
		WrangleFilenames( Props );
		DrawAndSaveImages( Props );
	}

	SaveProps( FileStream( Props.m_OutputFontFilename.CStr(), FileStream::EFM_Write ), Props );

	return 0;
}
