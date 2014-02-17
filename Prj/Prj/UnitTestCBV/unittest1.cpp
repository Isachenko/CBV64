#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../ComBool/ShortBool.h"
#include <string>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestCBV
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod_Constructors)
		{
			CsBV bv1 = CsBV();
			CsBV bv2 = CsBV(bv1);
			CsBV bv3 = CsBV(64, true);
			CsBV bv4 = CsBV(64, false);
			CsBV bv5 = CsBV((size_t)9348357, (__int8)45);
			CsBV bv6 = CsBV("1111111111111111111111111111111100000111000");
		}

		TEST_METHOD(TestMethod_GetBitLenght)
		{
			int bitLenght = 55;
			CsBV bv = CsBV((size_t)3438473, (__int8)bitLenght);
			int ans = bv.GetBitLength();
			Assert::IsTrue(ans == bitLenght);
		}

		TEST_METHOD(TestMethod_operatorSize_t1)
		{
			int bitLenght = 64;
			size_t value = 3438473;
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			int ans = size_t(bv);			
			Assert::IsTrue(ans == value);
		}

		TEST_METHOD(TestMethod_operatorSize_t2)
		{
			int bitLenght = 66;
			size_t value = 3438473;
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			int ans = size_t(bv);			
			Assert::IsTrue(ans == 0);
		}

		TEST_METHOD(TestMethod_GetBitAt)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			int ans1 = bv.GetBitAt(0);	
			int ans2 = bv.GetBitAt(3);			
			int ans3 = bv.GetBitAt(23);
			Assert::IsTrue(ans1 == 1);
			Assert::IsTrue(ans2 == 1);
			Assert::IsTrue(ans3 == 0);
		}

		TEST_METHOD(TestMethod_BitChar)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 61) | ((size_t)1 << 59);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CString ans = bv.BitChar('*', '_');			
			Assert::IsTrue(ans == L"__*_*________________________________________________________");
		}

	};
}