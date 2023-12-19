
#include  "stdafx.h"

#include  "CTViewTool.h"


using namespace jbxl;
using namespace jbxwl;




void  DLLFunctionTBL::set_module_function() 
{ 
	dllfunc = (DLLFunction**)dllmod;

	for (int i=0; i<vnum; i++) {
		dllfunc[i]->pproc_func = (GR_PROC_FUNC)(dllfunc[i]->get_funcptr(1));
		dllfunc[i]->pfree_func = (GR_FREE_FUNC)(dllfunc[i]->get_funcptr(2));
		dllfunc[i]->pactv_func = (GR_ACTV_FUNC)(dllfunc[i]->get_funcptr(3));
	}
	
	return;
} 




void  DLLFunctionTBL::free()
{
	DLLModuleTBL::free();

	dllfunc = NULL;
}