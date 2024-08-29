#pragma once
#ifdef AURORAENGINE_EXPORTS
#define AURORAENGINE_API __declspec(dllexport)
#else
#define  AURORAENGINE_API __declspec(dllimport)
#endif