#ifndef UNIT_APPCONFIG_H
#define UNIT_APPCONFIG_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "AppConfig.h"

class CUnit_AppConfig : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_AppConfig);
    CPPUNIT_TEST(Test_XML_Read);
    CPPUNIT_TEST(Test_XML_Write);
    CPPUNIT_TEST_SUITE_END();

public:
    CUnit_AppConfig();

    virtual ~CUnit_AppConfig();

    CUnit_AppConfig(const CUnit_AppConfig& ar);

    CUnit_AppConfig& operator = (const CUnit_AppConfig& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_XML_Read(void);
    void Test_XML_Write(void);
private:
    CAppConfig* m_pAppConfig;
};

#endif

#endif
