========================================================================
    CONSOLE APPLICATION : TextureMapping Project Overview
========================================================================

AppWizard has created this TextureMapping application for you.

This file contains a summary of what you will find in each of the files that
make up your TextureMapping application.


TextureMapping.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

TextureMapping.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

TextureMapping.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named TextureMapping.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

功能描述：
给定3D模型文件、N幅不同视角彩色照片、N+1个4x4空间变换矩阵，利用MRF模型求解最佳纹理标签（纹理映射）。(此版本未处理接缝)
Resource List:
1.输入图像-images文件夹
2.图像变换矩阵、模型变换矩阵-transform文件夹
3.相机参数-camera文件夹
4.模型文件-当前工作路径，支持obj/ply等，参见Assimp官网。
