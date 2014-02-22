#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ComBool/RareBool.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace RareBoolUnitTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestEmptyConstructor)
		{
			CrBV a = CrBV();
		}
		TEST_METHOD(TestRepeatedValueConstructor)
		{
			CrBV a = CrBV(100000, true);
			size_t length = a.GetLength();
			Assert::IsTrue(100000 == length);
		}
		TEST_METHOD(TestCopyConstructor)
		{
			CrBV a = CrBV(100000, true);
			CrBV b = CrBV(a);
			size_t length_a = a.GetLength();
			size_t length_b = b.GetLength();
			Assert::IsTrue(length_a == length_b);
		}
		TEST_METHOD(GetBitAt)
		{
			CrBV a = CrBV(100000, true);
			BOOL res = a.GetBitAt(50);
			Assert::IsFalse(res);
		}
		TEST_METHOD(Reverse)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			Assert::IsFalse(false);
		}
		TEST_METHOD(Empty)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			a.Empty();
			Assert::IsTrue(a.GetLength() == 0);
		}
		TEST_METHOD(Zero)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			a.Zero();
			Assert::IsTrue(a.GetForm() == 0);
		}
		TEST_METHOD(One)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			a.One();
			Assert::IsTrue(a.GetForm() == 1);
		}
		TEST_METHOD(TestOperatorConsume1)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			CrBV b = CrBV(100000, true);
			Assert::IsFalse(a > b);
		}
		TEST_METHOD(TestOperator)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			CrBV b = CrBV(100000, true);
			a |= b;
		}
		TEST_METHOD(TestOperator2)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			CrBV b = CrBV(100000, true);
			a &= b;
		}
		TEST_METHOD(TestOperator3)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			CrBV b = CrBV(100000, true);
			a ^= b;
		}
		TEST_METHOD(TestOperator4)
		{
			CrBV a = CrBV(100000, true);
			a.Reverse();
			CrBV b = CrBV(100000, true);
			a -= b;
		}
		TEST_METHOD(ReverseLoadTest)
		{
			CrBV a = CrBV(3700000, true);
			a.Reverse();
		}
		TEST_METHOD(SetBitAtLoadTest)
		{
			CrBV a = CrBV(2000000, true);
			a.Reverse();
			a.SetBitAt(1500000, false);
		}
	};
}