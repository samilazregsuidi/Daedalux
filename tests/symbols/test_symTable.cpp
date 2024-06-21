#include <gtest/gtest.h>
#include "../../src/core/symbol/symTable.hpp"
#include "../../src/core/symbol/symbol.hpp"
#include "../../src/promela/symbol/vardef/intSymNode.hpp"
#include "../../src/promela/symbol/vardef/pidSymNode.hpp"
#include "../../src/promela/symbol/vardef/boolSymNode.hpp"

class SymTableTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code that will be called before each test
        globalTable = new symTable("global", nullptr);
    }

    void TearDown() override {
        // Common teardown code that will be called after each test
        delete globalTable;
    }

    symTable* globalTable;
};

TEST_F(SymTableTestFixture, CreateSubTableTest) {
    symTable* subTable = globalTable->createSubTable("subTable");

    // Check that the subTable was created correctly
    EXPECT_EQ(subTable->getNameSpace(), "subTable");
    EXPECT_EQ(subTable->prevSymTab(), globalTable);
}

TEST_F(SymTableTestFixture, InsertAndLookupTest) {
    symbol* symbol1 = new intSymNode(0, "symbol1");
    symbol* symbol2 = new pidSymNode(0, "symbol2");

    globalTable->insert(symbol1);
    globalTable->insert(symbol2);

    // Check that the symbols were inserted correctly
    EXPECT_EQ(globalTable->lookup("symbol1"), symbol1);
    EXPECT_EQ(globalTable->lookup("symbol2"), symbol2);
}

TEST_F(SymTableTestFixture, RemoveTest) {
    symbol* symbol1 = new intSymNode(0, "symbol1");
    symbol* symbol2 = new pidSymNode(0, "symbol2");

    globalTable->insert(symbol1);
    globalTable->insert(symbol2);

    globalTable->remove("symbol1");

    // Check that symbol1 was removed
    EXPECT_EQ(globalTable->lookup("symbol1"), nullptr);
    // Check that symbol2 is still present
    EXPECT_EQ(globalTable->lookup("symbol2"), symbol2);
}