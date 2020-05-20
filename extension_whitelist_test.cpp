// extension_whitelist_test.cpp: 
// 

#include "stdafx.h"
#include "extension_whitelist.h"
#include "extension_whitelist_test.h"

void CExtensionWhitelistTest::SetUp() {
	db_filename = L"extension_whitelist";
}

void CExtensionWhitelistTest::TearDown() {
	// Remove the DB on tear down
	if (std::filesystem::exists(std::filesystem::path(db_filename))) {
		std::filesystem::remove(std::filesystem::path(db_filename));
	}
}

TEST_F(CExtensionWhitelistTest, ReadEmptyWhitelistDb) {
	ASSERT_TRUE(init(db_filename));

	std::map<int, std::wstring> extension_whitelist;
	ASSERT_FALSE(read_whitelist_from_db(extension_whitelist));
	EXPECT_TRUE(extension_whitelist.empty());
}

TEST_F(CExtensionWhitelistTest, ReadNonEmptyWhitelistDb) {
	ASSERT_TRUE(init(db_filename));

	// Fill the db with whitelist data
	std::map<int, std::wstring> db_data;
	db_data[0] = L"1.2.3.4";
	db_data[1] = L"mjnbclmflcpookeapghfhapeffmpodij";
	db_data[2] = L"bhbcbkonalnjkflmdkdodieehnmmeknp";
	ASSERT_TRUE(update_whitelist_db(db_data));

	std::map<int, std::wstring>  read_extension_whitelist;
	ASSERT_TRUE(read_whitelist_from_db(read_extension_whitelist));
	EXPECT_EQ(db_data[0], read_extension_whitelist[0]);
	EXPECT_EQ(db_data[1], read_extension_whitelist[1]);
	EXPECT_EQ(db_data[2], read_extension_whitelist[2]);
}

TEST_F(CExtensionWhitelistTest, GetWhitelistVersion) {
	int major_version, minor_version, build_version, patch_version;

	std::wstring whitelist_version = L"1.2.3.4";
	ASSERT_TRUE(get_whitelist_version(whitelist_version, major_version, minor_version, build_version, patch_version));
	EXPECT_EQ(major_version, 1);
	EXPECT_EQ(minor_version, 2);
	EXPECT_EQ(build_version, 3);
	EXPECT_EQ(patch_version, 4);

	whitelist_version = L"100.200.300.400";
	ASSERT_TRUE(get_whitelist_version(whitelist_version, major_version, minor_version, build_version, patch_version));
	EXPECT_EQ(major_version, 100);
	EXPECT_EQ(minor_version, 200);
	EXPECT_EQ(build_version, 300);
	EXPECT_EQ(patch_version, 400);


	// Show return false for the tests below
	whitelist_version = L"0";
	ASSERT_FALSE(get_whitelist_version(whitelist_version, major_version, minor_version, build_version, patch_version));

	whitelist_version = L"0.0";
	ASSERT_FALSE(get_whitelist_version(whitelist_version, major_version, minor_version, build_version, patch_version));

	whitelist_version = L"0.0.0";
	ASSERT_FALSE(get_whitelist_version(whitelist_version, major_version, minor_version, build_version, patch_version));
}
