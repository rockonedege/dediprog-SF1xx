//------------------------------------------------------------------------------
// Console.h: interface for the Console manipulators.
//------------------------------------------------------------------------------

#if !defined( CONSOLE_MANIP_H__INCLUDED )
#define CONSOLE_MANIP_H__INCLUDED

//------------------------------------------------------------------------------

//------------------------------------------------------------------"includes"--
#include <iostream>
#include <iomanip>
#include <windows.h>

namespace JadedHoboConsole
{
    static const WORD bgMask( BACKGROUND_BLUE      | 
                              BACKGROUND_GREEN     | 
                              BACKGROUND_RED       | 
                              BACKGROUND_INTENSITY   );
    static const WORD fgMask( FOREGROUND_BLUE      | 
                              FOREGROUND_GREEN     | 
                              FOREGROUND_RED       | 
                              FOREGROUND_INTENSITY   );
    
    static const WORD fgBlack    ( 0 ); 
    static const WORD fgLoRed    ( FOREGROUND_RED   ); 
    static const WORD fgLoGreen  ( FOREGROUND_GREEN ); 
    static const WORD fgLoBlue   ( FOREGROUND_BLUE  ); 
    static const WORD fgLoCyan   ( fgLoGreen   | fgLoBlue ); 
    static const WORD fgLoMagenta( fgLoRed     | fgLoBlue ); 
    static const WORD fgLoYellow ( fgLoRed     | fgLoGreen ); 
    static const WORD fgLoWhite  ( fgLoRed     | fgLoGreen | fgLoBlue ); 
    static const WORD fgGray     ( fgBlack     | FOREGROUND_INTENSITY ); 
    static const WORD fgHiWhite  ( fgLoWhite   | FOREGROUND_INTENSITY ); 
    static const WORD fgHiBlue   ( fgLoBlue    | FOREGROUND_INTENSITY ); 
    static const WORD fgHiGreen  ( fgLoGreen   | FOREGROUND_INTENSITY ); 
    static const WORD fgHiRed    ( fgLoRed     | FOREGROUND_INTENSITY ); 
    static const WORD fgHiCyan   ( fgLoCyan    | FOREGROUND_INTENSITY ); 
    static const WORD fgHiMagenta( fgLoMagenta | FOREGROUND_INTENSITY ); 
    static const WORD fgHiYellow ( fgLoYellow  | FOREGROUND_INTENSITY );
    static const WORD bgBlack    ( 0 ); 
    static const WORD bgLoRed    ( BACKGROUND_RED   ); 
    static const WORD bgLoGreen  ( BACKGROUND_GREEN ); 
    static const WORD bgLoBlue   ( BACKGROUND_BLUE  ); 
    static const WORD bgLoCyan   ( bgLoGreen   | bgLoBlue ); 
    static const WORD bgLoMagenta( bgLoRed     | bgLoBlue ); 
    static const WORD bgLoYellow ( bgLoRed     | bgLoGreen ); 
    static const WORD bgLoWhite  ( bgLoRed     | bgLoGreen | bgLoBlue ); 
    static const WORD bgGray     ( bgBlack     | BACKGROUND_INTENSITY ); 
    static const WORD bgHiWhite  ( bgLoWhite   | BACKGROUND_INTENSITY ); 
    static const WORD bgHiBlue   ( bgLoBlue    | BACKGROUND_INTENSITY ); 
    static const WORD bgHiGreen  ( bgLoGreen   | BACKGROUND_INTENSITY ); 
    static const WORD bgHiRed    ( bgLoRed     | BACKGROUND_INTENSITY ); 
    static const WORD bgHiCyan   ( bgLoCyan    | BACKGROUND_INTENSITY ); 
    static const WORD bgHiMagenta( bgLoMagenta | BACKGROUND_INTENSITY ); 
    static const WORD bgHiYellow ( bgLoYellow  | BACKGROUND_INTENSITY );
    
    static class con_dev
    {
        private:
        HANDLE                      hCon;
        DWORD                       cCharsWritten; 
        CONSOLE_SCREEN_BUFFER_INFO  csbi; 
        DWORD                       dwConSize;

        public:
        con_dev() 
        { 
            hCon = GetStdHandle( STD_OUTPUT_HANDLE );
        }
        private:
        void GetInfo()
        {
            GetConsoleScreenBufferInfo( hCon, &csbi );
            dwConSize = csbi.dwSize.X * csbi.dwSize.Y; 
        }
        public:
        void Clear()
        {
            COORD coordScreen = { 0, 0 };
            
            GetInfo(); 
            FillConsoleOutputCharacter( hCon, TEXT(' '),
                                        dwConSize, 
                                        coordScreen,
                                        &cCharsWritten ); 
            GetInfo(); 
            FillConsoleOutputAttribute( hCon,
                                        csbi.wAttributes,
                                        dwConSize,
                                        coordScreen,
                                        &cCharsWritten ); 
            SetConsoleCursorPosition( hCon, coordScreen ); 
        }
        void SetColor( WORD wRGBI, WORD Mask )
        {
            GetInfo();
            csbi.wAttributes &= Mask; 
            csbi.wAttributes |= wRGBI; 
            SetConsoleTextAttribute( hCon, csbi.wAttributes );
        }
    } console;
    
    template <typename T>
    inline std::basic_ostream<T>& clr( std::basic_ostream<T>& os )
    {
        os.flush();
        console.Clear();
        return os;
    };
    
    template <typename T>
    inline std::basic_ostream<T>& fg_red( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiRed, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_green( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiGreen, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_blue( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiBlue, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_white( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiWhite, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_cyan( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiCyan, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_magenta( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiMagenta, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_yellow( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgHiYellow, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_black( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgBlack, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& fg_gray( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( fgGray, bgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_red( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiRed, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_green( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiGreen, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_blue( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiBlue, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_white( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiWhite, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_cyan( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiCyan, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_magenta( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiMagenta, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_yellow( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgHiYellow, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_black( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgBlack, fgMask );
        
        return os;
    }
    
    template <typename T>
    inline std::basic_ostream<T>& bg_gray( std::basic_ostream<T>& os )
    {
        os.flush();
        console.SetColor( bgGray, fgMask );
        
        return os;
    }
}

//------------------------------------------------------------------------------
#endif //!defined ( CONSOLE_MANIP_H__INCLUDED )

