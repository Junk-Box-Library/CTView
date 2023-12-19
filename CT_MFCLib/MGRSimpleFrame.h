
#ifndef  __CT_MFCLIB_CMGRSIMPLEFRAME_H__
#define  __CT_MFCLIB_CMGRSIMPLEFRAME_H__


#pragma once


#include  "ExFrame.h"




// CMGRSimpleFrame フレーム

class CMGRSimpleFrame : public CExFrame
{
	DECLARE_DYNCREATE(CMGRSimpleFrame)
protected:
	CMGRSimpleFrame();           // 動的生成で使用される protected コンストラクタ
	virtual ~CMGRSimpleFrame();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


#endif