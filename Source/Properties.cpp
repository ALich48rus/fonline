#include "Properties.h"
#include "FileSystem.h"
#include "Script.h"
#include "IniParser.h"

Property::Property()
{
    nativeSendCallback = NULL;
    nativeSetCallback = NULL;
    setCallbacksAnyOldValue = false;
    getCallbackLocked = false;
    setCallbackLocked = false;
    sendIgnoreObj = NULL;
}

void* Property::CreateComplexValue( uchar* data, uint data_size )
{
    if( dataType == Property::String )
    {
        ScriptString* str = ScriptString::Create( data_size ? (char*) data : "", data_size );
        RUNTIME_ASSERT( str );
        return str;
    }
    else if( dataType == Property::Array )
    {
        asUINT       element_size = asObjType->GetEngine()->GetSizeOfPrimitiveType( asObjType->GetSubTypeId() );
        asUINT       arr_size = data_size / element_size;
        ScriptArray* arr = ScriptArray::Create( asObjType, arr_size );
        RUNTIME_ASSERT( arr );
        if( arr_size )
            memcpy( arr->At( 0 ), data, arr_size * element_size );
        return arr;
    }
    else
    {
        RUNTIME_ASSERT( !"Unexpected type" );
    }
    return NULL;
}

void Property::AddRefComplexValue( void* value )
{
    if( dataType == Property::String )
        ( (ScriptString*) value )->AddRef();
    else if( dataType == Property::Array )
        ( (ScriptArray*) value )->AddRef();
    else
        RUNTIME_ASSERT( !"Unexpected type" );
}

void Property::ReleaseComplexValue( void* value )
{
    if( dataType == Property::String )
        ( (ScriptString*) value )->Release();
    else if( dataType == Property::Array )
        ( (ScriptArray*) value )->Release();
    else
        RUNTIME_ASSERT( !"Unexpected type" );
}

uchar* Property::ExpandComplexValueData( void* base_ptr, uint& data_size, bool& need_delete )
{
    need_delete = false;
    if( dataType == Property::String )
    {
        ScriptString* str = *(ScriptString**) base_ptr;
        data_size = ( str ? str->length() : 0 );
        return data_size ? (uchar*) str->c_str() : NULL;
    }
    else if( dataType == Property::Array )
    {
        ScriptArray* arr = *(ScriptArray**) base_ptr;
        data_size = ( arr ? arr->GetSize() * arr->GetElementSize() : 0 );
        return data_size ? (uchar*) arr->At( 0 ) : NULL;
    }
    else
    {
        RUNTIME_ASSERT( !"Unexpected type" );
    }
    return NULL;
}

void Property::GenericGet( void* obj, void* ret_value )
{
    Properties* properties = (Properties*) obj;

    // Virtual property
    if( accessType & Property::VirtualMask )
    {
        if( !getCallback )
        {
            memzero( ret_value, baseSize );
            SCRIPT_ERROR_R( "'Get' callback is not assigned for virtual property '%s %s::%s'.",
                            typeName.c_str(), properties->registrator->scriptClassName.c_str(), propName.c_str() );
        }

        if( getCallbackLocked )
        {
            memzero( ret_value, baseSize );
            SCRIPT_ERROR_R( "Recursive call for virtual property '%s %s::%s'.",
                            typeName.c_str(), properties->registrator->scriptClassName.c_str(), propName.c_str() );
        }

        getCallbackLocked = true;

        #ifndef FONLINE_SCRIPT_COMPILER
        if( Script::PrepareContext( getCallback, _FUNC_, GetName() ) )
        {
            if( getCallbackArgs > 0 )
                Script::SetArgObject( obj );
            if( getCallbackArgs > 1 )
                Script::SetArgUInt( enumValue );
            if( Script::RunPrepared() )
            {
                getCallbackLocked = false;

                memcpy( ret_value, Script::GetReturnedRawAddress(), baseSize );
                if( !IsPOD() )
                {
                    void*& val = *(void**) ret_value;
                    if( val )
                        AddRefComplexValue( val );
                    else
                        val = CreateComplexValue( NULL, 0 );
                }
                return;
            }
        }
        #endif

        getCallbackLocked = false;

        // Error
        memzero( ret_value, baseSize );
        SCRIPT_ERROR_R( "Error in Get callback execution for virtual property '%s %s::%s'",
                        typeName.c_str(), properties->registrator->scriptClassName.c_str(), propName.c_str() );
    }

    // Raw property
    if( dataType == Property::POD )
    {
        RUNTIME_ASSERT( podDataOffset != uint( -1 ) );
        memcpy( ret_value, &properties->podData[ podDataOffset ], baseSize );
    }
    else
    {
        RUNTIME_ASSERT( complexDataIndex != uint( -1 ) );
        uchar* data = properties->complexData[ complexDataIndex ];
        uint   data_size = properties->complexDataSizes[ complexDataIndex ];
        void*  result = CreateComplexValue( data, data_size );
        memcpy( ret_value, &result, baseSize );
    }
}

void Property::GenericSet( void* obj, void* new_value )
{
    Properties* properties = (Properties*) obj;

    // Get current POD value
    void* cur_value;
    if( dataType == Property::POD )
    {
        RUNTIME_ASSERT( podDataOffset != uint( -1 ) );
        cur_value = &properties->podData[ podDataOffset ];

        // Clamp
        if( checkMinValue || checkMaxValue )
        {
            uint64 check_value = 0;
            if( baseSize == 1 )
                check_value = *(uchar*) new_value;
            else if( baseSize == 2 )
                check_value = *(ushort*) new_value;
            else if( baseSize == 4 )
                check_value = *(uint*) new_value;
            else if( baseSize == 8 )
                check_value = *(uint64*) new_value;

            if( checkMinValue && check_value < (uint64) minValue )
            {
                check_value = minValue;
                GenericSet( obj, &check_value );
                return;
            }
            else if( checkMaxValue && check_value > (uint64) maxValue )
            {
                check_value = maxValue;
                GenericSet( obj, &check_value );
                return;
            }
        }

        // Ignore void calls
        if( !memcmp( new_value, cur_value, baseSize ) )
            return;
    }

    // Get current complex value
    void* complex_value = NULL;
    void* cur_complex_value = NULL;
    if( dataType != Property::POD )
    {
        RUNTIME_ASSERT( complexDataIndex != uint( -1 ) );

        // Expand new value data for comparison
        bool   need_delete;
        uint   new_value_data_size;
        uchar* new_value_data = ExpandComplexValueData( new_value, new_value_data_size, need_delete );

        // Get current data for comparison
        uint   cur_value_data_size = properties->complexDataSizes[ complexDataIndex ];
        uchar* cur_value_data = properties->complexData[ complexDataIndex ];

        // Compare for ignore void calls
        if( new_value_data_size == cur_value_data_size && ( cur_value_data_size == 0 || !memcmp( new_value_data, cur_value_data, cur_value_data_size ) ) )
            return;

        // Only now create temporary value for use in callbacks as old value
        // Ignore creating complex values for engine callbacks
        if( !setCallbackLocked && !setCallbacks.empty() && setCallbacksAnyOldValue )
        {
            uchar* data = properties->complexData[ complexDataIndex ];
            uint   data_size = properties->complexDataSizes[ complexDataIndex ];
            complex_value = cur_complex_value = CreateComplexValue( data, data_size );
        }
        cur_value = &cur_complex_value;

        // Copy new property data
        if( properties->complexDataSizes[ complexDataIndex ] != new_value_data_size )
        {
            SAFEDELA( properties->complexData[ complexDataIndex ] );
            properties->complexDataSizes[ complexDataIndex ] = new_value_data_size;
            if( new_value_data_size )
                properties->complexData[ complexDataIndex ] = new uchar[ new_value_data_size ];
        }
        if( new_value_data_size )
            memcpy( properties->complexData[ complexDataIndex ], new_value_data, new_value_data_size );

        // Deallocate data
        if( need_delete && new_value_data_size )
            SAFEDELA( new_value_data );
    }

    // Store old value
    uint64 old_value = 0;
    memcpy( &old_value, cur_value, baseSize );

    // Assign value
    memcpy( cur_value, new_value, baseSize );

    // Set callbacks
    if( !setCallbacks.empty() && !setCallbackLocked )
    {
        setCallbackLocked = true;

        for( size_t i = 0; i < setCallbacks.size(); i++ )
        {
            #ifndef FONLINE_SCRIPT_COMPILER
            if( Script::PrepareContext( setCallbacks[ i ], _FUNC_, GetName() ) )
            {
                if( setCallbacksArgs[ i ] > 0 )
                    Script::SetArgObject( obj );
                if( setCallbacksArgs[ i ] > 1 )
                    Script::SetArgUInt( enumValue );
                if( setCallbacksArgs[ i ] > 2 )
                    Script::SetArgAddress( &old_value );
                if( !Script::RunPrepared() )
                    break;
            }
            #endif
        }

        setCallbackLocked = false;
    }

    // Native set callback
    if( nativeSetCallback && !setCallbackLocked )
        nativeSetCallback( obj, this, cur_value, &old_value );

    // Native send callback
    if( nativeSendCallback && obj != sendIgnoreObj && !setCallbackLocked )
    {
        if( ( properties->registrator->isServer && ( accessType & ( Property::PublicMask | Property::ProtectedMask ) ) ) ||
            ( !properties->registrator->isServer && ( accessType & Property::ModifiableMask ) ) )
        {
            nativeSendCallback( obj, this, cur_value, &old_value );
        }
    }

    // Reference counting
    if( complex_value )
        ReleaseComplexValue( complex_value );
}

const char* Property::GetName()
{
    return propName.c_str();
}

uint Property::GetRegIndex()
{
    return regIndex;
}

int Property::GetEnumValue()
{
    return enumValue;
}

Property::AccessType Property::GetAccess()
{
    return accessType;
}

uint Property::GetBaseSize()
{
    return baseSize;
}

bool Property::IsPOD()
{
    return dataType == Property::POD;
}

bool Property::IsReadable()
{
    return isReadable;
}

bool Property::IsWritable()
{
    return isWritable;
}

uchar* Property::GetRawData( void* obj, uint& data_size )
{
    Properties* properties = (Properties*) obj;

    if( dataType == Property::POD )
    {
        RUNTIME_ASSERT( podDataOffset != uint( -1 ) );
        data_size = baseSize;
        return &properties->podData[ podDataOffset ];
    }

    RUNTIME_ASSERT( complexDataIndex != uint( -1 ) );
    data_size = properties->complexDataSizes[ complexDataIndex ];
    return properties->complexData[ complexDataIndex ];
}

void Property::SetRawData( void* obj, uchar* data, uint data_size )
{
    Properties* properties = (Properties*) obj;

    if( IsPOD() )
    {
        RUNTIME_ASSERT( podDataOffset != uint( -1 ) );
        RUNTIME_ASSERT( baseSize == data_size );
        memcpy( &properties->podData[ podDataOffset ], data, data_size );
    }
    else
    {
        RUNTIME_ASSERT( complexDataIndex != uint( -1 ) );
        if( data_size != properties->complexDataSizes[ complexDataIndex ] )
        {
            properties->complexDataSizes[ complexDataIndex ] = data_size;
            SAFEDELA( properties->complexData[ complexDataIndex ] );
            if( data_size )
                properties->complexData[ complexDataIndex ] = new uchar[ data_size ];
        }
        if( data_size )
            memcpy( properties->complexData[ complexDataIndex ], data, data_size );
    }
}

void Property::SetData( void* obj, uchar* data, uint data_size )
{
    Properties* properties = (Properties*) obj;

    if( dataType == Property::POD )
    {
        RUNTIME_ASSERT( data_size == baseSize );
        GenericSet( obj, data );
    }
    else
    {
        void* value = CreateComplexValue( data, data_size );
        GenericSet( obj, &value );
        ReleaseComplexValue( value );
    }
}

void Property::SetSendIgnore( void* obj )
{
    sendIgnoreObj = obj;
}

string Property::SetGetCallback( const char* script_func )
{
    #ifndef FONLINE_SCRIPT_COMPILER
    // Todo: Check can get

    char decl[ MAX_FOTEXT ];
    Str::Format( decl, "%s%s %s(%s&,%s)", typeName.c_str(), !IsPOD() ? "@" : "", "%s", registrator->scriptClassName.c_str(), registrator->enumTypeName.c_str() );
    int  bind_id = Script::Bind( script_func, decl, false, true );
    if( bind_id <= 0 )
    {
        Str::Format( decl, "%s%s %s(%s&)", typeName.c_str(), !IsPOD() ? "@" : "", "%s", registrator->scriptClassName.c_str() );
        bind_id = Script::Bind( script_func, decl, false, true );
        if( bind_id <= 0 )
        {
            char buf[ MAX_FOTEXT ];
            Str::Format( buf, decl, script_func );
            return "Unable to bind function '" + string( buf ) + "'.";
        }
        getCallbackArgs = 1;
    }
    else
    {
        getCallbackArgs = 2;
    }

    getCallback = bind_id;
    #endif
    return "";
}

string Property::AddSetCallback( const char* script_func )
{
    #ifndef FONLINE_SCRIPT_COMPILER
    // Todo: Check can set

    char decl[ MAX_FOTEXT ];
    Str::Format( decl, "void %s(%s&,%s,%s&)", "%s", registrator->scriptClassName.c_str(), registrator->enumTypeName.c_str(), typeName.c_str() );
    int  bind_id = Script::Bind( script_func, decl, false, true );
    if( bind_id <= 0 )
    {
        Str::Format( decl, "void %s(%s&,%s)", "%s", registrator->scriptClassName.c_str(), registrator->enumTypeName.c_str() );
        bind_id = Script::Bind( script_func, decl, false, true );
        if( bind_id <= 0 )
        {
            Str::Format( decl, "void %s(%s&)", "%s", registrator->scriptClassName.c_str() );
            bind_id = Script::Bind( script_func, decl, false, true );
            if( bind_id <= 0 )
            {
                char buf[ MAX_FOTEXT ];
                Str::Format( buf, decl, script_func );
                return "Unable to bind function '" + string( buf ) + "'.";
            }
            setCallbacksArgs.push_back( 1 );
        }
        else
        {
            setCallbacksArgs.push_back( 2 );
        }
    }
    else
    {
        setCallbacksArgs.push_back( 3 );
        setCallbacksAnyOldValue = true;
    }

    setCallbacks.push_back( bind_id );
    #endif
    return "";
}

Properties::Properties( PropertyRegistrator* reg )
{
    registrator = reg;
    if( !reg )
        return;
    RUNTIME_ASSERT( registrator );
    RUNTIME_ASSERT( registrator->registrationFinished );

    // Allocate POD data
    if( !registrator->podDataPool.empty() )
    {
        podData = registrator->podDataPool.back();
        registrator->podDataPool.pop_back();
    }
    else
    {
        podData = new uchar[ registrator->wholePodDataSize ];
    }
    memzero( podData, registrator->wholePodDataSize );

    // Complex data
    complexData.resize( registrator->complexPropertiesCount );
    complexDataSizes.resize( registrator->complexPropertiesCount );

    // Set default values
    for( size_t i = 0, j = registrator->registeredProperties.size(); i < j; i++ )
    {
        Property* prop = registrator->registeredProperties[ i ];
        if( prop->podDataOffset == uint( -1 ) )
            continue;

        // Todo: complex string value

        if( prop->setDefaultValue )
        {
            memcpy( &podData[ prop->podDataOffset ], &prop->defaultValue, prop->baseSize );
        }
        else if( prop->generateRandomValue )
        {
            // Todo: fix min/max
            for( uint i = 0; i < prop->baseSize; i++ )
                podData[ prop->podDataOffset + i ] = Random( 0, 255 );
        }
    }
}

Properties::~Properties()
{
    registrator->podDataPool.push_back( podData );
    podData = NULL;

    for( size_t i = 0; i < complexData.size(); i++ )
        SAFEDELA( complexData[ i ] );
    complexData.clear();
    complexDataSizes.clear();

    for( size_t i = 0; i < unresolvedProperties.size(); i++ )
    {
        SAFEDELA( unresolvedProperties[ i ]->Name );
        SAFEDELA( unresolvedProperties[ i ]->TypeName );
        SAFEDELA( unresolvedProperties[ i ]->Data );
        SAFEDEL( unresolvedProperties[ i ] );
    }
    unresolvedProperties.clear();
}

Properties& Properties::operator=( const Properties& other )
{
    RUNTIME_ASSERT( registrator == other.registrator );

    // Copy POD data
    memcpy( &podData[ 0 ], &other.podData[ 0 ], registrator->wholePodDataSize );

    // Copy complex data
    for( size_t i = 0; i < registrator->registeredProperties.size(); i++ )
    {
        Property* prop = registrator->registeredProperties[ i ];
        if( prop->complexDataIndex != uint( -1 ) )
            prop->SetRawData( this, other.complexData[ prop->complexDataIndex ], other.complexDataSizes[ prop->complexDataIndex ] );
    }

    return *this;
}

void* Properties::FindData( const char* property_name )
{
    if( !registrator )
        return 0;
    Property* prop = registrator->Find( property_name );
    RUNTIME_ASSERT( prop );
    RUNTIME_ASSERT( prop->podDataOffset != uint( -1 ) );
    return prop ? &podData[ prop->podDataOffset ] : NULL;
}

uint Properties::StoreData( bool with_protected, PUCharVec** all_data, UIntVec** all_data_sizes )
{
    uint whole_size = 0;
    *all_data = &storeData;
    *all_data_sizes = &storeDataSizes;
    storeData.resize( 0 );
    storeDataSizes.resize( 0 );

    // Store POD properties data
    storeData.push_back( podData );
    storeDataSizes.push_back( (uint) registrator->publicPodDataSpace.size() + ( with_protected ? (uint) registrator->protectedPodDataSpace.size() : 0 ) );
    whole_size += storeDataSizes.back();

    // Calculate complex data to send
    UShortVec complex_indicies = ( with_protected ? registrator->publicProtectedComplexDataProps : registrator->publicComplexDataProps );
    for( size_t i = 0; i < complex_indicies.size();)
    {
        Property* prop = registrator->registeredProperties[ complex_indicies[ i ] ];
        RUNTIME_ASSERT( prop->complexDataIndex != uint( -1 ) );
        if( !complexDataSizes[ prop->complexDataIndex ] )
            complex_indicies.erase( complex_indicies.begin() + i );
        else
            i++;
    }

    // Store complex properties data
    if( complex_indicies.empty() )
    {
        storeData.push_back( !complex_indicies.empty() ? (uchar*) &complex_indicies[ 0 ] : NULL );
        storeDataSizes.push_back( (uint) complex_indicies.size() * sizeof( short ) );
        whole_size += storeDataSizes.back();
        for( size_t i = 0; i < complex_indicies.size(); i++ )
        {
            Property* prop = registrator->registeredProperties[ complex_indicies[ i ] ];
            storeData.push_back( complexData[ prop->complexDataIndex ] );
            storeDataSizes.push_back( complexDataSizes[ prop->complexDataIndex ] );
            whole_size += storeDataSizes.back();
        }
    }
    return whole_size;
}

void Properties::RestoreData( PUCharVec& all_data, UIntVec& all_data_sizes )
{
    // Restore POD data
    memcpy( &podData[ 0 ], all_data[ 0 ], all_data_sizes[ 0 ] );

    // Restore complex data
    if( all_data.size() > 1 )
    {
        UShortVec complex_indicies( all_data_sizes[ 1 ] / sizeof( ushort ) );
        memcpy( &complex_indicies[ 0 ], all_data[ 1 ], all_data_sizes[ 1 ] );

        for( size_t i = 0; i < complex_indicies.size(); i++ )
        {
            RUNTIME_ASSERT( complex_indicies[ i ] < registrator->registeredProperties.size() );
            Property* prop = registrator->registeredProperties[ complex_indicies[ i ] ];
            RUNTIME_ASSERT( prop->complexDataIndex != uint( -1 ) );
            uint      data_size = all_data_sizes[ 2 + i ];
            uchar*    data = all_data[ 2 + i ];
            prop->SetRawData( this, data, data_size );
        }
    }
}

void Properties::RestoreData( UCharVecVec& all_data )
{
    PUCharVec all_data_ext( all_data.size() );
    UIntVec   all_data_sizes( all_data.size() );
    for( size_t i = 0; i < all_data.size(); i++ )
    {
        all_data_ext[ i ] = &all_data[ i ][ 0 ];
        all_data_sizes[ i ] = (uint) all_data[ i ].size();
    }
    RestoreData( all_data_ext, all_data_sizes );
}

void Properties::Save( void ( * save_func )( void*, size_t ) )
{
    RUNTIME_ASSERT( registrator->registrationFinished );

    uint count = registrator->serializedPropertiesCount + (uint) unresolvedProperties.size();
    save_func( &count, sizeof( count ) );
    for( size_t i = 0, j = registrator->registeredProperties.size(); i < j; i++ )
    {
        Property* prop = registrator->registeredProperties[ i ];
        if( prop->podDataOffset == uint( -1 ) && prop->complexDataIndex == uint( -1 ) )
            continue;

        ushort name_len = (ushort) prop->propName.length();
        save_func( &name_len, sizeof( name_len ) );
        save_func( (void*) prop->propName.c_str(), name_len );

        uchar type_name_len = (uchar) prop->typeName.length();
        save_func( &type_name_len, sizeof( type_name_len ) );
        save_func( (void*) prop->typeName.c_str(), type_name_len );

        uint   data_size;
        uchar* data = prop->GetRawData( this, data_size );
        save_func( &data_size, sizeof( data_size ) );
        if( data_size )
            save_func( data, data_size );
    }
    for( size_t i = 0, j = unresolvedProperties.size(); i < j; i++ )
    {
        UnresolvedProperty* unresolved_property = unresolvedProperties[ i ];

        ushort              name_len = ( ushort ) Str::Length( unresolved_property->Name );
        save_func( &name_len, sizeof( name_len ) );
        save_func( (void*) unresolved_property->Name, name_len );

        uchar type_name_len = ( uchar ) Str::Length( unresolved_property->TypeName );
        save_func( &type_name_len, sizeof( type_name_len ) );
        save_func( (void*) unresolved_property->TypeName, type_name_len );

        uint   data_size = unresolved_property->DataSize;
        uchar* data = unresolved_property->Data;
        save_func( &data_size, sizeof( data_size ) );
        if( data_size )
            save_func( data, data_size );
    }
}

void Properties::Load( void* file, uint version )
{
    RUNTIME_ASSERT( registrator->registrationFinished );

    UCharVec data;
    uint     count = 0;
    FileRead( file, &count, sizeof( count ) );
    for( uint i = 0; i < count; i++ )
    {
        ushort name_len = 0;
        FileRead( file, &name_len, sizeof( name_len ) );
        char   name[ MAX_FOTEXT ];
        FileRead( file, name, name_len );
        name[ name_len ] = 0;

        uchar type_name_len = 0;
        FileRead( file, &type_name_len, sizeof( type_name_len ) );
        char  type_name[ MAX_FOTEXT ];
        FileRead( file, type_name, type_name_len );
        type_name[ type_name_len ] = 0;

        uint data_size = 0;
        FileRead( file, &data_size, sizeof( data_size ) );
        data.resize( data_size );
        if( data_size )
            FileRead( file, &data[ 0 ], data_size );

        Property* prop = registrator->Find( name );
        if( prop && Str::Compare( prop->typeName.c_str(), type_name ) )
        {
            prop->SetRawData( this, data_size ? &data[ 0 ] : NULL, data_size );
        }
        else
        {
            UnresolvedProperty* unresolved_property = new UnresolvedProperty();
            unresolved_property->Name = Str::Duplicate( name );
            unresolved_property->TypeName = Str::Duplicate( type_name );
            unresolved_property->DataSize = data_size;
            unresolved_property->Data = ( data_size ? new uchar[ data_size ] : NULL );
            if( data_size )
                memcpy( unresolved_property->Data, &data[ 0 ], data_size );
            unresolvedProperties.push_back( unresolved_property );
        }
    }
}

hash Properties::LoadFromText( const char* text )
{
    bool      is_error = false;
    hash      pid = 0;
    IniParser ini;
    ini.LoadFilePtr( text, Str::Length( text ) );
    ini.CacheKeys();
    StrSet& keys = ini.GetCachedKeys();
    for( auto it = keys.begin(); it != keys.end(); ++it )
    {
        const char* key = it->c_str();
        char        value[ MAX_FOTEXT ];
        ini.GetStr( key, "", value );

        if( !pid && Str::Compare( key, "ProtoId" ) )
        {
            pid = (hash) ConvertParamValue( value );
            continue;
        }

        Property* prop = registrator->Find( key );
        if( !prop )
        {
            WriteLog( "Property '%s' not found.\n", key );
            is_error = true;
            continue;
        }

        if( prop->IsPOD() )
        {
            uchar pod[ 8 ];
            if( prop->isEnumDataType )
            {
                int              enum_value = 0;
                bool             enum_found = false;
                asIScriptEngine* engine = Script::GetEngine();
                for( asUINT i = 0, j = engine->GetEnumCount(); i < j; i++ )
                {
                    int         enum_type_id;
                    const char* enum_name = engine->GetEnumByIndex( i, &enum_type_id );
                    if( Str::Compare( enum_name, prop->typeName.c_str() ) )
                    {
                        for( asUINT k = 0, l = engine->GetEnumValueCount( enum_type_id ); k < l; k++ )
                        {
                            const char* enum_value_name = engine->GetEnumValueByIndex( enum_type_id, k, &enum_value );
                            if( Str::Compare( enum_value_name, value ) )
                            {
                                enum_found = true;
                                break;
                            }
                        }
                        break;
                    }
                }
                if( !enum_found )
                {
                    WriteLog( "Value '%s' of enum '%s' in property '%s' not found.\n", value, prop->typeName.c_str(), key );
                    is_error = true;
                }
                *(int*) pod = enum_value;
            }
            else
            {
                if( prop->baseSize == 1 )
                    *(char*) pod = (char) ConvertParamValue( value );
                else if( prop->baseSize == 2 )
                    *(short*) pod = (short) ConvertParamValue( value );
                else if( prop->baseSize == 4 )
                    *(int*) pod = (int) ConvertParamValue( value );
                else if( prop->baseSize == 8 )
                    *(int64*) pod = (int64) ConvertParamValue( value );
                else
                    RUNTIME_ASSERT( !"Unreachable place" );
            }

            prop->SetRawData( this, pod, prop->baseSize );
        }
        else if( prop->dataType == Property::String )
        {
            prop->SetRawData( this, (uchar*) value, Str::Length( value ) );
        }
        else
        {
            WriteLog( "Property '%s' data type '%s' serialization is not supported.\n", key, prop->typeName.c_str() );
            is_error = true;
        }
    }
    return !is_error ? pid : 0;
}

int Properties::GetValueAsInt( int enum_value )
{
    Property* prop = registrator->FindByEnum( enum_value );
    if( !prop )
        SCRIPT_ERROR_R0( "Enum '%d' not found", enum_value );
    if( !prop->IsPOD() )
        SCRIPT_ERROR_R0( "Can't retreive integer value from non POD property '%s'", prop->GetName() );
    if( !prop->IsReadable() )
        SCRIPT_ERROR_R0( "Can't retreive integer value from non readable property '%s'", prop->GetName() );

    if( prop->baseSize == 1 )
        return (int) prop->GetValue< char >( this );
    if( prop->baseSize == 2 )
        return (int) prop->GetValue< short >( this );
    if( prop->baseSize == 4 )
        return (int) prop->GetValue< int >( this );
    if( prop->baseSize == 8 )
        return (int) prop->GetValue< int64 >( this );
    RUNTIME_ASSERT( !"Unreachable place" );
    return 0;
}

void Properties::SetValueAsInt( int enum_value, int value )
{
    Property* prop = registrator->FindByEnum( enum_value );
    if( !prop )
        SCRIPT_ERROR_R( "Enum '%d' not found", enum_value );
    if( !prop->IsPOD() )
        SCRIPT_ERROR_R( "Can't set integer value to non POD property '%s'", prop->GetName() );
    if( !prop->IsWritable() )
        SCRIPT_ERROR_R( "Can't set integer value to non writable property '%s'", prop->GetName() );

    if( prop->isBoolDataType )
        prop->SetValue< char >( this, value != 0 ? 1 : 0 );
    else if( prop->baseSize == 1 )
        prop->SetValue< char >( this, (char) value );
    else if( prop->baseSize == 2 )
        prop->SetValue< short >( this, (short) value );
    else if( prop->baseSize == 4 )
        prop->SetValue< int >( this, (int) value );
    else if( prop->baseSize == 8 )
        prop->SetValue< int64 >( this, (int64) value );
    else
        RUNTIME_ASSERT( !"Unreachable place" );
}

bool Properties::SetValueAsIntByName( const char* enum_name, int value )
{
    Property* prop = registrator->Find( enum_name );
    if( !prop )
        SCRIPT_ERROR_R0( "Enum '%s' not found", enum_name );
    if( !prop->IsPOD() )
        SCRIPT_ERROR_R0( "Can't set by name integer value from non POD property '%s'", prop->GetName() );

    if( prop->baseSize == 1 )
        prop->SetValue< char >( this, (char) value );
    else if( prop->baseSize == 2 )
        prop->SetValue< short >( this, (short) value );
    else if( prop->baseSize == 4 )
        prop->SetValue< int >( this, (int) value );
    else if( prop->baseSize == 8 )
        prop->SetValue< int64 >( this, (int64) value );
    else
        RUNTIME_ASSERT( !"Unreachable place" );
    return true;
}

PropertyRegistrator::PropertyRegistrator( bool is_server, const char* script_class_name )
{
    registrationFinished = false;
    isServer = is_server;
    scriptClassName = script_class_name;
    wholePodDataSize = 0;
    complexPropertiesCount = 0;
    serializedPropertiesCount = 0;
    defaultGroup = NULL;
    defaultGenerateRandomValue = NULL;
    defaultDefaultValue = NULL;
    defaultMinValue = NULL;
    defaultMaxValue = NULL;
}

PropertyRegistrator::~PropertyRegistrator()
{
    scriptClassName = "";
    wholePodDataSize = 0;

    for( size_t i = 0; i < registeredProperties.size(); i++ )
    {
        SAFEREL( registeredProperties[ i ]->asObjType );
        SAFEDEL( registeredProperties[ i ] );
    }
    registeredProperties.clear();

    for( size_t i = 0; i < podDataPool.size(); i++ )
        delete[] podDataPool[ i ];
    podDataPool.clear();

    SetDefaults();
}

Property* PropertyRegistrator::Register(
    const char* type_name,
    const char* name,
    Property::AccessType access,
    const char* group /* = NULL */,
    bool* generate_random_value /* = NULL */,
    int64* default_value /* = NULL */,
    int64* min_value /* = NULL */,
    int64* max_value /* = NULL */
    )
{
    if( registrationFinished )
    {
        WriteLogF( _FUNC_, " - Registration of class properties is finished.\n" );
        return NULL;
    }

    // Check defaults
    group = ( group ? group : defaultGroup );
    generate_random_value = ( generate_random_value ? generate_random_value : defaultGenerateRandomValue );
    default_value = ( default_value ? default_value : defaultDefaultValue );
    min_value = ( min_value ? min_value : defaultMinValue );
    max_value = ( max_value ? max_value : defaultMaxValue );

    // Get engine
    asIScriptEngine* engine = Script::GetEngine();
    RUNTIME_ASSERT( engine );

    // Extract type
    int type_id = engine->GetTypeIdByDecl( type_name );
    if( type_id < 0 )
    {
        WriteLogF( _FUNC_, " - Invalid type<%s>.\n", type_name );
        return NULL;
    }

    Property::DataType data_type;
    uint               data_size = 0;
    asIObjectType*     as_obj_type = engine->GetObjectTypeById( type_id );
    bool               is_bool_data_type = false;
    bool               is_enum_data_type = false;
    if( type_id & asTYPEID_OBJHANDLE )
    {
        WriteLogF( _FUNC_, " - Invalid property type<%s>, handles not allowed.\n", type_name );
        return NULL;
    }
    else if( !( type_id & asTYPEID_MASK_OBJECT ) )
    {
        data_type = Property::POD;

        int type_id = engine->GetTypeIdByDecl( type_name );
        int primitive_size = engine->GetSizeOfPrimitiveType( type_id );
        if( primitive_size <= 0 )
        {
            WriteLogF( _FUNC_, " - Invalid property POD type<%s>.\n", type_name );
            return NULL;
        }

        data_size = (uint) primitive_size;
        if( data_size != 1 && data_size != 2 && data_size != 4 && data_size != 8 )
        {
            WriteLogF( _FUNC_, " - Invalid size of property POD type<%s>, size<%d>.\n", type_name, data_size );
            return NULL;
        }

        is_bool_data_type = ( type_id == asTYPEID_BOOL );
        is_enum_data_type = ( type_id > asTYPEID_DOUBLE );
    }
    else if( Str::Compare( as_obj_type->GetName(), "string" ) )
    {
        data_type = Property::String;

        data_size = sizeof( void* );
    }
    else if( Str::Compare( as_obj_type->GetName(), "array" ) )
    {
        data_type = Property::Array;

        data_size = sizeof( void* );
        if( as_obj_type->GetSubTypeId() & asTYPEID_MASK_OBJECT )
        {
            WriteLogF( _FUNC_, " - Invalid property type<%s>, array elements must have POD type.\n", type_name );
            return NULL;
        }
    }
    else
    {
        WriteLogF( _FUNC_, " - Invalid property type<%s>.\n", type_name );
        return NULL;
    }

    // Check name for already used
    asIObjectType* ot = engine->GetObjectTypeByName( scriptClassName.c_str() );
    RUNTIME_ASSERT( ot );
    for( asUINT i = 0, j = ot->GetPropertyCount(); i < j; i++ )
    {
        const char* n;
        ot->GetProperty( i, &n );
        if( Str::Compare( n, name ) )
        {
            WriteLogF( _FUNC_, " - Trying to register already registered property<%s>.\n", name );
            return NULL;
        }
    }

    // Allocate property
    Property* prop = new Property();
    uint      reg_index = (uint) registeredProperties.size();

    // Disallow set or get accessors
    bool disable_get = false;
    bool disable_set = false;
    if( access & Property::VirtualMask )
        disable_set = true;
    if( isServer && ( access & Property::ClientMask ) )
        disable_get = disable_set = true;
    if( !isServer && ( access & Property::ServerMask ) )
        disable_get = disable_set = true;
    if( !isServer && ( ( access & Property::PublicMask ) || ( access & Property::ProtectedMask ) ) && !( access & Property::ModifiableMask ) )
        disable_set = true;

    // Register common stuff on first property registration
    string enum_type = ( scriptClassName.find( "Cl" ) != string::npos ? scriptClassName.substr( 0, scriptClassName.size() - 2 ) : scriptClassName ) + "Property";
    if( registeredProperties.empty() )
    {
        enumTypeName = enum_type;

        int result = engine->RegisterEnum( enum_type.c_str() );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object property enum<%s> fail, error<%d>.\n", enum_type.c_str(), result );
            return NULL;
        }

        result = engine->RegisterEnumValue( enum_type.c_str(), "Invalid", 0 );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object property enum<%s::%s> zero value fail, error<%d>.\n", enum_type.c_str(), name, result );
            return NULL;
        }

        char decl[ MAX_FOTEXT ];
        Str::Format( decl, "int GetAsInt(%s)", enum_type.c_str() );
        result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHOD( Properties, GetValueAsInt ), asCALL_THISCALL );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object method<%s> fail, error<%d>.\n", decl, result );
            return NULL;
        }

        Str::Format( decl, "void SetAsInt(%s,int)", enum_type.c_str() );
        result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHOD( Properties, SetValueAsInt ), asCALL_THISCALL );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object method<%s> fail, error<%d>.\n", decl, result );
            return NULL;
        }
    }

    // Register default getter
    if( !disable_get )
    {
        char decl[ MAX_FOTEXT ];
        Str::Format( decl, "const %s%s get_%s() const", type_name, data_type != Property::POD ? "@" : "", name );
        int  result = -1;
        if( data_type != Property::POD )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, GetValue< void* >, (void*), void* ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 1 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, GetValue< char >, (void*), char ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 2 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, GetValue< short >, (void*), short ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 4 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, GetValue< int >, (void*), int ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 8 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, GetValue< int64 >, (void*), int64 ), asCALL_THISCALL_OBJFIRST, prop );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object property<%s> getter fail, error<%d>.\n", name, result );
            return NULL;
        }
    }

    // Register setter
    if( !disable_set )
    {
        char decl[ MAX_FOTEXT ];
        Str::Format( decl, "void set_%s(%s%s)", name, type_name, data_type != Property::POD ? "&" : "" );
        int  result = -1;
        if( data_type != Property::POD )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, SetValue< void* >, ( void*, void* ), void ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 1 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, SetValue< char >, ( void*, char ), void ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 2 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, SetValue< short >, ( void*, short ), void ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 4 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, SetValue< int >, ( void*, int ), void ), asCALL_THISCALL_OBJFIRST, prop );
        else if( data_size == 8 )
            result = engine->RegisterObjectMethod( scriptClassName.c_str(), decl, asMETHODPR( Property, SetValue< int64 >, ( void*, int64 ), void ), asCALL_THISCALL_OBJFIRST, prop );
        if( result < 0 )
        {
            WriteLogF( _FUNC_, " - Register object property<%s> setter fail, error<%d>.\n", name, result );
            return NULL;
        }
    }

    // Register enum values for property reflection
    char enum_value_name[ MAX_FOTEXT ];
    Str::Format( enum_value_name, "%s::%s", enum_type.c_str(), name );
    int  enum_value = (int) Str::GetHash( enum_value_name );
    int  result = engine->RegisterEnumValue( enum_type.c_str(), name, enum_value );
    if( result < 0 )
    {
        WriteLogF( _FUNC_, " - Register object property enum<%s::%s> value<%d> fail, error<%d>.\n", enum_type.c_str(), name, enum_value, result );
        return NULL;
    }

    // Add property to group
    if( group )
    {
        char full_decl[ MAX_FOTEXT ];
        Str::Format( full_decl, "const %s[] %s%s", enum_type.c_str(), enum_type.c_str(), group );

        ScriptArray* group_array = NULL;
        if( enumGroups.count( full_decl ) )
            group_array = enumGroups[ full_decl ];

        if( !group_array )
        {
            char           decl[ MAX_FOTEXT ];
            Str::Format( decl, "%s[]", enum_type.c_str() );
            asIObjectType* enum_array_type = engine->GetObjectTypeByDecl( decl );
            if( !enum_array_type )
            {
                WriteLogF( _FUNC_, " - Invalid type for property group<%s>.\n", decl );
                return NULL;
            }

            group_array = ScriptArray::Create( enum_array_type );
            if( !enum_array_type )
            {
                WriteLogF( _FUNC_, " - Can not create array type for property group<%s>.\n", decl );
                return NULL;
            }

            int result = engine->RegisterGlobalProperty( full_decl, group_array );
            if( result < 0 )
            {
                WriteLogF( _FUNC_, " - Register object property group<%s> fail, error<%d>.\n", full_decl, result );
                return NULL;
            }

            enumGroups.insert( PAIR( string( full_decl ), group_array ) );
        }

        group_array->InsertLast( &enum_value );
    }

    // POD property data offset
    uint data_base_offset = uint( -1 );
    if( data_type == Property::POD && !disable_get && !( access & Property::VirtualMask ) )
    {
        bool     is_public = ( access & Property::PublicMask ) != 0;
        bool     is_protected = ( access & Property::ProtectedMask ) != 0;
        BoolVec& space = ( is_public ? publicPodDataSpace : ( is_protected ? protectedPodDataSpace : privatePodDataSpace ) );

        uint     space_size = (uint) space.size();
        uint     space_pos = 0;
        while( space_pos + data_size <= space_size )
        {
            bool fail = false;
            for( uint i = 0; i < data_size; i++ )
            {
                if( space[ space_pos + i ] )
                {
                    fail = true;
                    break;
                }
            }
            if( !fail )
                break;
            space_pos += data_size;
        }

        uint new_size = space_pos + data_size;
        new_size += 8 - new_size % 8;
        if( new_size > (uint) space.size() )
            space.resize( new_size );

        for( uint i = 0; i < data_size; i++ )
            space[ space_pos + i ] = true;

        data_base_offset = space_pos;

        wholePodDataSize = (uint) ( publicPodDataSpace.size() + protectedPodDataSpace.size() + privatePodDataSpace.size() );
        RUNTIME_ASSERT( !( wholePodDataSize % 8 ) );
    }

    // Complex property data index
    uint complex_data_index = uint( -1 );
    if( data_type != Property::POD && ( !disable_get || !disable_set ) && !( access & Property::VirtualMask ) )
    {
        complex_data_index = complexPropertiesCount++;
        if( access & Property::PublicMask )
        {
            publicComplexDataProps.push_back( (ushort) reg_index );
            publicProtectedComplexDataProps.push_back( (ushort) reg_index );
        }
        else if( access & Property::ProtectedMask )
        {
            protectedComplexDataProps.push_back( (ushort) reg_index );
            publicProtectedComplexDataProps.push_back( (ushort) reg_index );
        }
        else if( access & Property::PrivateMask )
        {
            privateComplexDataProps.push_back( (ushort) reg_index );
        }
        else
        {
            RUNTIME_ASSERT( !"Unreachable place" );
        }
    }

    // Make entry
    prop->registrator = this;
    prop->regIndex = reg_index;
    prop->enumValue = enum_value;
    prop->complexDataIndex = complex_data_index;
    prop->podDataOffset = data_base_offset;
    prop->baseSize = data_size;
    prop->getCallback = 0;
    prop->getCallbackArgs = 0;
    prop->nativeSetCallback = NULL;

    prop->propName = name;
    prop->typeName = type_name;
    prop->dataType = data_type;
    prop->accessType = access;
    prop->asObjType = as_obj_type;
    prop->isEnumDataType = is_enum_data_type;
    prop->isBoolDataType = is_bool_data_type;
    prop->isReadable = !disable_get;
    prop->isWritable = !disable_set;
    prop->generateRandomValue = ( generate_random_value ? *generate_random_value : false );
    prop->setDefaultValue = ( default_value != NULL );
    prop->checkMinValue = ( min_value != NULL );
    prop->checkMaxValue = ( max_value != NULL );
    prop->defaultValue = ( default_value ? *default_value : 0 );
    prop->minValue = ( min_value ? *min_value : 0 );
    prop->maxValue = ( max_value ? *max_value : 0 );

    registeredProperties.push_back( prop );

    if( prop->asObjType )
        prop->asObjType->AddRef();

    if( prop->podDataOffset != uint( -1 ) || prop->complexDataIndex != uint( -1 ) )
        serializedPropertiesCount++;

    return prop;
}

void PropertyRegistrator::SetDefaults(
    const char* group /* = NULL */,
    bool* generate_random_value /* = NULL */,
    int64* default_value /* = NULL */,
    int64* min_value /* = NULL */,
    int64* max_value     /* = NULL */
    )
{
    SAFEDELA( defaultGroup );
    SAFEDEL( defaultGenerateRandomValue );
    SAFEDEL( defaultDefaultValue );
    SAFEDEL( defaultMinValue );
    SAFEDEL( defaultMaxValue );

    if( group )
        defaultGroup = Str::Duplicate( group );
    if( generate_random_value )
        defaultGenerateRandomValue = new bool(*generate_random_value);
    if( default_value )
        defaultDefaultValue = new int64( *default_value );
    if( min_value )
        defaultMinValue = new int64( *min_value );
    if( max_value )
        defaultMaxValue = new int64( *max_value );
}

void PropertyRegistrator::FinishRegistration()
{
    RUNTIME_ASSERT( !registrationFinished );
    registrationFinished = true;

    // Fix POD data offsets
    for( size_t i = 0, j = registeredProperties.size(); i < j; i++ )
    {
        Property* prop = registeredProperties[ i ];
        if( prop->podDataOffset == uint( -1 ) )
            continue;

        if( prop->accessType & Property::ProtectedMask )
            prop->podDataOffset += (uint) publicPodDataSpace.size();
        else if( prop->accessType & Property::PrivateMask )
            prop->podDataOffset += (uint) publicPodDataSpace.size() + (uint) protectedPodDataSpace.size();
    }
}

Property* PropertyRegistrator::Get( uint property_index )
{
    if( property_index < (uint) registeredProperties.size() )
        return registeredProperties[ property_index ];
    return NULL;
}

Property* PropertyRegistrator::Find( const char* property_name )
{
    for( size_t i = 0, j = registeredProperties.size(); i < j; i++ )
        if( Str::Compare( property_name, registeredProperties[ i ]->propName.c_str() ) )
            return registeredProperties[ i ];
    return NULL;
}

Property* PropertyRegistrator::FindByEnum( int enum_value )
{
    for( size_t i = 0, j = registeredProperties.size(); i < j; i++ )
        if( registeredProperties[ i ]->enumValue == enum_value )
            return registeredProperties[ i ];
    return NULL;
}

void PropertyRegistrator::SetNativeSetCallback( const char* property_name, NativeCallback callback )
{
    if( !property_name )
    {
        for( size_t i = 0, j = registeredProperties.size(); i < j; i++ )
            registeredProperties[ i ]->nativeSetCallback = callback;
    }
    else
    {
        Find( property_name )->nativeSetCallback = callback;
    }
}

void PropertyRegistrator::SetNativeSendCallback( NativeCallback callback )
{
    for( size_t i = 0, j = registeredProperties.size(); i < j; i++ )
        registeredProperties[ i ]->nativeSendCallback = callback;
}

uint PropertyRegistrator::GetWholeDataSize()
{
    return wholePodDataSize;
}
