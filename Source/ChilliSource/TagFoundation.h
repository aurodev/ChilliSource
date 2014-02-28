/*
 *  TagFoundation.h
 *  moFlo
 *
 *  Created by Scott Downie on 22/09/2010.
 *  Copyright 2010 Tag Games. All rights reserved.
 *
 */

//========================================================
// Description:
//
// Holds the type defines for integral data types as set
// out in the Tag Games coding standards.
//
// Holds required header files
//========================================================

#ifndef _TAG_FOUNDATION_H_
#define _TAG_FOUNDATION_H_

//TODO: Better way to not include the headers from here but still be able to swap libs easily
#include <memory>

//TODO: Remove the array deleter and put it in a more sutiable place. This is a functor for deleting shared arrays.
template <typename T> class ArrayDeleter
{
public:
    void operator () (T* in_arrayObject) const
    {
        delete[] in_arrayObject;
    }
};

#define SHARED_PTR std::shared_ptr
#define WEAK_PTR std::weak_ptr
#define WEAK_ARRAY_PTR std::weak_array
#define UNIQUE_PTR std::unique_ptr
#define UNIQUE_ARRAY_PTR std::unique_array
#define SHARED_PTR_CAST std::static_pointer_cast
#define ENABLE_SHARED_FROM_THIS std::enable_shared_from_this
#define SHARED_FUTURE std::shared_future
#define MAKE_SHARED_PTR std::make_shared

//TODO: Better way to not include the headers from here but still be able to swap libs easily i.e. if we want to use a different STL implementation
#include <unordered_map>
#define HASH_MAP std::unordered_map

//TODO: Better way to not include the headers from here but still be able to swap libs easily
#include <vector>
#define DYNAMIC_ARRAY std::vector

#include <string>
#include <cstdint>

typedef char         		s8;
typedef int_least16_t		s16;
typedef int_least32_t		s32;
typedef int_least64_t		s64;

typedef unsigned char		u8;
typedef uint_least16_t		u16;
typedef uint_least32_t		u32;
typedef uint_least64_t		u64;

typedef float				f32;
typedef double				f64;

typedef u64                 TimeIntervalSecs;
typedef u64                 TimeIntervalMs;
typedef	const s8*			cString;

namespace ChilliSource
{
    namespace Audio{}
    namespace Core{}
    namespace Debugging{}
    namespace Input{}
    namespace Networking{}
    namespace Rendering{}
    namespace Social{}
    namespace Video{}
    namespace Web{}
}


namespace CSAudio = ChilliSource::Audio;
namespace CSCore = ChilliSource::Core;
namespace CSDebugging = ChilliSource::Debugging;
namespace CSInput = ChilliSource::Input;
namespace CSNetworking = ChilliSource::Networking;
namespace CSRendering = ChilliSource::Rendering;
namespace CSSocial = ChilliSource::Social;
namespace CSVideo = ChilliSource::Video;
namespace CSWeb = ChilliSource::Web;

//Static Assertions
template <bool> struct CompileTimeChecker
{
    CompileTimeChecker(...);
};
template <> struct CompileTimeChecker<false>{};


//---Macros
#define SAFE_DELETE(x)						{if(x) delete(x); x = NULL;}
#define SAFE_DELETE_ARRAY(x)				{if(x) delete[] (x); x = NULL;}

#define STRING_CAST(x)						(ChilliSource::Core::CStringConverter::ToString(x))

#define PARSE_FLOAT(x)                      (Core::CStringConverter::ParseFloat(x))
#define PARSE_INT(x)                        (Core::CStringConverter::ParseInt(x))
#define PARSE_UINT(x)                       (Core::CStringConverter::ParseUnsignedInt(x))
#define PARSE_BOOL(x)                       (Core::CStringConverter::ParseBool(x))
#define PARSE_VECTOR2(x)                    (Core::CStringConverter::ParseVector2(x))
#define PARSE_VECTOR3(x)                    (Core::CStringConverter::ParseVector3(x))
#define PARSE_VECTOR4(x)                    (Core::CStringConverter::ParseVector4(x))
#define PARSE_MATRIX4X4(x)                  (Core::CStringConverter::ParseMatrix4(x))
#define PARSE_QUATERNION(x)                 (Core::CStringConverter::ParseQuaternion(x))
#define PARSE_COLOUR(x)                     (Core::CStringConverter::ParseColourValue(x))

#define STRING_HASH(x)						ChilliSource::Core::CHashCRC32::GenerateHashCode(x)
#define GET_LOCALISED_TEXT(x)               ChilliSource::Core::CLocalisedText::GetText(x)

#define DEBUG_LOG(x)						(ChilliSource::Core::CLogging::LogVerbose(x))
#define WARNING_LOG(x)						(ChilliSource::Core::CLogging::LogWarning(x))
#define ERROR_LOG(x)						(ChilliSource::Core::CLogging::LogError(x))
#define FATAL_LOG(x)						(ChilliSource::Core::CLogging::LogFatal(x))


#ifdef DEBUG
    #define MOFLOW_ASSERT(x, msg) if((x) == 0){ChilliSource::Core::CLogging::LogFatal(msg);}
#else
    #define MOFLOW_ASSERT(x, msg)
#endif

#ifdef DEBUG
    #define SMART_CAST(to, from) dynamic_cast<to >(from); assert(dynamic_cast<to >(from) != NULL);  
#else
    #define SMART_CAST(to, from) static_cast<to >(from);
#endif


#include <ChilliSource/Core/Base/Logging.h>
#include <ChilliSource/BuildProperties.h>
#include <ChilliSource/Core/String/StringConverter.h>
#include <ChilliSource/Core/Localisation/LocalisedText.h>

//---Missing OpenGL definitions
#define GL_GLEXT_PROTOTYPES
#ifdef TARGET_ANDROID
	typedef char GLchar;
#endif

#endif