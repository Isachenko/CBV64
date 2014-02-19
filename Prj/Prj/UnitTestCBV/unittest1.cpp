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
			Assert::IsTrue(ans == L"__*_*___________________________________________________________");
		}

		TEST_METHOD(TestMethod_Extract)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CsBV ans = bv.Extract(1, 10);					
			Assert::IsTrue(ans.GetBitAt(2) == 1);			
			Assert::IsTrue(ans.GetBitLength() == 10);
		}

		TEST_METHOD(TestMethod_SetBitAt)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			bv.SetBitAt(10, 1);
			bv.SetBitAt(15, 1);
			bv.SetBitAt(15, 0);
			bv.SetBitAt(3, 0);
			Assert::IsTrue(bv.GetBitAt(10) == 1);			
			Assert::IsTrue(bv.GetBitAt(15) == 0);		
			Assert::IsTrue(bv.GetBitAt(3) == 0);
			Assert::IsTrue(bv.GetBitAt(0) == 1);
		}

		TEST_METHOD(TestMethod_operatorAssign)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			bv = 4;
			Assert::IsTrue(bv.GetBitAt(61) == 1);
			bv = "01011";
			Assert::IsTrue(bv.GetBitAt(4) == 1);
			CsBV bv1 = CsBV((size_t)value, (__int8)bitLenght);
			bv = bv1;			
			Assert::IsTrue(bv.GetBitAt(3) == 1);
		}

		TEST_METHOD(TestMethod_operatorOrAssign)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			size_t value1 = ((size_t)1 << 62) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CsBV bv1 = CsBV((size_t)value1, (__int8)bitLenght);
			CString str1 = bv.BitChar('*', '_');
			bv |= bv1;
			CString str2 = bv.BitChar('*', '_');
			bv |= 4;
			CString str3 = bv.BitChar('*', '_');
			Assert::IsTrue(bv.GetBitAt(0) == 1);
			Assert::IsTrue(bv.GetBitAt(1) == 1);
			Assert::IsTrue(bv.GetBitAt(3) == 1);
			Assert::IsTrue(bv.GetBitAt(61) == 1);
			Assert::IsTrue(bv.GetBitAt(45) == 0);
		}
		
		TEST_METHOD(TestMethod_operatorAndssign)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60) | ((size_t)1 << 2);
			size_t value1 = ((size_t)1 << 62) | ((size_t)1 << 60) | ((size_t)1 << 2);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CsBV bv1 = CsBV((size_t)value1, (__int8)bitLenght);
			CString str1 = bv.BitChar('*', '_');
			bv &= bv1;
			CString str2 = bv.BitChar('*', '_');
			bv &= 6;
			CString str3 = bv.BitChar('*', '_');
			Assert::IsTrue(bv.GetBitAt(0) == 0);
			Assert::IsTrue(bv.GetBitAt(1) == 0);
			Assert::IsTrue(bv.GetBitAt(3) == 0);
			Assert::IsTrue(bv.GetBitAt(61) == 1);
			Assert::IsTrue(bv.GetBitAt(45) == 0);
		}
		
		TEST_METHOD(TestMethod_operatorXorAssign)
		{
			CsBV bv1 = "101000";
			CsBV bv2 = "100110";
			CString str1 = bv1.BitChar('*', '_');
			CString str2 = bv2.BitChar('*', '_');
			bv1 ^= bv2;
			CString str3 = bv1.BitChar('*', '_');
			Assert::IsTrue(str3 == "__***_");
		}
		
		TEST_METHOD(TestMethod_operatorMinsuAssign)
		{			
			CsBV bv1 = "1110011";
			CsBV bv2 = "1101001";
			bv1 -= bv2;
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0010010");
			bv1 -= (size_t)1 << 58;
			str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0010000");
		}
		
		TEST_METHOD(TestMethod_operatorShiftAssign)
		{			
			CsBV bv1 = "1110011";
			bv1 <<= 2;
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "1001100");
			bv1 >>= 3;
			str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0001001");
		}

	};
}