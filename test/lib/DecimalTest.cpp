#include "gradido_blockchain/lib/Decimal.h"
#include "gtest/gtest.h"

struct DecimalRoundTestSet
{
	DecimalRoundTestSet(Decimal _decimalNumber, uint8_t _precision, const std::string& _finalResult)
		: decimalNumber(_decimalNumber), precision(_precision), finalResult(_finalResult) {}
	Decimal decimalNumber;
	uint8_t precision;
	std::string finalResult;
};

TEST(DecimalTest, roundToPrecision)
{
	std::vector<DecimalRoundTestSet> testSets = {
		{Decimal("0.01281726829"), 9, "0.012817268"},
		{Decimal("0.1281726829"), 8,  "0.12817268"},
		{Decimal("10.2918291022"), 4, "10.2918"},
		{Decimal("171.2182715"), 6, "171.218272"},
		{Decimal("-0.01218219212"), 7, "-0.0121822"},
		{Decimal("-1219.0192818182"), 6, "-1219.019282"}
	};
	
	for (DecimalRoundTestSet testSet : testSets)
	{
		//testSet.decimalNumber.roundToPrecision(testSet.precision);
		EXPECT_EQ(testSet.decimalNumber.toString(testSet.precision), testSet.finalResult);
	}
}