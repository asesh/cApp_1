// extension_whitelist_test.h: 
//

#ifndef EXTENSION_WHITELIST_TEST_H
#define EXTENSION_WHITELIST_TEST_H

class CExtensionWhitelistTest : public ::testing::Test, public CExtensionWhitelist {
public:
	virtual void SetUp() override;
	virtual void TearDown() override;

protected:
	std::wstring db_filename;
};

#endif // EXTENSION_WHITELIST_TEST_H