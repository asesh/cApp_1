// profiler.h: Interface of profiler
// Copyright (C) 2017 CloudFactory. All rights reserved

#pragma once

// Chrono synonyms
typedef std::chrono::time_point<std::chrono::high_resolution_clock> \
chrono_high_resolution_clock;

// Interface for our profiler
class CProfiler
{
public:
	// Start profiling
	static chrono_high_resolution_clock			start();
	static chrono_high_resolution_clock			start(const std::wstring &output_string);

	// End profiling
	static chrono_high_resolution_clock			end(const chrono_high_resolution_clock &start_time);

	// Profiling time diff
	static std::chrono::microseconds			time_difference(const chrono_high_resolution_clock &start, const chrono_high_resolution_clock &end);
	static std::chrono::microseconds			time_difference(const chrono_high_resolution_clock &start);
};

// Macros for profiling
#ifdef _DEBUG // Debug version

#define START_PROFILING_STRING	L"Started Profiling----------------------------"
#define END_PROFILING_STRING	L"End Profiling----------------------------"

#ifdef OS_WIN // Windows

#define OUTPUT_DEBUG_STRING(DEBUG_STRING) (::OutputDebugString(DEBUG_STRING);)

#elif OS_MAC // Mac

#endif // OS_WIN

#define START_PROFILING(PROFILING_ID_STRING) \
										CProfiler::start(std::wstring(L"\t*Profiler Started: ").append(PROFILING_ID_STRING).append(L"*\n"));
#define END_PROFILING(START_TIME)		CProfiler::end(START_TIME);

#else // Release version

// Do nothing for release version
#define START_PROFILING()
#define END_PROFILING(START_TIME)

#define OUTPUT_DEBUG_STRING(DEBUG_STRING)

#endif // _DEBUG

// DISPLAY_END_PROFILING macro will display the delta time elapsed between
// start and end of profiling but for release version it won't do
// anything. Make sure to call START_PROFILING before calling this macro
#ifdef _DEBUG // Debug version

#define OUTPUT_DEBUG_STRING(DEBUG_STRING) \
										::OutputDebugString(DEBUG_STRING);

#define END_AND_DISPLAY_PROFILING(START_TIME) \
										OUTPUT_DEBUG_STRING(std::wstring(L"\t*Profiler Ended: " + \
										std::to_wstring(CProfiler::time_difference(START_TIME). \
										count()) + L" microseconds*\n").data());

#else // Release version

// Do nothing for release version
#define DISPLAY_END_PROFILING (START_TIME)

#endif // _DEBUG