// extension_whitelist.cpp: 
// Copyright (C) 2020. All rights reserved

#include "stdafx.h"
#include "extension_whitelist.h"

constexpr wchar_t db_filename[] = L"";
constexpr wchar_t db_file_checksum[] = L"state";
constexpr char extension_whitelist_table[] = "extension_whitelist";

enum class EReadState {
	TableDoesNotExists
};

std::unique_ptr<CExtensionWhitelist> CExtensionWhitelist::Create(const std::wstring& whitelist_db_name) {
	std::unique_ptr<CExtensionWhitelist> extension_whitelist(new CExtensionWhitelist(whitelist_db_name));
	if (extension_whitelist && extension_whitelist->init(whitelist_db_name)) {
		return std::move(extension_whitelist);
	}
	extension_whitelist.reset();
	return std::move(extension_whitelist);
}

CExtensionWhitelist::CExtensionWhitelist(const std::wstring& whitelist_db_name) {

}

CExtensionWhitelist::~CExtensionWhitelist() {
	if (m_sqlite_db) {
		::sqlite3_close(m_sqlite_db);
	}
}

// Reads existing data from the whitelist db
bool CExtensionWhitelist::read_whitelist_from_db(std::map<int, std::wstring>& extension_list_from_db) {
	char* error_message = nullptr;

	// Get the whitelist data from the DB
	if (::sqlite3_exec(m_sqlite_db, "SELECT id, value from 'extension_whitelist';", nullptr, nullptr, &error_message) != SQLITE_OK) {
		//std::wcerr << L"The extension_id column doesn't exists" << std::endl;
		std::cout << error_message << std::endl;
		delete[] error_message;
		return false;
	}

	// Get all the data from the db
	if (::sqlite3_exec(m_sqlite_db, "SELECT * extension_whitelist", nullptr, nullptr, &error_message) != SQLITE_OK) {
		//std::wcerr << L"Retrieving data from extension_whitelist table" << std::endl;
		std::cout << error_message << std::endl;
		delete[] error_message;
		return false;
	}

	return true;
}

bool CExtensionWhitelist::update_whitelist_db(const std::map<int, std::wstring>& extension_list) {
	// Only update the db if the version is greater than the current version

	std::map<int, std::wstring> existing_data;

	if (extension_list.empty()) {
		return false;
	}

	// Get the current version of whitelist
	if (!read_whitelist_from_db(existing_data)) {
		//std::wcerr << L"The current version of whitelist couldn't be retrieved" << std::endl;
	}

	int major_version = 0, minor_version = 0, build_version = 0, patch_version = 0;

	// Check if there's data in the DB
	if (!existing_data.empty()) {
		// Compare the version of the existing whitelist with the requested one
		if (!get_whitelist_version(existing_data[0], major_version, minor_version, 
			build_version, patch_version)) {
			return false;
		}
	}

	// If there's no data then it's either empty or the DB was recently created
	for (const auto& extension_data : extension_list) {
		std::string extension_value(extension_data.second.begin(), extension_data.second.end());
		std::stringstream query_string;
		query_string << "INSERT INTO extension_whitelist(value) VALUES('" << extension_value << "');";

		char* error_message = nullptr;
		if (::sqlite3_exec(m_sqlite_db, query_string.str().c_str(), nullptr, nullptr, &error_message) != SQLITE_OK) {
			std::cout << error_message << std::endl;
			delete[] error_message;
		}
	}

	return true;
}

bool CExtensionWhitelist::get_whitelist_version(const std::wstring& whitelist_version,
	int& major_version,
	int& minor_version,
	int& build_version,
	int& patch_version) {
	std::wregex regex(LR"(^(\d+).(\d+).(\d+).(\d+)$)");

	std::wsmatch regex_match;

	// Extract the major, minor, build and patch number from the version string
	if (!std::regex_match(whitelist_version, regex_match, regex)) {
		return false;
	}

	// Assign the versions
	major_version = std::stoi(regex_match[1]);
	minor_version = std::stoi(regex_match[2]);
	build_version = std::stoi(regex_match[3]);
	patch_version = std::stoi(regex_match[4]);

	return true;
}

// Initialized the DB for storing and retrieving whitelist. 
// If the DB doesn't exist then it will create one, similarly if the table doens't exists
// then it will create one
bool CExtensionWhitelist::init(const std::wstring& whitelist_db_name) {

	bool db_exists = false;
	if (std::filesystem::exists(whitelist_db_name)) {
		db_exists = true;
	}

	// Open the SQLite db for reading and writing
	if (::sqlite3_open16(whitelist_db_name.c_str(), &m_sqlite_db) != SQLITE_OK) {
		std::wcerr << L"Error opening or creating: " << whitelist_db_name << std::endl;
		return false;
	}

	// Create a table if it doens't exists
	char* error_message = nullptr;
	if (::sqlite3_exec(m_sqlite_db, 
		"CREATE TABLE IF NOT EXISTS extension_whitelist(id INTEGER PRIMARY KEY, value TEXT NOT NULL);", 
		nullptr, nullptr, &error_message) != SQLITE_OK) {
		std::wcerr << L"Either the table doesn't exists or is empty" << std::endl;
		std::cout << error_message << std::endl;
		delete[] error_message;
		return false;
	}

	return true;
}