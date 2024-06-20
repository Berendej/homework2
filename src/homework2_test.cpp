#define BOOST_TEST_MODULE homework2 test

#include <boost/test/included/unit_test.hpp>

bool test_md5()
{
	system("cat ip_filter.tsv | ./homework2 | md5sum > calc.txt");
    int cmp_result = system("cmp etalon.txt calc.txt");
	return (0 == cmp_result);
}

BOOST_AUTO_TEST_CASE(md5_test)
{
    BOOST_CHECK( test_md5()  );
}
