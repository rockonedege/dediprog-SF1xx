/**
 *  \file imbue_null_codecvt.h 
 *  \brief  codecvt facet for std::wofstream to write wchar_t to file in UNICODE
 * 
 * - This code was originally written by Taka Muraoka and published at
 * http://www.codeproject.com/vcpp/stl/upgradingstlappstounicode.asp 
 * and freely available. 
 * 
 */

#ifndef IMBUE_NULL_CODECVT_H_INCLUDED
#define IMBUE_NULL_CODECVT_H_INCLUDED

#include <locale>
using namespace std;    ///< import the c++ name space
using std::codecvt ;
typedef codecvt < wchar_t , char , mbstate_t > NullCodecvtBase ;

/**
 * \brief 
 * a MACRO to facilitate imbuing a locale with facet NullCodecvt
 *
 */
#define IMBUE_NULL_CODECVT( outputFile ) \
{ \
    (outputFile).imbue( std::locale(locale::classic(), new NullCodecvt )) ; \
}


/**
 * \brief 
 * a new codecvt-derived class that converts wchar_ts to wchar_ts 
 * (i.e. do nothing) and attach it to the wofstream object. 
 *
 * - std::wofstream invokes a std::codecvt object to narrow the output 
 * data just before it is written out to the file. std::codecvt objects
 * are responsible for converting strings from one character set to another
 * and C++ requires that two be provided as standard: one that converts chars 
 * to chars (i.e. effectively does nothing) and one that converts wchar_ts to chars
 *
 * - By imbuing this class, when the wofstream tried to convert the data it was writing out, 
 * it would invoke this new codecvt object that did nothing and the 
 * data would be written out unchanged. 
 */
class NullCodecvt
    : public NullCodecvtBase
{

public:
    typedef wchar_t _E ;
    typedef char _To ;
    typedef mbstate_t _St ;

    explicit NullCodecvt( size_t _R=0 ) : NullCodecvtBase(_R) { }

protected:
    virtual result do_in( _St& /*_State*/ ,
                   const _To* /*_F1*/ , const _To* /*_L1*/ , const _To*& /*_Mid1*/ ,
                   _E* /*F2*/ , _E* /*_L2*/ , _E*& /*_Mid2*/
                   ) const
    {
        return noconv ;
    }
    virtual result do_out( _St& /*_State*/ ,
                   const _E* /*_F1*/ , const _E* /*_L1*/ , const _E*& /*_Mid1*/ ,
                   _To* /*F2*/, _E* /*_L2*/ , _To*& /*_Mid2*/
                   ) const
    {
        return noconv ;
    }
    virtual result do_unshift( _St& /*_State*/ ,
            _To* /*_F2*/ , _To* /*_L2*/ , _To*& /*_Mid2*/ ) const
    {
        return noconv ;
     }
    virtual size_t do_length( _St& /*_State*/ , const _To* _F1 ,
           const _To* _L1 , size_t _N2 ) const _THROW0()
    {
        return (_N2 < (size_t)(_L1 - _F1)) ? _N2 : _L1 - _F1 ;
    }
    virtual bool do_always_noconv() const _THROW0()
    {
        return true ;
    }
    virtual int do_max_length() const _THROW0()
    {
        return 2 ;
    }
    virtual int do_encoding() const _THROW0()
    {
        return 2 ;
    }
} ;



#endif // IMBUE_NULL_CODECVT_H_INCLUDED
