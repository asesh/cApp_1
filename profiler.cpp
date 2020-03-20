// profiler.cpp: Implementation of profiler
// Copyright (C) 2017 CloudFactory. All rights reserved

#include "stdafx.h"
#include "profiler.h"

/////////////////////////////////////////////////////////////////////
// Name: start
// Desc: Start profiling
/////////////////////////////////////////////////////////////////////
std::chrono::time_point<std::chrono::high_resolution_clock> CProfiler::start()
{
	return std::chrono::high_resolution_clock::now();
}

/////////////////////////////////////////////////////////////////////
// Name: start
// Desc: Start profiling
/////////////////////////////////////////////////////////////////////
std::chrono::time_point<std::chrono::high_resolution_clock> CProfiler::start(const std::wstring &output_string)
{
	OUTPUT_DEBUG_STRING(output_string.data());
	return std::chrono::high_resolution_clock::now();
}

/////////////////////////////////////////////////////////////////////
// Name: end
// Desc: End profiling
/////////////////////////////////////////////////////////////////////
std::chrono::time_point<std::chrono::high_resolution_clock> CProfiler::end(const chrono_high_resolution_clock &start_time)
{
	return std::chrono::high_resolution_clock::now();
}

/////////////////////////////////////////////////////////////////////
// Name: time_difference
// Desc: Calculate the time difference between two starting and 
//		 ending points in time
/////////////////////////////////////////////////////////////////////
std::chrono::microseconds CProfiler::time_difference(const chrono_high_resolution_clock &start, const chrono_high_resolution_clock &end)
{
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}

/////////////////////////////////////////////////////////////////////
// Name: time_difference
// Desc: Calculate the time difference between two starting and 
//		 ending points in time
/////////////////////////////////////////////////////////////////////
std::chrono::microseconds CProfiler::time_difference(const chrono_high_resolution_clock &start)
{
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start);
}