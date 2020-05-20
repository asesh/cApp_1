// extension_whitelist.h: 
//

#ifndef EXTENSION_WHITELIST_H
#define EXTENSION_WHITELIST_H

#include "sqlite3.h"

class CExtensionWhitelist {
public:
	static std::unique_ptr<CExtensionWhitelist> Create(const std::wstring& whitelist_db_name);

	CExtensionWhitelist() = default;
	CExtensionWhitelist(const std::wstring& whitelist_db_name);
	~CExtensionWhitelist();

	bool read_whitelist_from_db(std::map<int, std::wstring>& extension_list_from_db); // Reads existing data from the whitelist db
	bool update_whitelist_db(const std::map<int, std::wstring>& extension_list); // Updates the whitelist db by communicating with WM extension

	bool check_whitelist_update(); // Check for updates of whitelist
	bool check_whitelist_integrity(); // Checks if the whitelist has been tampered or not

	bool get_whitelist_version(const std::wstring& whitelist_version,
		int& major_version,
		int& minior_version,
		int& build_version,
		int& patch_version);

protected:
	bool init(const std::wstring& whitelist_db_name); // Creates or opens the whitelist db

protected:

	sqlite3* m_sqlite_db = nullptr;
};

#endif // EXTENSION_WHITELIST_H
