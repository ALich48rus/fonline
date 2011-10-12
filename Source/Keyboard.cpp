#include "StdAfx.h"
#include "Keyboard.h"
#include <strstream>

namespace Keyb
{
    struct KeybData
    {
        bool IsAviable;
        char Rus, RusShift, Eng, EngShift;
        bool operator==( const char& ch ) { return IsAviable && ( Rus == ch || RusShift == ch || Eng == ch || EngShift == ch ); }
        KeybData( char rus, char rus_shift, char eng, char eng_shift )
        {
            IsAviable = true;
            Rus = rus;
            RusShift = rus_shift;
            Eng = eng;
            EngShift = eng_shift;
        }
        KeybData()
        {
            IsAviable = false;
            Rus = 0;
            RusShift = 0;
            Eng = 0;
            EngShift = 0;
        }
    };
    typedef vector< KeybData > KeybDataVec;

    KeybDataVec Data;
    int         Lang = LANG_ENG;
    bool        ShiftDwn = false;
    bool        CtrlDwn = false;
    bool        AltDwn = false;
    bool        KeyPressed[ 0x100 ] = { 0 };
    int         KeysMap[ 0x100 ] = { 0 };
}

void Keyb::InitKeyb()
{
    Data.resize( 0x100 );
    Data[ DIK_1 ] =                    KeybData( '1', '!', '1', '!' );
    Data[ DIK_2 ] =                    KeybData( '2', '"', '2', '@' );
    Data[ DIK_3 ] =                    KeybData( '3', '�', '3', '#' );
    Data[ DIK_4 ] =                    KeybData( '4', ';', '4', '$' );
    Data[ DIK_5 ] =                    KeybData( '5', '%', '5', '%' );
    Data[ DIK_6 ] =                    KeybData( '6', ':', '6', '^' );
    Data[ DIK_7 ] =                    KeybData( '7', '?', '7', '&' );
    Data[ DIK_8 ] =                    KeybData( '8', '*', '8', '*' );
    Data[ DIK_9 ] =                    KeybData( '9', '(', '9', '(' );
    Data[ DIK_0 ] =                    KeybData( '0', ')', '0', ')' );
    Data[ DIK_MINUS ] =                KeybData( '-', '_', '-', '_' );
    Data[ DIK_EQUALS ] =               KeybData( '=', '+', '=', '+' );
    Data[ DIK_Q ] =                    KeybData( '�', '�', 'q', 'Q' );
    Data[ DIK_W ] =                    KeybData( '�', '�', 'w', 'W' );
    Data[ DIK_E ] =                    KeybData( '�', '�', 'e', 'E' );
    Data[ DIK_R ] =                    KeybData( '�', '�', 'r', 'R' );
    Data[ DIK_T ] =                    KeybData( '�', '�', 't', 'T' );
    Data[ DIK_Y ] =                    KeybData( '�', '�', 'y', 'Y' );
    Data[ DIK_U ] =                    KeybData( '�', '�', 'u', 'U' );
    Data[ DIK_I ] =                    KeybData( '�', '�', 'i', 'I' );
    Data[ DIK_O ] =                    KeybData( '�', '�', 'o', 'O' );
    Data[ DIK_P ] =                    KeybData( '�', '�', 'p', 'P' );
    Data[ DIK_LBRACKET ] =             KeybData( '�', '�', '[', '{' );
    Data[ DIK_RBRACKET ] =             KeybData( '�', '�', ']', '}' );
    Data[ DIK_A ] =                    KeybData( '�', '�', 'a', 'A' );
    Data[ DIK_S ] =                    KeybData( '�', '�', 's', 'S' );
    Data[ DIK_D ] =                    KeybData( '�', '�', 'd', 'D' );
    Data[ DIK_F ] =                    KeybData( '�', '�', 'f', 'F' );
    Data[ DIK_G ] =                    KeybData( '�', '�', 'g', 'G' );
    Data[ DIK_H ] =                    KeybData( '�', '�', 'h', 'H' );
    Data[ DIK_J ] =                    KeybData( '�', '�', 'j', 'J' );
    Data[ DIK_K ] =                    KeybData( '�', '�', 'k', 'K' );
    Data[ DIK_L ] =                    KeybData( '�', '�', 'l', 'L' );
    Data[ DIK_SEMICOLON ] =    KeybData( '�', '�', ';', ':' );
    Data[ DIK_APOSTROPHE ] =   KeybData( '�', '�', '\'', '\"' );
    Data[ DIK_Z ] =                    KeybData( '�', '�', 'z', 'Z' );
    Data[ DIK_X ] =                    KeybData( '�', '�', 'x', 'X' );
    Data[ DIK_C ] =                    KeybData( '�', '�', 'c', 'C' );
    Data[ DIK_V ] =                    KeybData( '�', '�', 'v', 'V' );
    Data[ DIK_B ] =                    KeybData( '�', '�', 'b', 'B' );
    Data[ DIK_N ] =                    KeybData( '�', '�', 'n', 'N' );
    Data[ DIK_M ] =                    KeybData( '�', '�', 'm', 'M' );
    Data[ DIK_COMMA ] =                KeybData( '�', '�', ',', '<' );
    Data[ DIK_PERIOD ] =               KeybData( '�', '�', '.', '>' );
    Data[ DIK_SLASH ] =                KeybData( '.', ',', '/', '?' );
    Data[ DIK_MULTIPLY ] =             KeybData( '*', '*', '*', '*' );
    Data[ DIK_SPACE ] =                KeybData( ' ', ' ', ' ', ' ' );
    Data[ DIK_GRAVE ] =                KeybData( '�', '�', '`', '~' );
    Data[ DIK_NUMPAD1 ] =              KeybData( '1', '1', '1', '1' );
    Data[ DIK_NUMPAD2 ] =              KeybData( '2', '2', '2', '2' );
    Data[ DIK_NUMPAD3 ] =              KeybData( '3', '3', '3', '3' );
    Data[ DIK_NUMPAD4 ] =              KeybData( '4', '4', '4', '4' );
    Data[ DIK_NUMPAD5 ] =              KeybData( '5', '5', '5', '5' );
    Data[ DIK_NUMPAD6 ] =              KeybData( '6', '6', '6', '6' );
    Data[ DIK_NUMPAD7 ] =              KeybData( '7', '7', '7', '7' );
    Data[ DIK_NUMPAD8 ] =              KeybData( '8', '8', '8', '8' );
    Data[ DIK_NUMPAD9 ] =              KeybData( '9', '9', '9', '9' );
    Data[ DIK_NUMPAD0 ] =              KeybData( '0', '0', '0', '0' );
    Data[ DIK_SUBTRACT ] =             KeybData( '-', '-', '-', '-' );
    Data[ DIK_ADD ] =                  KeybData( '+', '+', '+', '+' );
    Data[ DIK_DECIMAL ] =              KeybData( '.', '.', '.', '.' );
    Data[ DIK_NUMPADEQUALS ] = KeybData( '=', '=', '=', '=' );
    Data[ DIK_NUMPADCOMMA ] =  KeybData( ',', ',', ',', ',' );
    Data[ DIK_DIVIDE ] =               KeybData( '/', '/', '/', '/' );
    Data[ DIK_RETURN ] =               KeybData( '\n', '\n', '\n', '\n' );
    Data[ DIK_NUMPADENTER ] =  KeybData( '\n', '\n', '\n', '\n' );
    Data[ DIK_TAB ] =                  KeybData( '\t', '\t', '\t', '\t' );
    Data[ DIK_BACKSLASH ] =    KeybData( '\\', '\\', '\\', '\\' );

    // Keys remapping
    for( int i = 0; i < 0x100; i++ )
        KeysMap[ i ] = i;
    istrstream str( GameOpt.KeyboardRemap.c_str() );
    while( !str.eof() )
    {
        int from, to;
        str >> from >> to;
        if( str.fail() )
            break;
        from &= 0xFF;
        to &= 0xFF;
        KeysMap[ from ] = to;
    }
}

void Keyb::Finish()
{
    Data.clear();
}

void Keyb::Lost()
{
    CtrlDwn = false;
    AltDwn = false;
    ShiftDwn = false;
    ZeroMemory( KeyPressed, sizeof( KeyPressed ) );
}

void Keyb::GetChar( uchar dik, string& str, int* position, int max, int flags )
{
    char str2[ 0x4000 ];
    Str::Copy( str2, str.c_str() );
    Keyb::GetChar( dik, str2, position, max, flags );
    str = str2;
}

void Keyb::GetChar( uchar dik, char* str, int* position, int max, int flags )
{
    if( AltDwn )
        return;
    bool ctrl_shift = ( CtrlDwn || ShiftDwn );

    int  len = (int) strlen( str );
    int  posit_ = len;
    int& posit = ( position ? *position : posit_ );
    if( posit > len )
        posit = len;

    // Controls
    if( dik == DIK_RIGHT && !ctrl_shift )
    {
        if( str[ posit ] )
            posit++;
    }
    else if( dik == DIK_LEFT && !ctrl_shift )
    {
        if( posit )
            posit--;
    }
    else if( dik == DIK_BACK && !ctrl_shift )
    {
        if( !len || !posit )
            return;
        posit--;
        for( int i = posit; str[ i ]; i++ )
            if( str[ i + 1 ] )
                str[ i ] = str[ i + 1 ];
        str[ len - 1 ] = '\0';
    }
    else if( dik == DIK_HOME && !ctrl_shift )
    {
        posit = 0;
    }
    else if( dik == DIK_END && !ctrl_shift )
    {
        posit = len;
    }
    else if( dik == DIK_PAUSE && !ctrl_shift )
    {
        PuntoSwitch( str );
        if( Lang == LANG_RUS )
            Lang = LANG_ENG;
        else if( Lang == LANG_ENG )
            Lang = LANG_RUS;
    }
    else if( dik == DIK_DELETE && !ctrl_shift )
    {
        if( !len || posit == len )
            return;
        for( int i = posit; str[ i ]; i++ )
            if( str[ i + 1 ] )
                str[ i ] = str[ i + 1 ];
        str[ len - 1 ] = '\0';
    }
    // Clipboard
    else if( CtrlDwn && !ShiftDwn && len > 0 && ( dik == DIK_C || dik == DIK_X ) && OpenClipboard( NULL ) )
    {
        EmptyClipboard();
        HGLOBAL hg = GlobalAlloc( GMEM_MOVEABLE, len + 1 );
        if( hg )
        {
            void* p = GlobalLock( hg );
            memcpy( p, str, len + 1 );
            GlobalUnlock( hg );
            SetClipboardData( CF_TEXT, hg );
        }
        CloseClipboard();

        if( dik == DIK_X )
        {
            *str = '\0';
            posit = 0;
        }
    }
    else if( CtrlDwn && !ShiftDwn && dik == DIK_V && len < max && OpenClipboard( NULL ) )
    {
        HANDLE h = GetClipboardData( CF_OEMTEXT );
        if( h )
        {
            char* cb = Str::Duplicate( (char*) h );
            if( !cb )
                return;
            OemToChar( cb, cb );
            EraseInvalidChars( cb, flags );

            int cb_len = (int) strlen( cb );
            if( cb_len > max - len )
                cb_len = max - len;

            char* buf = new char[ max + 1 ];
            if( posit )
                memcpy( buf, str, posit );
            if( cb_len )
                memcpy( buf + posit, cb, cb_len );
            if( len - posit )
                memcpy( buf + posit + cb_len, &str[ posit ], len - posit );
            buf[ posit + cb_len + ( len - posit ) ] = 0;
            Str::Copy( str, max + 1, buf );
            delete[] buf;
            delete[] cb;

            posit += cb_len;
        }
        CloseClipboard();
    }
    // Data
    else
    {
        if( len >= max )
            return;
        if( CtrlDwn )
            return;
        KeybData& k = Data[ dik ];
        if( !k.IsAviable )
            return;

        char c;
        if( Lang == LANG_RUS )
            c = ( ShiftDwn ? k.RusShift : k.Rus );
        else
            c = ( ShiftDwn ? k.EngShift : k.Eng );

        if( IsInvalidSymbol( c, flags ) )
            return;

        char* str_ = &str[ len ];
        for( int i = 0, j = len - posit + 1; i < j; i++, str_-- )
            *( str_ + 1 ) = *str_;

        str[ posit ] = c;
        posit++;
    }
}

void Keyb::PuntoSwitch( char* str )
{
    if( !str )
        return;
    for( ; *str; str++ )
    {
        auto it = std::find( Data.begin() + DIK_0 + 1, Data.end(), *str );
        if( it != Data.end() )
        {
            KeybData& k = *it;
            if( k.Rus == *str )
                *str = k.Eng;
            else if( k.Eng == *str )
                *str = k.Rus;
            else if( k.RusShift == *str )
                *str = k.EngShift;
            else if( k.EngShift == *str )
                *str = k.RusShift;
        }
    }
}

void Keyb::EraseInvalidChars( char* str, int flags )
{
    if( !str )
        return;
    while( *str )
    {
        char c = *str;
        if( IsInvalidSymbol( c, flags ) )
        {
            // Copy back
            for( char* str_ = str; *str_; str_++ )
                *str_ = *( str_ + 1 );
        }
        else
        {
            str++;
        }
    }
}

bool Keyb::IsInvalidSymbol( char c, uint flags )
{
    if( flags & KIF_NO_SPEC_SYMBOLS && ( c == '\n' || c == '\r' || c == '\t' ) )
        return true;
    if( flags & KIF_ONLY_NUMBERS && !( c >= '0' && c <= '9' ) )
        return true;
    if( flags & KIF_FILE_NAME )
    {
        switch( c )
        {
        case '\\':
        case '/':
        case ':':
        case '*':
        case '?':
        case '"':
        case '<':
        case '>':
        case '|':
        case '\n':
        case '\r':
        case '\t':
            return true;
        default:
            break;
        }
    }
    return std::find( Data.begin(), Data.end(), c ) == Data.end();
}
