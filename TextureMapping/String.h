#pragma once

#include "stdafx.h"

std::string _TCHAR2string(_TCHAR* argv)
{
	CString txtCString = (CString)argv;
	USES_CONVERSION;//������ʶ��
	std::string GLcharCString = T2A(txtCString);
	return GLcharCString;
}