/**
 * <DpCmd.h>
 * 
 * Copyright (c) 2006 by Dediprog
 */
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include "..\DediPro\project.h"


namespace Context { struct CDediContext;}

using boost::scoped_ptr;
namespace po = boost::program_options;

class CDpCmd : boost::noncopyable
{
public:
    CDpCmd();

public:
    int Dispatch(int ac, TCHAR* av[]);
	bool get_status();
private:
    void Handler();
	void Sequence();
private:
    void SetVpp();
    bool ListTypes();
    bool DetectChip();
    void CalChecksum();
    void BlankCheck();
    void Erase();
    void Program();
    void Read();

    void RawInstructions();
    void Auto();
    void Verify();
private:
    bool do_loadFile();
    void do_BlankCheck();
    void do_Erase();
    void do_Program();
    void do_Read();
    void do_DisplayOrSave();
    void do_ReadSR();
    void do_RawInstructions();

    void do_Auto();
    void do_Verify();
private:
    bool Wait();
    bool InitProject();
    bool MergeFileAndChip();
    void SaveProgContextChanges();
private:
    po::variables_map m_vm;
    scoped_ptr<CProject> m_proj ;
    Context::CDediContext& m_context;
	bool m_status;
};

