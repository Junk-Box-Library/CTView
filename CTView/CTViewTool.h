/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DLL用ツール
//

#ifndef __CT_VIEW_TOOL_H_
#define __CT_VIEW_TOOL_H_


#include  "tools++.h"
#include  "WinTools.h"
#include  "WinDLLTool.h"
#include  "Gdata.h"
#include  "MainFrm.h"


using namespace jbxl;
using namespace jbxwl;




// for DLL Function
typedef  MSGraph<sWord>* (*GR_PROC_FUNC)(MSGraph<sWord>*, CMainFrame*);
typedef  void            (*GR_FREE_FUNC)(MSGraph<sWord>*);
typedef  BOOL            (*GR_ACTV_FUNC)(void);




class  DLLFunction : public DLLModule 
{
public:
	DLLFunction(LPCTSTR fname, int n) : DLLModule(fname, n) { init();}
	DLLFunction()  { init();}
	~DLLFunction() { }

	void  init() { pproc_func = NULL; pfree_func = NULL; pactv_func = NULL;}


public:
	GR_PROC_FUNC  pproc_func;
	GR_FREE_FUNC  pfree_func;
	GR_ACTV_FUNC  pactv_func;


public:
	MSGraph<sWord>* proc_graph(MSGraph<sWord>* vp, CMainFrame* frm=NULL) { return (*pproc_func)(vp, frm);}
	void		    free_graph(MSGraph<sWord>* vp) { (*pfree_func)(vp); return;}
	BOOL 		    actv_graph(void) { return (*pactv_func)();}
};








class  DLLFunctionTBL : public DLLModuleTBL 
{
public:
	DLLFunctionTBL(LPCTSTR dirn) : DLLModuleTBL(dirn) { init();}
	DLLFunctionTBL()  { init();}
	~DLLFunctionTBL() { }

	DLLFunction** dllfunc;

public:
	void  init() { dllfunc=NULL;}
	void  free();

	DLLModule* new_module(LPCTSTR fname, int n) { DLLFunction* mp = new DLLFunction(fname, n); return (DLLModule*)mp;} 
	void  set_module_function();
};





#endif