//
// TextureMapping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Initialize.h"
#include "String.h"
#include "Model.h"
#include "Display.h"

namespace bg = boost::geometry;

int _tmain(int argc, _TCHAR* argv[])
{
	init();

	//Create Model
	std::string model_path = _TCHAR2string(argv[1]);
	Model model(model_path);

	Display(model);

	return 0;
}
