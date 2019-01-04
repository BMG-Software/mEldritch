#ifndef CUSTOMASSERT_H
#define CUSTOMASSERT_H

// ASSERT only evaluates the expression in specified builds
// CHECK evaluates the expression in every build but only checks it in specified builds
// WARN takes no expression and is used in place of WARN
// Use ASSERTDESC, CHECKDESC, and WARNDESC to add a description of the test

// Disable for perf. When this is set, assert expressions are not compiled out of Final builds
#define ASSERTS_IN_FINAL 0
#define PRINT_ASSERT_ONLY ( 0 || BUILD_FINAL )

#if BUILD_DEBUG || ASSERTS_IN_FINAL

#if PRINT_ASSERT_ONLY

#define INNERASSERT( exp, desc, type ) \
	do \
	{ \
		static bool Disabled = 0; \
		const char*	Assertion = "%s failed: \"%s\" (%s)\nin %s in %s at line %d\n"; \
		if( !Disabled && !( exp ) ) \
		{ \
			PRINTF( Assertion, type, #exp, desc, __FUNCTION__, __FILE__, __LINE__ ); \
			Disabled = true; \
		} \
	} while(0)

#else

#define INNERASSERT( exp, desc, type ) \
	do \
	{ \
		static bool Disabled = false; \
		const char*	Assertion = "%s failed: \"%s\" (%s)\nin %s in %s at line %d\n"; \
		if( !Disabled && !( exp ) ) \
		{ \
			PRINTF( Assertion, type, #exp, desc, __FUNCTION__, __FILE__, __LINE__ ); \
			if( CustomAssert::ShouldAssert( Disabled, Assertion, type, #exp, desc, __FUNCTION__, __FILE__, __LINE__ ) ) \
			{ \
				BREAKPOINT; \
			} \
		} \
	} while(0)

#endif

#define ASSERTDESC( exp, desc ) INNERASSERT( exp, desc, "Assertion" )
#define CHECKDESC( exp, desc ) INNERASSERT( exp, desc, "Check" )
#define WARNDESC( desc ) INNERASSERT( false, desc, "Warn" )

#else

#define ASSERTDESC( exp, desc ) DoNothing
#define CHECKDESC( exp, desc ) DoExp( exp )
#define WARNDESC( desc ) DoNothing

#endif

#if BUILD_DEBUG
#define DEBUGASSERTDESC( exp, desc ) ASSERTDESC( exp, desc )
#define DEBUGCHECKDESC( exp, desc ) CHECKDESC( exp, desc )
#define DEBUGWARNDESC( desc ) WARNDESC( desc )
#else
#define DEBUGASSERTDESC( exp, desc ) DoNothing
#define DEBUGCHECKDESC( exp, desc ) DoExp( exp )
#define DEBUGWARNDESC( desc ) DoNothing
#endif

#if BUILD_DEV
#define DEVASSERTDESC( exp, desc ) ASSERTDESC( exp, desc )
#define DEVGCHECKDESC( exp, desc ) CHECKDESC( exp, desc )
#define DEVWARNDESC( desc ) WARNDESC( desc )
#else
#define DEVASSERTDESC( exp, desc ) DoNothing
#define DEVCHECKDESC( exp, desc ) DoExp( exp )
#define DEVWARNDESC( desc ) DoNothing
#endif

#define ASSERT( exp ) ASSERTDESC( exp, "" )
#define CHECK( exp ) CHECKDESC( exp, "" )
#define WARN WARNDESC( "" )
#define DEBUGASSERT( exp ) DEBUGASSERTDESC( exp, "" )
#define DEBUGCHECK( exp ) DEBUGCHECKDESC( exp, "" )
#define DEBUGWARN DEBUGWARNDESC( "" )
#define DEVASSERT( exp ) DEVASSERTDESC( exp, "" )
#define DEVCHECK( exp ) DEVCHECKDESC( exp, "" )
#define DEVWARN DEVWARNDESC( "" )

namespace CustomAssert
{
	bool	ShouldAssert( bool& Disable, const char* Assertion, ... );
}

#endif
