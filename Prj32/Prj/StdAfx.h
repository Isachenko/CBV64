// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions (including VB)
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxdlgs.h>        // for FileOpen
#include <afxtempl.h>
#include <afxpriv.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <vector>
#include <utility>
#include "..\common\BaseBool.h"
#include "..\common\BaseTern.h"

typedef std::vector<CTM> MTM;
typedef std::pair <CTV, int> PairTv_I;
typedef std::vector <PairTv_I> MTv_I;
typedef std::vector <MTv_I> MMTv_I;

#include "..\common\sop.h"
#include "..\common\sbf.h"

#include "..\com_lib\bufer.h"
#include "..\com_lib\objects.h"
#include "..\com_lib\sf.h"
#include "..\com_lib\sflog.h"

#include "..\ComBool\ShortBool.h"
#include "..\ComBool\ShortTern.h"

// This macro is the same as IMPLEMENT_OLECREATE, except it passes TRUE
//  for the bMultiInstance parameter to the COleObjectFactory constructor.
//  We want a separate instance of this application to be launched for
//  each OLE automation proxy object requested by automation controllers.
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2
