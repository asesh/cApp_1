//
//

#include "stdafx.h"
#include <io.h>
#include <ctime>
#include <fcntl.h>

#include <algorithm>

#include <Windows.h>
#include <gdiplus.h>

//#include <aws/core/Aws.h>
//#include <aws/s3/S3Client.h>
//#include <aws/s3/model/PutObjectRequest.h>


using namespace Gdiplus;

#pragma comment(lib, "gdiplus")

//#include <atlbase.h>
//#include <objbase.h>
//#include <psapi.h>
//#include <shellapi.h>
//#include <wrl/client.h>
//#include "google_update_idl.h"

//bool g_app_exit = false;
//
//std::mutex g_lock_queue_mutex;
//std::deque<std::string> g_data_container;
//
//bool g_data_available = false;
//std::condition_variable g_data_available_cv;
//
//void save_app_data(const std::string &data)
//{
//	std::lock_guard<std::mutex> lock_mutex(g_lock_queue_mutex);
//
//	g_data_container.push_back(data);
//
//	g_data_available = true;
//	g_data_available_cv.notify_one();
//	//g_data_available = false;
//}
//
//void write_app_switched_callback()
//{
//	while (!g_app_exit)
//	{
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//		save_app_data("data from callback");
//	}
//}
//
//void write_app_switched_thread_proc()
//{
//	while (!g_app_exit)
//	{
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//		save_app_data("data from thread procedure");
//	}
//}
//
//void send_data_thread_proc()
//{
//	std::mutex send_data_thread_mutex;
//	std::unique_lock<std::mutex> lock_mutex(send_data_thread_mutex);
//
//	while (1)
//	{
//		g_data_available_cv.wait(lock_mutex, [=]()->bool
//		{
//			return g_data_available || g_app_exit;
//		});
//
//		if (g_app_exit)
//			break;
//
//		std::unique_lock<std::mutex> lock_mutex(g_lock_queue_mutex);
//
//		auto data_iter = g_data_container.begin();
//		for (; data_iter != g_data_container.end();)
//		{
//			std::cout << "Data available: " << *data_iter << std::endl;
//			data_iter = g_data_container.erase(data_iter);
//
//			if (data_iter != g_data_container.end())
//				++data_iter;
//		}
//		lock_mutex.unlock();
//	}
//
//	lock_mutex.unlock();
//}
//
//
//namespace np
//{
//	int data = 0;
//}
//
//int g_data = 0;
//
//int mystrlen(char *data)
//{
//	int counter = 0;
//	while (*data++)
//	{
//		counter++;
//	}
//
//	return counter;
//}
//
//using namespace std;
//
//class CBaseTest
//{
//public:
//	void *operator new(const size_t allocation_size)
//	{
//		return ::malloc(allocation_size);
//	}
//
//	void operator delete(void *block_of_memory)
//	{
//		::free(block_of_memory);
//	}
//};
//
//class test : public CBaseTest
//{
//
//public:
//	int age;
//	string name;
//
//	test(string str, int a)
//	{
//		age = a;
//		name = str;
//	}
//	void display();
//};
//
//void test::display()
//{
//	cout << "\n\nname is:-" << name << "  and age is :- " << age << endl;
//}
//
//class A {};
//
//void Foo(A *p)
//{
//	std::cout << &p << std::endl;
//	std::cout << p << std::endl;
//}
//
LRESULT CALLBACK message_wnd_proc(HWND window_handle, UINT window_message, WPARAM wparam, LPARAM lparam)
{
	switch (window_message)
	{
	case WM_CLOSE:
		::DestroyWindow(window_handle);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wparam))
		{
		case VK_ESCAPE:
			::SendMessage(window_handle, WM_CLOSE, 0, 0);
			break;
		}

	default:
		return ::DefWindowProc(window_handle, window_message, wparam, lparam);
	}

	return 0;
}

//void recapture_app_name(HWND window_handle, std::string &captured_app_name)
//{
//	DWORD process_id = 0;
//	DWORD thread_identifier = ::GetWindowThreadProcessId(window_handle, &process_id);
//
//	if (process_id)
//	{
//		HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
//		if (process_handle)
//		{
//			char image_path[MAX_PATH * 2] = { 0 };
//			DWORD buffer_size = sizeof(image_path);
//
//			// Get the buffer size required to hold full process image name
//			// There's no need to check if it managed to get the absolute path of app as we have already zeroed out the buffer
//			::QueryFullProcessImageNameA(process_handle, 0, image_path, &buffer_size);
//
//			captured_app_name.assign(image_path);
//
//			::CloseHandle(process_handle);
//		}
//	}
//}

#include <atlbase.h>
#include <Propkey.h>

std::string g_tracked_app_path;

std::mutex g_app_switch_mutex;

std::wstring get_app_description(const std::wstring& app_path);

bool on_app_switched(HWND window_handle)
{
	std::lock_guard<std::mutex> lock_mutex(g_app_switch_mutex);

	//GUITHREADINFO gui_thread_info = {};
	//gui_thread_info.cbSize = sizeof(GUITHREADINFO);

	static std::time_t start_time;
	static uint64_t start_timestamp;

	DWORD process_id = 0;
	DWORD thread_identifier = ::GetWindowThreadProcessId(window_handle, &process_id);
	if (process_id)
	{
		HANDLE process_handle = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, process_id);
		if (process_handle)
		{
			char image_path[MAX_PATH * 2] = { 0 };
			DWORD buffer_size = sizeof(image_path);

			// Get the buffer size required to hold full process image name
			// There's no need to check if it managed to get the absolute path of app as we have already zeroed out the buffer
			::QueryFullProcessImageNameA(process_handle, 0, image_path, &buffer_size);

			if(g_tracked_app_path.compare(image_path) != 0) // User switched to a new application
			{
				//start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				//auto end_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_timestamp);

				if (!g_tracked_app_path.empty()) // We were tracking another application before
				{
					auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() -
						start_timestamp;
					std::cout << "\t\tDuration elapsed: " << duration_milliseconds << " milliseconds" << std::endl;
				}
				std::wstring app_wide_description;
				std::string file_path(image_path);
				std::wstring image_wide_path(file_path.begin(), file_path.end());
				app_wide_description = get_app_description(image_wide_path);
				std::string app_description(app_wide_description.begin(), app_wide_description.end());


				std::cout << "**Active app: " << image_path << "-> App desc: " << app_description << " -> Process id: " << process_id << std::endl;
				g_tracked_app_path = image_path;
				start_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			}
			// else user didn't switch to a new application

			//std::string app_path(image_path);
			//std::transform(app_path.begin(), app_path.end(), app_path.begin(), std::tolower);
			//std::string werfault_exe = "werfault.exe";
			//if (app_path.find(werfault_exe) != std::string::npos)
			//{
			//	std::cout << "--------------Fault app detected" << std::endl;
			//}

			::CloseHandle(process_handle);

			//std::string captured_app_name;
			//if (::GetGUIThreadInfo(thread_identifier, &gui_thread_info))
			//{
			//	recapture_app_name(gui_thread_info.hwndActive, captured_app_name);
			//	std::transform(captured_app_name.begin(), captured_app_name.end(), captured_app_name.begin(), std::tolower);
			//}

			//if (captured_app_name.compare(app_path) == 0)
			//{
			//	std::cout << "*** Captured app name: " << captured_app_name << std::endl;
			//}
			//else
			//{
			//	std::cout << "### Captured app names don't match. Two distinct names are: First one: " << app_path << "; Second one: " <<captured_app_name << std::endl;
			//}

			//std::cout << endl << endl;

			return true;
		}
		else
		{
			std::cout << "**Unable to open process of process id: " << process_id << std::endl;
		}
	}
	return false;
}

void CALLBACK win_event_callback(
	HWINEVENTHOOK win_event_hook,
	DWORD window_event,
	HWND window_handle,
	LONG id_object,
	LONG id_child,
	DWORD event_thread,
	DWORD event_time)
{
	switch (window_event)
	{
	case EVENT_SYSTEM_FOREGROUND:
		on_app_switched(window_handle);
		break;
	}
}

//#include <TlHelp32.h>

//std::uint32_t find(const char* proc)
//{
//	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	auto pe = PROCESSENTRY32{ sizeof(PROCESSENTRY32) };
//
//	if (Process32First(snapshot, &pe)) {
//		do {
//			if (!_stricmp(proc, pe.szExeFile)) {
//				CloseHandle(snapshot);
//				return pe.th32ProcessID;
//			}
//		} while (Process32Next(snapshot, &pe));
//	}
//	CloseHandle(snapshot);
//	return 0;
//}

//std::string &&get_move()
//{
//	std::string data = "hello world";
//	return std::move(data);
//}
//
//void get_string(std::string &data)
//{
//	data = std::move(std::string("Hello world"));
//}

//void test_move(std::string &reference_string)
//{
//	/*std::string hello_world = "hello world";
//	reference_string = std::move(hello_world);*/
//	const char string_data[] = "hello world";
//	reference_string = std::move(string_data);
//}

//bool write_exit_code(int32_t exit_code) {
//	wchar_t* local_app_data_path = nullptr;
//	if (SUCCEEDED(::SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &local_app_data_path))) {
//		std::wstring workstream_monitor_folder_name = L"WorkStreamMonitor";
//		std::wstring user_data_folder_path(local_app_data_path);
//		user_data_folder_path.append(L"\\CloudFactory\\WorkStream\\User Data\\");
//
//		std::wstring absolute_workstream_monitor_log_folder_path(user_data_folder_path);
//		absolute_workstream_monitor_log_folder_path.append(workstream_monitor_folder_name.data());
//
//		if (!::SetCurrentDirectory(absolute_workstream_monitor_log_folder_path.data())) {
//			if (!::SetCurrentDirectory(user_data_folder_path.data())) {
//				return false;
//			}
//
//			::CreateDirectory(workstream_monitor_folder_name.data(), nullptr);
//			if (!::SetCurrentDirectory(workstream_monitor_folder_name.data())) {
//				return false;
//			}
//		}
//
//		const wchar_t wm_exit_log_file_name[] = L"wm_exit";
//
//		bool truncate_file_contents = false;
//
//		std::streampos wm_exit_file_log_size = 0;
//
//		// Open the 
//		ifstream file(wm_exit_log_file_name, ios::binary | ios::ate);
//		if (!file.fail()) {
//			wm_exit_file_log_size = file.tellg();
//			std::wcout << L"File size: " << wm_exit_file_log_size << std::endl;
//			file.close();
//		}
//
//		// Let's make sure the size of this file doesn't exceed 1 MB. If it does, then we truncate it's contents
//		// before writing any data into it
//		if (wm_exit_file_log_size >= 1024 * 1024) {
//			truncate_file_contents = true;
//		}
//
//		int file_open_mode = std::ios_base::out | (truncate_file_contents ? std::ios_base::trunc : std::ios_base::app);
//
//		// Open the file for writing exit code from last run
//		std::wofstream wm_exit_log_file_stream(wm_exit_log_file_name, file_open_mode);
//		if (!wm_exit_log_file_stream.fail()) {
//
//			// Format date and time
//			std::wstringstream date_time;
//			auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//			date_time << std::put_time(std::localtime(&in_time_t), L"%Y-%m-%d %X");
//
//			// Dump date, time and exit code of workstream monitor
//			wm_exit_log_file_stream << date_time.str() << ' ' << exit_code << std::endl;
//		}
//		::CoTaskMemFree(local_app_data_path);
//	}
//	return true;
//}

//inline bool file_exists(const std::string& name)
//{
//	struct stat buffer;
//	return (stat(name.c_str(), &buffer) == 0);
//}
//
//// snippet-start:[s3.cpp.put_object_async.mutex_vars]
//std::mutex upload_mutex;
//std::condition_variable upload_variable;
//// snippet-end:[s3.cpp.put_object_async.mutex_vars]
//
//// snippet-start:[s3.cpp.put_object_async_finished.code]
//void put_object_async_finished(const Aws::S3::S3Client* client,
//	const Aws::S3::Model::PutObjectRequest& request,
//	const Aws::S3::Model::PutObjectOutcome& outcome,
//	const std::shared_ptr<const Aws::Client::AsyncCallerContext>& context)
//{
//	// Output operation status
//	if (outcome.IsSuccess()) {
//		std::cout << "put_object_async_finished: Finished uploading "
//			<< context->GetUUID() << std::endl;
//	}
//	else {
//		auto error = outcome.GetError();
//		std::cout << "ERROR: " << error.GetExceptionName() << ": "
//			<< error.GetMessage() << std::endl;
//	}
//
//	// Notify the thread that started the operation
//	upload_variable.notify_one();
//}
//// snippet-end:[s3.cpp.put_object_async_finished.code]
//
///**
// * Asynchronously put an object into an Amazon S3 bucket
// */
// // snippet-start:[s3.cpp.put_object_async.code]
//bool put_s3_object_async(const Aws::S3::S3Client& s3_client,
//	const Aws::String& s3_bucket_name,
//	const Aws::String& s3_object_name,
//	const std::string& file_name)
//{
//	// Verify file_name exists
//	if (!file_exists(file_name)) {
//		std::cout << "ERROR: NoSuchFile: The specified file does not exist"
//			<< std::endl;
//		return false;
//	}
//
//	// Set up request
//	Aws::S3::Model::PutObjectRequest object_request;
//
//	object_request.SetBucket(s3_bucket_name);
//	object_request.SetKey(s3_object_name);
//	const std::shared_ptr<Aws::IOStream> input_data =
//		Aws::MakeShared<Aws::FStream>("SampleAllocationTag",
//			file_name.c_str(),
//			std::ios_base::in | std::ios_base::binary);
//	object_request.SetBody(input_data);
//
//	// Set up AsyncCallerContext. Pass the S3 object name to the callback.
//	auto context =
//		Aws::MakeShared<Aws::Client::AsyncCallerContext>("PutObjectAllocationTag");
//	context->SetUUID(s3_object_name);
//
//	// Put the object asynchronously
//	s3_client.PutObjectAsync(object_request,
//		put_object_async_finished,
//		context);
//	return true;
//	// snippet-end:[s3.cpp.put_object_async.code]
//}

#define ARRAY_SIZE(ARRAY)				(sizeof(ARRAY) / sizeof(ARRAY[0]))
#define SAFE_DELETE_PTR_ARRAY(POINTER) {delete [] POINTER;}











/*

Take screenshots of multiple displays and save them into different filenames

*/

constexpr wchar_t image_jpeg_mime[] = L"image/jpeg";

#define LOG_ERROR(x) std::wcerr<< x << std::endl;
#define LOG_INFO(x) std::wcout << x << std::endl;

void get_base64_encoded_data(unsigned char const* bytes_to_encode,
	unsigned int in_len,
	std::string& base64_encoded_data) {
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	while (in_len--) {
		char_array_3[i++] = *(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				base64_encoded_data += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; (j < i + 1); j++)
			base64_encoded_data += base64_chars[char_array_4[j]];

		while ((i++ < 3))
			base64_encoded_data += '=';
	}
}

HDC m_desktop_dc = nullptr, m_memory_dc = nullptr;

HBITMAP m_memory_bitmap = nullptr;

IStream* m_screenshot_stream = nullptr;

bool internal_capture_screenshot_data(HDC desktop_device_context, HDC memory_dc,
	int screen_width, int screen_height, int screen_width_virtual, int screen_height_virtual,
	IStream* screenshot_stream, bool secondary_display, std::string& screenshot_data);
bool get_encoder_clsid(const wchar_t* format, CLSID* encoder_clsid);
void free_resources();

bool capture_screenshots() {
	ULONG_PTR m_gdiplus_token = 0;
	GdiplusStartupInput gdiplus_startup_input;
	if (::GdiplusStartup(&m_gdiplus_token, &gdiplus_startup_input, nullptr) != Gdiplus::Ok) {
		LOG_ERROR(L"Initializing GDI+ for capturing screenshots");
		free_resources();
		return false;
	}

	// Get the number of monitors attached
	auto monitor_count = ::GetSystemMetrics(SM_CMONITORS);

	// Get the DC of all the monitors
	m_desktop_dc = ::GetDC(HWND_DESKTOP);
	if (m_desktop_dc == nullptr) {
		LOG_ERROR(L"Getting the DC of the desktop");
		return false;
	}

	// Get the width and height of the primary monitor
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	if (FAILED(::CreateStreamOnHGlobal(nullptr, true, &m_screenshot_stream))) {
		LOG_ERROR(L"Creating stream on HGLOBAL");
		return false;
	}

	m_memory_dc = ::CreateCompatibleDC(m_desktop_dc);

	std::string primary_screenshot_data, secondary_screenshot_data;

	// If there are more than one monitors then we capture the screenshots of both the primary and secondary displays but those images
	// will be stitched together.
	if (monitor_count > 1) {
		// Multiple displays detected
		LOG_INFO(L"Multiple displays detected");

		// Get the width and height of the secondary monitor AKA virtual display
		int screen_width_virtual = ::GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int screen_height_virtual = ::GetSystemMetrics(SM_CYVIRTUALSCREEN);

		// Capture the primary desktop screenshot
		if (!internal_capture_screenshot_data(m_desktop_dc, m_memory_dc,
			screen_width, screen_height, 0, 0, m_screenshot_stream, false, primary_screenshot_data)) {
			free_resources();
			LOG_ERROR(L"Saving primary desktop screenshot into stream");
			return false;
		}
		// Capture the secondary desktop screenshot
		if (!internal_capture_screenshot_data(m_desktop_dc, m_memory_dc,
			screen_width, screen_height, screen_width_virtual, screen_height_virtual, m_screenshot_stream, true,
			secondary_screenshot_data)) {
			free_resources();
			LOG_ERROR(L"Saving virtual desktop screenshot into stream");
			return false;
		}
	}
	else {
		// Single display detected
		LOG_INFO(L"Single display detected");

		if (!internal_capture_screenshot_data(m_desktop_dc, m_memory_dc,
			screen_width, screen_height, 0, 0, m_screenshot_stream, false, primary_screenshot_data)) {
			LOG_ERROR(L"Saving primary desktop screenshot to stream");
			free_resources();
			return false;
		}
	}

	free_resources();

	return true;
}

bool get_encoder_clsid(const wchar_t* format, CLSID* encoder_clsid) {
	uint32_t number_of_encoders = 0;
	uint32_t total_size_image_codec_info = 0;

	// Get the number of available image encoders
	if (::GetImageEncodersSize(&number_of_encoders, &total_size_image_codec_info) != Gdiplus::Ok) {
		LOG_ERROR(L"Getting the image encoders size");
		return false;
	}
	if (total_size_image_codec_info == 0) {
		LOG_ERROR(L"The total size of the retrieved image codec info is invalid");
		return false;
	}

	std::unique_ptr<ImageCodecInfo> image_codec_info(new ImageCodecInfo[total_size_image_codec_info]);

	if (::GetImageEncoders(number_of_encoders, total_size_image_codec_info, image_codec_info.get()) == Gdiplus::Ok) {
		for (uint32_t encoder_counter = 0; encoder_counter < number_of_encoders; ++encoder_counter) {
			if (std::wstring(image_codec_info.get()[encoder_counter].MimeType).compare(format) == 0) {
				*encoder_clsid = image_codec_info.get()[encoder_counter].Clsid;
				return true;
			}
		}
		LOG_ERROR(L"While processing the image encoders");
		return false;
	}
	LOG_ERROR(L"Getting the image encoders");
	return false;
}

// Captures the screenshots of the primary and secondary displays. 
// Note: To capture a screenshot of secondary display: set screen width to the width of of primary display, 
// sceen height to the height of secondary display and virtual screen width to the width of secondary display
bool internal_capture_screenshot_data(HDC desktop_device_context, HDC memory_dc,
	int screen_width, int screen_height, int screen_width_virtual, int screen_height_virtual,
	IStream* screenshot_stream, bool secondary_display, std::string& screenshot_data) {

	static constexpr char base64_header[] = "data:image/jpg;base64,";

	CLSID encoder_clsid;

	// Capture the bits of either primary or secondary display
	HBITMAP memory_bitmap = ::CreateCompatibleBitmap(desktop_device_context,
		secondary_display ? screen_width_virtual - screen_width : screen_width,
		secondary_display ? screen_height_virtual : screen_height);
	::SelectObject(memory_dc, memory_bitmap);
	::BitBlt(memory_dc, 0, 0,
		secondary_display ? screen_width_virtual : screen_width,
		secondary_display ? screen_height_virtual : screen_height, desktop_device_context,
		secondary_display ? screen_width : 0, 0, SRCCOPY);

	// Lambda function for clean up of this function
	auto internal_cleanup = [&]() {
		::DeleteObject(memory_bitmap);
	};

	Gdiplus::Bitmap screenshot_bitmap(memory_bitmap, false);
	if (!get_encoder_clsid(image_jpeg_mime, &encoder_clsid)) {
		internal_cleanup();
		return false;
	}

	std::string jpeg_filename;
	std::string jpeg_filename_base64;
	if (!secondary_display) {
		jpeg_filename = "primary_screen.jpeg";
		jpeg_filename_base64 = "primary_screen_base64.txt";
	}
	else {
		jpeg_filename = "secondary_screen.jpeg";
		jpeg_filename_base64 = "secondary_screen_base64.txt";
	}

	if (screenshot_bitmap.Save(screenshot_stream, &encoder_clsid, nullptr) == Gdiplus::Ok) {
		LOG_INFO(L"Saved JPEG blob to stream");

		static LARGE_INTEGER image_seek_position = { 0 };
		ULARGE_INTEGER image_size;

		if (!secondary_display) {
			image_seek_position = { 0 };
		}

		if (SUCCEEDED(screenshot_stream->Seek(image_seek_position, STREAM_SEEK_CUR, &image_size))) {

			// Allocate memory for the image blob
			std::unique_ptr<BYTE> image_blob(new BYTE[static_cast<uint32_t>(image_size.LowPart)]);
			if (!image_blob) {
				LOG_ERROR(L"Allocating memory for image blob");
				internal_cleanup();
				return false;
			}

			ULONG bytes_read = 0;

			// Fill buffer from stream
			if (SUCCEEDED(screenshot_stream->Seek(image_seek_position, STREAM_SEEK_SET, 0))) {
				if (SUCCEEDED(screenshot_stream->Read(image_blob.get(), image_size.LowPart, &bytes_read))) {
					image_seek_position.QuadPart += bytes_read;

					std::fstream write_jpeg(jpeg_filename, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
					write_jpeg.write((const char*)image_blob.get(), image_size.LowPart);
					write_jpeg.close();
					std::cout << "Saved the screenshot to: " << jpeg_filename << std::endl;

					std::string base64_encoded_data;
					get_base64_encoded_data(image_blob.get(), image_size.LowPart, base64_encoded_data);
					std::string base64_encoded_with_header(base64_header); // Append this header for timer extension
					base64_encoded_with_header.append(std::move(base64_encoded_data));
					std::fstream output_filestream(jpeg_filename_base64, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
					output_filestream.write(base64_encoded_with_header.c_str(), base64_encoded_with_header.size());
					screenshot_data = std::move(base64_encoded_with_header);
					internal_cleanup();
					return true;
				}
				LOG_ERROR(L"Reading image blob from the screenshot stream");
				internal_cleanup();
				return false;
			}
			LOG_ERROR(L"Seeking to the beginning of the screenshot stream");
			internal_cleanup();
			return false;
		}
		LOG_ERROR(L"Seeking to the current location of the screenshot stream");
		internal_cleanup();
	}
	return false;
}

// Free up the most commonly used resources
void free_resources() {
	if (m_screenshot_stream) {
		m_screenshot_stream->Release();
		m_screenshot_stream = nullptr;
	}
	if (m_desktop_dc) {
		::ReleaseDC(HWND_DESKTOP, m_desktop_dc);
		m_desktop_dc = nullptr;
	}
	if (m_memory_dc) {
		::DeleteDC(m_memory_dc);
		m_memory_dc = nullptr;
	}
}















#include <Iphlpapi.h>
#include <Assert.h>
#pragma comment(lib, "iphlpapi.lib")

bool getMAC(std::list<std::wstring>& mac_addresses) {

	DWORD buffer_length = sizeof(IP_ADAPTER_INFO);
	char mac_address[50];

	IP_ADAPTER_INFO* adapter_info = new IP_ADAPTER_INFO[sizeof(IP_ADAPTER_INFO)];

	// Make an initial call to get the necessary buffer size
	if (::GetAdaptersInfo(adapter_info, &buffer_length) == ERROR_BUFFER_OVERFLOW) {
		SAFE_DELETE_PTR_ARRAY(adapter_info);
		adapter_info = new IP_ADAPTER_INFO[buffer_length];
	}

	if (::GetAdaptersInfo(adapter_info, &buffer_length) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = adapter_info;
		do {
			snprintf(mac_address, ARRAY_SIZE(mac_address),
				"%02X:%02X:%02X:%02X:%02X:%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			std::string mac_address_data(mac_address);
			std::wstring mac_address_wide_data(mac_address_data.begin(), mac_address_data.end());
			mac_addresses.push_back(mac_address_wide_data);

			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	SAFE_DELETE_PTR_ARRAY(adapter_info);
	return true;

	//PIP_ADAPTER_INFO AdapterInfo;
	//DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	//char* mac_addr = (char*)malloc(18);

	//AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	//if (AdapterInfo == NULL) {
	//	printf("Error allocating memory needed to call GetAdaptersinfo\n");
	//	free(mac_addr);
	//	return NULL; // it is safe to call free(NULL)
	//}

	//// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen variable
	//if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
	//	free(AdapterInfo);
	//	AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
	//	if (AdapterInfo == NULL) {
	//		printf("Error allocating memory needed to call GetAdaptersinfo\n");
	//		free(mac_addr);
	//		return NULL;
	//	}
	//}

	//if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
	//	// Contains pointer to current adapter info
	//	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	//	do {
	//		// technically should look at pAdapterInfo->AddressLength
	//		//   and not assume it is 6.
	//		sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
	//			pAdapterInfo->Address[0], pAdapterInfo->Address[1],
	//			pAdapterInfo->Address[2], pAdapterInfo->Address[3],
	//			pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
	//		printf("mac: %s\n", mac_addr);
	//		// print them all, return the last one.
	//		// return mac_addr;

	//		printf("\n");
	//		pAdapterInfo = pAdapterInfo->Next;
	//	} while (pAdapterInfo);
	//}
	//free(AdapterInfo);
	//return mac_addr; // caller must free.
}

#include <dshow.h>

#pragma comment(lib, "strmiids")

//HRESULT EnumerateDevices(REFGUID category, IEnumMoniker** ppEnum)
//{
//	// Create the System Device Enumerator.
//	ICreateDevEnum* vide_input_device_enum;
//	HRESULT com_result = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
//		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&vide_input_device_enum));
//
//	if (SUCCEEDED(com_result))
//	{
//		// Create an enumerator for the category.
//		com_result = vide_input_device_enum->CreateClassEnumerator(category, ppEnum, 0);
//		if (com_result == S_FALSE)
//		{
//			com_result = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
//		}
//		vide_input_device_enum->Release();
//	}
//	return com_result;
//}
//
//
//void DisplayDeviceInformation(IEnumMoniker* enum_category)
//{
//	IMoniker* moniker = NULL;
//
//	while (enum_category->Next(1, &moniker, NULL) == S_OK)
//	{
//		IPropertyBag* property_bag;
//		HRESULT com_result = moniker->BindToStorage(0, 0, IID_PPV_ARGS(&property_bag));
//		if (FAILED(com_result))
//		{
//			moniker->Release();
//			continue;
//		}
//
//		VARIANT var;
//		VariantInit(&var);
//
//		// Get description or friendly name.
//		com_result = property_bag->Read(L"Description", &var, 0);
//		if (FAILED(com_result))
//		{
//			com_result = property_bag->Read(L"FriendlyName", &var, 0);
//		}
//		if (SUCCEEDED(com_result))
//		{
//			printf("%S\n", var.bstrVal);
//			VariantClear(&var);
//		}
//
//		property_bag->Release();
//		moniker->Release();
//	}
//}

void get_webcam_name(std::wstring& webcam_name) {
	REFGUID category = CLSID_VideoInputDeviceCategory;
	IEnumMoniker* enum_category = nullptr;

	// Create the System Device Enumerator.
	ICreateDevEnum* vide_input_device_enum;
	HRESULT com_result = ::CoCreateInstance(CLSID_SystemDeviceEnum, 
		nullptr,
		CLSCTX_INPROC_SERVER, 
		IID_PPV_ARGS(&vide_input_device_enum));
	if (SUCCEEDED(com_result)) {
		// Create an enumerator for the category.
		if (vide_input_device_enum->CreateClassEnumerator(category, &enum_category, 0) == S_FALSE) {
			com_result = VFW_E_NOT_FOUND;
		}
		vide_input_device_enum->Release();
		if (com_result == VFW_E_NOT_FOUND)
			return;
	}

	IMoniker* moniker = nullptr;

	// Just enumerate the first video input device
	while(enum_category->Next(1, &moniker, nullptr) == S_OK) {
		IPropertyBag* property_bag = nullptr;
		HRESULT com_result = moniker->BindToStorage(nullptr, nullptr, IID_PPV_ARGS(&property_bag));
		if(FAILED(com_result)) {
			moniker->Release();
			continue;
		}

		VARIANT var;
		::VariantInit(&var);

		// Get description or friendly name.
		com_result = property_bag->Read(L"Description", &var, 0);
		if(FAILED(com_result)) {
			com_result = property_bag->Read(L"FriendlyName", &var, 0);
		}
		if(SUCCEEDED(com_result)) {
			webcam_name = var.bstrVal;
			VariantClear(&var);
		}

		property_bag->Release();
		moniker->Release();
	}
}

typedef LONG NTSTATUS, * PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

//RTL_OSVERSIONINFOW GetRealOSVersion() {
//	HMODULE ntddl_module = ::GetModuleHandleW(L"ntdll.dll");
//	if (ntddl_module) {
//		RtlGetVersionPtr rtl_get_version_ptr = reinterpret_cast<RtlGetVersionPtr>(::GetProcAddress(ntddl_module, "RtlGetVersion"));
//		if (rtl_get_version_ptr != nullptr) {
//			RTL_OSVERSIONINFOW rovi = { 0 };
//			rovi.dwOSVersionInfoSize = sizeof(rovi);
//			if (STATUS_SUCCESS == rtl_get_version_ptr(&rovi)) {
//				return rovi;
//			}
//		}
//	}
//	RTL_OSVERSIONINFOW rovi = { 0 };
//	return rovi;
//}
typedef LONG NTSTATUS, * PNTSTATUS;
#define STATUS_SUCCESS (0x00000000)
typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
std::wstring get_os_version_info() {
	std::wstring os_version_info;
	RTL_OSVERSIONINFOW rovi = { 0 };
	rovi.dwOSVersionInfoSize = sizeof(rovi);
	HMODULE ntddl_module = ::GetModuleHandleW(L"ntdll.dll");
	if (ntddl_module) {
		RtlGetVersionPtr rtl_get_version_ptr = reinterpret_cast<RtlGetVersionPtr>(::GetProcAddress(ntddl_module, "RtlGetVersion"));
		if (rtl_get_version_ptr != nullptr) {
			if (STATUS_SUCCESS == rtl_get_version_ptr(&rovi)) {
				os_version_info = std::wstring(L"win ").append(std::to_wstring(rovi.dwMajorVersion)).append(L".").append(std::to_wstring(rovi.dwMinorVersion));
			}
		}
	}
	return os_version_info;
}

class CUnique {
public:
	CUnique() {}
	~CUnique() {}

public:
	int foo = 12;
};


bool file_exits(const std::wstring& file_name) {
	std::wifstream file(file_name.c_str());
	return file.good();
}

bool create_file(const std::wstring& file_name) {
	std::wfstream file(file_name.c_str(), std::ios_base::out);
	return file.good();
}


#include <atlbase.h>
#include <Propkey.h>

#pragma comment(lib, "version.lib")

// Source: https://stackoverflow.com/questions/53123914/retrieve-file-description-an-application-verqueryvalue
std::wstring get_app_description(const std::wstring& app_path) {
	DWORD length = GetFileVersionInfoSize(app_path.data(), nullptr);
	if (!length) {
		return L"";
	}

	std::unique_ptr<BYTE[]> version_info(new BYTE[length]);
	if (!GetFileVersionInfo(app_path.data(), 0, length, version_info.get())) {
		return L"";
	}

	static struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *translate;

	UINT translate_length = 0;
	if (!VerQueryValue(version_info.get(), L"\\VarFileInfo\\Translation",
		reinterpret_cast<void**>(&translate), &translate_length))
		return L"";

	for (unsigned int index = 0; index < (translate_length / sizeof(LANGANDCODEPAGE)); index++) {
		wchar_t sub_block[MAX_PATH * 2];
		//use sprintf if sprintf_s is not available
		wsprintf(sub_block, L"\\StringFileInfo\\%04x%04x\\FileDescription", translate[index].wLanguage, translate[index].wCodePage);
		wchar_t* app_description = nullptr;
		UINT buffer_length;
		if (VerQueryValue(version_info.get(), sub_block, reinterpret_cast<void**>(&app_description), &buffer_length)) {
			return app_description;
		}
	}
	return L"";
}





#include "extension_whitelist.h"




int main() {
	//std::set<std::wstring> some_set;
	//some_set.insert(L"foo");
	//some_set.insert(L"abc");
	//some_set.insert(L"efg");
	//some_set.insert(L"foo");
	//some_set.insert(L"FOO");
	//some_set.insert(L"abc");
	//for (const auto& item : some_set) {
	//	std::wcout << item << std::endl;
	//}





	// Extension whitelist
	std::unique_ptr<CExtensionWhitelist> extension_list = CExtensionWhitelist::Create(L"list");

	//std::map<int, std::wstring> extension_whitelist;
	//extension_list->read_whitelist_from_db(extension_whitelist);

	//testing::InitGoogleTest();
	//testing::InitGoogleMock();

	//RUN_ALL_TESTS();














	//::CoInitialize(nullptr);





	//capture_screenshots();









	//if (!::SetCurrentDirectoryW(LR"(C:\Users\Asesh\AppData\Local\CloudFactory\WorkStream\User Data\WorkStreamMonitor\safe_exit)")) {
	//	std::wcout << L"Failed to changed the directory" << std::endl;
	//}
	//else {
	//	std::wcout << L"Changed the directory successfully" << std::endl;
	//}
	//wchar_t file_path[] = LR"(C:\Users\Asesh\AppData\Local\CloudFactory\WorkStream\User Data\WorkStreamMonitor\safe_exit)";
	//create_file(file_path);
	//if (file_exits(file_path)) {
	//	std::wcout << L"File exists" << std::endl;
	//}
	//else {
	//	std::wcout << L"Files doesn't exist" << std::endl;
	//}






	//RECT rcScreen;

	//MONITORINFO mi;
	//mi.cbSize = sizeof(MONITORINFO);
	//if (GetMonitorInfo(MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTONEAREST),
	//	&mi))
	//{
	//	rcScreen = mi.rcMonitor;

	//}
	//else
	//{
	//	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcScreen, 0);
	//}
	//std::cout << "foo" << std::endl;












	//std::unique_ptr<CUnique> unique;

	//std::unique_ptr<CUnique> foo;
	//foo = std::make_unique<CUnique>();
	//if (foo) {
	//	std::wcout << L"Object is valid" << std::endl;
	//}
	//else {
	//	std::wcout << L"Object is not valid" << std::endl;
	//}


	//std::wstring webcam_name;

	// Get webcam devices
	//HRESULT com_result = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	//if (SUCCEEDED(com_result)) {
	//	get_webcam_name(webcam_name);
	//	CoUninitialize();
	//}

	//std::wcout << "Webcam's name: " << webcam_name << std::endl;




	//std::wstring os_version_info = get_os_version_info();
	//std::wcout << os_version_info << std::endl;




	//DWORDLONG dwlConditionMask = 0;
	//int op = VER_;
	//VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	//VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	//OSVERSIONINFOEX os_version_info = { 0 };
	//os_version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	//if (::VerifyVersionInfo(&os_version_info, VER_MAJORVERSION | VER_MINORVERSION, dwlConditionMask)) {
	//	std::wcout << L"Version exists: " << os_version_info.dwMajorVersion << L", minor: " << os_version_info.dwMinorVersion << std::endl;
	//}





	//std::list<std::wstring> mac_addresses;
	//getMAC(mac_addresses);
	//for (const auto& mac_address : mac_addresses) {
	//	std::wcout << mac_address << std::endl;
	//}

	//::SetConsoleTitle(L"CloudFactory App Usage");
	//::ShowWindow(::GetConsoleWindow(), SW_MAXIMIZE);

	//take_screenshot();

	//Aws::SDKOptions options;
	//Aws::InitAPI(options);
	//{
	//	// Assign these values before running the program
	//	const Aws::String bucket_name = "BUCKET_NAME";
	//	const Aws::String object_name = "OBJECT_NAME";
	//	const std::string file_name = "FILE_NAME_TO_UPLOAD";
	//	const Aws::String region = "";      // Optional

	//	// If a region is specified, use it
	//	Aws::Client::ClientConfiguration clientConfig;
	//	if (!region.empty())
	//		clientConfig.region = region;

	//	// snippet-start:[s3.cpp.put_object_async.invoke.code]
	//	// NOTE: The S3Client object that starts the async operation must 
	//	// continue to exist until the async operation completes.
	//	Aws::S3::S3Client s3Client(clientConfig);

	//	// Put the file into the S3 bucket asynchronously
	//	std::unique_lock<std::mutex> lock(upload_mutex);
	//	if (put_s3_object_async(s3Client,
	//		bucket_name,
	//		object_name,
	//		file_name)) {
	//		// While the upload is in progress, we can perform other tasks.
	//		// For this example, we just wait for the upload to finish.
	//		std::cout << "main: Waiting for file upload to complete..."
	//			<< std::endl;
	//		upload_variable.wait(lock);

	//		// The upload has finished. The S3Client object can be cleaned up 
	//		// now. We can also terminate the program if we wish.
	//		std::cout << "main: File upload completed" << std::endl;
	//	}
	//	// snippet-end:[s3.cpp.put_object_async.invoke.code]
	//}
	//Aws::ShutdownAPI(options);


















	//int32_t exit_code = 120;
	//write_exit_code(exit_code);

	//TIME_ZONE_INFORMATION time_zone_information = { 0 };
	//auto tz_information = ::GetTimeZoneInformation(&time_zone_information);
	//if (tz_information == TIME_ZONE_ID_INVALID) {
	//	std::wcout << "Error retrieving time zone information" << std::endl;
	//}
	//else {
	//	std::wcout << time_zone_information.StandardName << std::endl;
	//}

	//Microsoft::WRL::ComPtr<IProcessLauncher> process_launcher;
	//std::wstring app_guid = L"{D9B6A4D4-504A-4F3A-9468-3C5B95620B24}";

	//bool m_system_time_recently_changed = false;

	//::CoInitializeEx(0, COINIT_MULTITHREADED);

	////
	//CComPtr<ICloudFactoryUpdateCore> cf_update_core;
	//HRESULT result = cf_update_core.CoCreateInstance(__uuidof(CloudFactoryUpdateCoreClass));
	//if (FAILED(result)) {
	//	return 1;
	//}

	//result = ::CoSetProxyBlanket(cf_update_core, RPC_C_AUTHN_DEFAULT,
	//	RPC_C_AUTHZ_DEFAULT, COLE_DEFAULT_PRINCIPAL, RPC_C_AUTHN_LEVEL_DEFAULT,
	//	RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_DEFAULT);
	//if (FAILED(result)) {
	//	return 1;
	//}

	//result = ::CoCreateInstance(__uuidof(ProcessLauncherClass), nullptr,
	//	CLSCTX_ALL, IID_PPV_ARGS(&process_launcher));
	//if (SUCCEEDED(result)) {
	//	if (FAILED(result = process_launcher->LaunchElevatedChangeSystemTime(app_guid.data(),
	//		L"ChangeSystemTime",
	//		2018, 7, 7,
	//		1, 0, 1))) {
	//		return 1;
	//	}
	//	m_system_time_recently_changed = true;
	//}

	/*typedef DWORD(WINAPI * W32TimeSyncNow_t) (
		LPCWSTR szComputer,
		BOOL bWait,
		DWORD dwFlags);

#define W32TimeFlag_No_Soft 0x01 
#define W32TimeFlag_Unknown1 0x02 
#define W32TimeFlag_Rediscover 0x04 
#define W32TimeFlag_Update 0x08 

	HMODULE w32_module = ::LoadLibrary(L"w32time.dll");
	if (!w32_module) {
		std::wcout << L"Couldn't load w32time.dll" << std::endl;
		::system("pause");
		return 0;
	}

	W32TimeSyncNow_t w32_time_sync_now = reinterpret_cast<W32TimeSyncNow_t>(::GetProcAddress(w32_module, "W32TimeSyncNow"));
	if (!w32_time_sync_now) {
		std::wcout << L"Couldn't find the specified procedure address: " << L"W32TimeSyncNow" << std::endl;
		::system("pause");
		return 0;
	}
	if (w32_time_sync_now) {
		DWORD return_code = w32_time_sync_now(NULL,
			TRUE,
			W32TimeFlag_Update |
			W32TimeFlag_Rediscover);
		switch (return_code) {
		case 0:
			std::wcout << L"The command completed successfully.";
			break;
		case 1:
			std::wcout << L"The computer did not resync because no time data was available.";
			break;
		case 2:
			std::wcout << L"The computer did not resync because only stale time data was available.";
			break;
		case 3:
			std::wcout << L"The computer did not resync because the time service was shutting down.";
			break;
		case 4:
			std::wcout << L"The computer did not resync because the required time change was too big.";
			break;
		default:
			std::wcout << L"Unknown error code: " << ::GetLastError();
			break;
		}
	}
	::FreeLibrary(w32_module);*/

	//DWORD comspec_length = ::GetEnvironmentVariable(L"COMSPEC", NULL, 0);
	//if (comspec_length == 0) {
	//	return false;
	//}
	//std::unique_ptr<wchar_t[]> comspec(new wchar_t[comspec_length]);
	//::GetEnvironmentVariable(L"COMSPEC", comspec.get(), comspec_length);

	//// The retrieved value of environment variable 'COMSPEC' could have been
	//// modified so let's check if the path of command line exists
	//wchar_t system_directory_path[MAX_PATH];
	//::GetSystemDirectory(system_directory_path, MAX_PATH);

	//std::wstring command_line_path(system_directory_path);
	//command_line_path.append(L"\\cmd.exe");
	//size_t first_length = command_line_path.length();
	//size_t second_length = command_line_path.length() * sizeof(command_line_path[0]);
	//comspec.reset(new wchar_t[command_line_path.length() * sizeof(command_line_path[0])]);
	//::memcpy(comspec.get(), command_line_path.data(), command_line_path.length() * sizeof(command_line_path[0]));
	//std::wcout << comspec.get() << std::endl;

	/*std::wregex workstream_browser_path_regex(L"^(.*)(CloudFactory\\\\WorkStream\\\\Application\\\\)(workstream.exe$)",
		std::regex_constants::icase);
	std::wstring workstream_browser_path = L"C:\\Program Files (x86)\\CloudFactory\\WorkStream\\Application\\workstream.exe";

	if (std::regex_search(workstream_browser_path, workstream_browser_path_regex)) {
		std::wcout << L"Ya matches" << std::endl;
	}
	else {
		std::wcout << L"Doesn't match" << std::endl;
	}*/


	//wchar_t app_data_path[MAX_PATH];
	//::SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, app_data_path);
	////std::wcout << app_data_path << std::endl;

	//std::wstring user_data_folder_path(app_data_path);
	//user_data_folder_path.append(L"\\CloudFactory\\WorkStream\\User Data");

	//std::wcout << user_data_folder_path.data() << std::endl;

	//string x = "C:\\Users\\lenovo\\Desktop\\QBFdata.txt";
	//string temp = "";
	//for (int i = x.length() - 1; i > 0; i--)
	//{
	//	if (x[i] == '\\')
	//		break;
	//	else
	//		temp.append(&x[i]);
	//}
	//cout << temp << "\n";

	//std::unique_ptr<char *> memory;
	//memory = std::make_unique<char *>(new char[100]);
	//*memory.get() = "hello world";

	//std::string rvalue_ref;
	//std::string hello_world;
	////test_move(hello_world);

	//rvalue_ref = std::move(get_move());

	//get_string(std::move(rvalue_ref));

	//get_string(rvalue_ref);

	//std::cout << rvalue_ref << std::endl;

	//GUITHREADINFO gui_thread_info = {};
	//gui_thread_info.cbSize = sizeof(GUITHREADINFO);

	//while (1)
	//{
	//	if (::GetAsyncKeyState(0x1b) & 0x8000)
	//	{
	//		break;
	//	}

	//	if (::GetGUIThreadInfo(0, &gui_thread_info))
	//	{
	//		std::cout << "Active window: ";
	//		on_app_switched(gui_thread_info.hwndActive);
	//		std::cout << std::endl;

	//		std::cout << "Window that has captured the mouse focus: ";
	//		on_app_switched(gui_thread_info.hwndCapture);
	//		std::cout << std::endl;

	//		std::cout << "Window that has capture the keyboard focus: ";
	//		on_app_switched(gui_thread_info.hwndFocus);
	//		std::cout << std::endl;
	//	}

	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	//}


	//char sentence[] = "C:\\Windows\\SysWOW64\\WerFault.exe";
	//std::transform(sentence, sentence.end(), sentence.begin(), std::tolower);
	//std::string werfault_exe = "werfault.exe";
	//if (sentence.find(werfault_exe) != std::string::npos)
	//{
	//	std::cout << "Found it" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Not found" << std::endl;
	//}

	//std::thread write_app_switch_callback_thread(write_app_switched_callback);
	//std::thread write_app_switch_thread(write_app_switched_thread_proc);
	//std::thread send_data_thread(send_data_thread_proc);
	//std::thread master_thread([&]()
	//{
	//	std::this_thread::sleep_for(std::chrono::seconds(15));
	//	g_app_exit = true;
	//});

	//bool button_pressed = false;
	//while (1)
	//{
	//	if (::GetKeyState(VK_LBUTTON) & 0x8000)
	//	{
	//		button_pressed = true;
	//		std::cout << "L button down\r\n";
	//	}
	//	if((::GetKeyState(VK_LBUTTON) & 0x1))
	//	{
	//		button_pressed = false;
	//		std::cout << "L button released\r\n";
	//	}

	//	if (::GetKeyState(VK_ESCAPE) & 0x8000)
	//		break;

	//	std::this_thread::sleep_for(10ms);
	//}

	//if (::GetAsyncKeyState(VK_UP) & 0x8000)
	//{
	//	std::cout << "up" << std::endl;
	//}

	//std::string buffer;

	//try
	//{
	//	buffer.resize(-1);
	//}
	//catch (std::length_error &)
	//{
	//	std::cout << "Exception caught" << std::endl;
	//}

	//LPTSTR data = L"Hello";
	//LPCTSTR const_data = static_cast<LPCTSTR>(data);
	//std::wcout << const_data << std::endl;

	// Chrome_SystemMessageWindow
	//HWND window_handle = ::FindWindow(L"Chrome_SystemMessageWindow", L"");
	//if (window_handle)
	//{
	//	std::cout << "Found the window" << std::endl;
	//	if (::PostMessage(window_handle, WM_CLOSE, 0, 0))
	//	{
	//		std::cout << "Succeeded" << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "Failed" << std::endl;
	//	}
	//}

	//auto thread_one_handle = std::thread([]() {
	//	std::cout << "Hello from thread one" << std::endl;
	//});
	//auto thread_two_handle = std::thread([]() {
	//	std::cout << "Hello from thread two" << std::endl;
	//});
	//auto thread_three_handle = std::thread([]()
	//{
	//	std::cout << "Hello from thread three" << std::endl;
	//});
	//thread_one_handle.join();
	//thread_two_handle.join();
	//thread_three_handle.join();








	//std::cout << mystrlen("Hello world") << std::endl;

	//char folder_path[MAX_PATH];
	//::SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, folder_path);
	//std::cout << folder_path << std::endl;
	//std::string biz_mail_path(folder_path);
	//biz_mail_path.append("\\BizMail");

	//write_app_switch_callback_thread.join();
	//write_app_switch_thread.join();
	//send_data_thread.join();
	//master_thread.join();

	//auto value = [&]()
	//{
	//	np::data++;
	//	g_data++;
	//};
	//value();
	//std::cout << np::data << std::endl;













	//std::map<int, int> data_map;
	//std::unordered_map<int, int> unordered_data_map;

	//for (int32_t master_counter = 0; master_counter < 10; master_counter++)
	//{
	//	auto started_profiling = START_PROFILING(L"map");

	//	for (int32_t counter = 0; counter < 1000; counter++)
	//	{
	//		data_map.insert(std::pair<int, int>(counter, counter));
	//	}

	//	END_AND_DISPLAY_PROFILING(started_profiling);

	//	started_profiling = START_PROFILING(L"unordered_map");

	//	for (int32_t counter = 0; counter < 1000; counter++)
	//	{
	//		unordered_data_map.insert(std::pair<int, int>(counter, counter));
	//	}

	//	END_AND_DISPLAY_PROFILING(started_profiling);
	//}





	//static const wchar_t* class_name = L"app_usage";
	//WNDCLASSEX wx = {};
	//wx.cbSize = sizeof(WNDCLASSEX);
	//wx.lpfnWndProc = message_wnd_proc;
	//wx.hInstance = ::GetModuleHandle(L"");
	//wx.lpszClassName = class_name;
	//wx.style = CS_HREDRAW | CS_VREDRAW;
	//if (RegisterClassEx(&wx)) {
	//	HWND message_window = CreateWindowEx(0, class_name, L"App Usage", WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, HWND_MESSAGE, NULL, ::GetModuleHandle(L""), NULL);
	//	if (!message_window)
	//		return 1;
	//}
	//// Set windows event hook to monitor foreground window switches
	//HWINEVENTHOOK window_event_hook = ::SetWinEventHook(
	//	EVENT_SYSTEM_FOREGROUND,
	//	EVENT_SYSTEM_FOREGROUND,
	//	nullptr,
	//	win_event_callback,
	//	0,
	//	0,
	//	WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	//MSG message = { 0 };
	//while (::GetMessage(&message, nullptr, 0, 0)) {
	//	::DispatchMessage(&message);
	//}
	//::UnhookWinEvent(window_event_hook);







	//const char werfault_app_exe_regex[] = "(WerFault.exe$)";

	//std::string wer_fault_app_path = "C:\\Windows\\SysWOW64\\WerFault.exe";

	//std::regex wer_fault_regex("(WerFault.exe$)", std::regex_constants::icase);
	//std::smatch regex_match;
	//if (std::regex_search(wer_fault_app_path, regex_match, wer_fault_regex))
	//{
	//	std::cout << "Matches" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Doesn't match" << std::endl;
	//}

	//std::string app_name;

	//char image_path[MAX_PATH * 2] = { '\0' };
	//DWORD buffer_size = sizeof(image_path);
	//app_name.assign(image_path, buffer_size);

	//std::cout << app_name.data() << std::endl;
	//
	//::system("pause");

	::getchar();

	return 0;
}
