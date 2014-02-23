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

		//-----------------------------------------------------------

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
		
		TEST_METHOD(TestMethod_operatorAndAssign)
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

		//----------------------------------------------------------

		TEST_METHOD(TestMethod_operatorOr)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60);
			size_t value1 = ((size_t)1 << 62) | ((size_t)1 << 60);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CsBV bv1 = CsBV((size_t)value1, (__int8)bitLenght);
			CString str1 = bv.BitChar('*', '_');
			bv = bv | bv1;
			CString str2 = bv.BitChar('*', '_');
			bv =  bv | (size_t)4;
			bv =  (size_t)8 | bv;
			CString str3 = bv.BitChar('*', '_');
			Assert::IsTrue(bv.GetBitAt(0) == 1);
			Assert::IsTrue(bv.GetBitAt(1) == 1);
			Assert::IsTrue(bv.GetBitAt(3) == 1);
			Assert::IsTrue(bv.GetBitAt(61) == 1);
			Assert::IsTrue(bv.GetBitAt(60) == 1);
			Assert::IsTrue(bv.GetBitAt(45) == 0);
		}
		
		TEST_METHOD(TestMethod_operatorAnd)
		{
			int bitLenght = 64;
			size_t value = ((size_t)1 << 63) | ((size_t)1 << 60) | ((size_t)1 << 2);
			size_t value1 = ((size_t)1 << 62) | ((size_t)1 << 60) | ((size_t)1 << 2);
			CsBV bv = CsBV((size_t)value, (__int8)bitLenght);
			CsBV bv1 = CsBV((size_t)value1, (__int8)bitLenght);
			CString str1 = bv.BitChar('*', '_');
			bv = bv& bv1;
			CString str2 = bv.BitChar('*', '_');
			bv = bv & (size_t)6;
			bv = (size_t)6 & bv;
			CString str3 = bv.BitChar('*', '_');
			Assert::IsTrue(bv.GetBitAt(0) == 0);
			Assert::IsTrue(bv.GetBitAt(1) == 0);
			Assert::IsTrue(bv.GetBitAt(3) == 0);
			Assert::IsTrue(bv.GetBitAt(61) == 1);
			Assert::IsTrue(bv.GetBitAt(45) == 0);
		}
		
		TEST_METHOD(TestMethod_operatorXor)
		{
			CsBV bv1 = "101000";
			CsBV bv2 = "100110";
			CString str1 = bv1.BitChar('*', '_');
			CString str2 = bv2.BitChar('*', '_');
			bv1 = bv1 ^ bv2;
			CString str3 = bv1.BitChar('*', '_');
			Assert::IsTrue(str3 == "__***_");
		}
		
		TEST_METHOD(TestMethod_operatorMinus)
		{			
			CsBV bv1 = "1110011";
			CsBV bv2 = "1101001";
			bv1 = bv1 - bv2;
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0010010");
			bv1 = bv1 - ((size_t)1 << 58);
			str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0010000");
		}

		TEST_METHOD(TestMethod_operatorNot)
		{			
			CsBV bv1 = "1110011";
			bv1 = ~bv1;
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0001100");
		}

		TEST_METHOD(TestMethod_Invert)
		{			
			CsBV bv = CsBV(64, true);
			bv.Invert(12);
			CString ans = bv.BitChar('*', '_');			
			Assert::IsTrue(ans == L"************************************************************__**");
		}

		//------------------------
		
		TEST_METHOD(TestMethod_operatorShift)
		{			
			CsBV bv1 = "1110011";
			bv1 = bv1 << (__int8)2;
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "1001100");
			bv1 = bv1 >> (__int8)3;
			str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0001001");
		}

		TEST_METHOD(TestMethod_LoopShift)
		{			
			CsBV bv1 = "1110011";
			bv1.LoopLeftShift(2);
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "1001111");
			bv1.LoopRightShift(3);
			str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "1111001");
		}

		TEST_METHOD(TestMethod_LoopRightShift)
		{			
			CsBV bv1 = "1110011010101111";
			bv1.LoopRightShift(5);
			CString str1 = bv1.BitChar();
			Assert::IsTrue(str1 == "0111111100110101");
		}
		
		TEST_METHOD(TestMethod_CountBit)
		{			
			CsBV bv1 = "1110011010101111";
			bv1.LoopRightShift(5);
			__int8 ans = bv1.CountBit();
			Assert::IsTrue(ans == 11);
		}

		TEST_METHOD(TestMethod_LeftOne)
		{			
			CsBV bv1 = "1110011010101111";
			_int8 ans = bv1.LeftOne();
			Assert::IsTrue(ans == 0);
			ans = bv1.LeftOne(2);
			Assert::IsTrue(ans == 5);
			ans = bv1.LeftOne(6);
			Assert::IsTrue(ans == 8);
		}

		TEST_METHOD(TestMethod_RightOne)
		{			
			CsBV bv1 = "1110011010101111";
			_int8 ans = bv1.RightOne();
			Assert::IsTrue(ans == 15);
			ans = bv1.RightOne(5);
			Assert::IsTrue(ans == 2);
			ans = bv1.RightOne(8);
			Assert::IsTrue(ans == 6);
		}

		//---------------------------------------		

		TEST_METHOD(TestMethod_Concat)
		{			
			CsBV bv1 = "1110011010101111";
			CsBV bv2 = "00000111";
			bv1.Concat(bv2);
			CString ans = bv1.BitChar();
			Assert::IsTrue(ans == "111001101010111100000111");
			bv1.Concat(1);
			ans = bv1.BitChar();
			Assert::IsTrue(ans == "1110011010101111000001111");
			bv1.Concat(sOB[1],2);
			ans = bv1.BitChar();
			Assert::IsTrue(ans == "111001101010111100000111101");
		}

		//-------------------------------------------------		

		TEST_METHOD(TestMethod_IsEmptyIsZeroIsOne)
		{			
			CsBV bv1 = "1110011010101111";
			Assert::IsTrue(!bv1.IsEmpty());
			Assert::IsTrue(!bv1.IsZero());
			Assert::IsTrue(!bv1.IsOne());
			bv1 = "111111";
			Assert::IsTrue(!bv1.IsEmpty());
			Assert::IsTrue(!bv1.IsZero());
			Assert::IsTrue(bv1.IsOne());
			bv1 = "0000";
			Assert::IsTrue(!bv1.IsEmpty());
			Assert::IsTrue(bv1.IsZero());
			Assert::IsTrue(!bv1.IsOne());
			bv1 = "";
			Assert::IsTrue(bv1.IsEmpty());
			Assert::IsTrue(!bv1.IsZero());
			Assert::IsTrue(!bv1.IsOne());
		}

		//----------------------------------------		

		TEST_METHOD(TestMethod_operatorEqual)
		{			
			CsBV bv1 = "10000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000";
			Assert::IsFalse(bv1 == bv2);
			Assert::IsTrue(bv3 == bv2);
			Assert::IsFalse(bv1 == (size_t)3000000000);
			Assert::IsTrue(bv1 == sOB[0]);
			Assert::IsFalse((size_t)36336 == bv2);
			Assert::IsTrue(sOB[2] == bv4);
		}

		TEST_METHOD(TestMethod_operatorNotEqual)
		{			
			CsBV bv1 = "10000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000";
			Assert::IsTrue(bv1 != bv2);
			Assert::IsFalse(bv3 != bv2);
			Assert::IsTrue(bv1 != (size_t)3000000000);
			Assert::IsFalse(bv1 != sOB[0]);
			Assert::IsTrue((size_t)36336 != bv2);
			Assert::IsFalse(sOB[2] != bv4);
		}

		TEST_METHOD(TestMethod_operatorGreater)
		{			
			CsBV bv1 = "1000000001000000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000000000000";
			Assert::IsTrue(bv2 > bv1);
			Assert::IsFalse(bv3 > bv2);
			Assert::IsFalse(bv1 > (sOB[0] | sOB[3]));
			Assert::IsTrue(bv1 > sOB[0]);
			Assert::IsFalse((size_t)36336846523653 > bv2);
			Assert::IsTrue((sOB[1] | sOB[2]) > bv4);
		}

		TEST_METHOD(TestMethod_operatorLess)
		{			
			CsBV bv1 = "1000000001000000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000000000000";
			Assert::IsFalse(bv2 < bv1);
			Assert::IsFalse(bv3 < bv2);
			Assert::IsFalse(bv1 < (sOB[0] | sOB[3]));
			Assert::IsFalse(bv1 < sOB[0]);
			Assert::IsTrue((size_t)0 < bv2);
			Assert::IsFalse((sOB[1] | sOB[2]) < bv4);
		}

		TEST_METHOD(TestMethod_operatorGreaterOrEqual)
		{			
			CsBV bv1 = "1000000001000000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000000000000";
			Assert::IsTrue(bv2 >= bv1);
			Assert::IsTrue(bv3 >= bv2);
			Assert::IsFalse(bv1 >= (sOB[0] | sOB[3]));
			Assert::IsTrue(bv1 >= sOB[0]);
			Assert::IsFalse((size_t)36336846523653 >= bv2);
			Assert::IsTrue((sOB[1] | sOB[2]) >= bv4);
		}

		TEST_METHOD(TestMethod_operatorLessOrEqual)
		{			
			CsBV bv1 = "1000000001000000";
			CsBV bv2 = "1010101011101010";
			CsBV bv3 = "1010101011101010";
			CsBV bv4 = "0010000000000000";
			Assert::IsFalse(bv2 <= bv1);
			Assert::IsTrue(bv3 <= bv2);
			Assert::IsFalse(bv1 <= (sOB[0] | sOB[3]));
			Assert::IsFalse(bv1 <= sOB[0]);
			Assert::IsTrue((size_t)0 <= bv2);
			Assert::IsFalse((sOB[1] | sOB[2]) <= bv4);
		}

		//-------------------------------

		TEST_METHOD(TestMethod_Empty)
		{			
			CsBV bv = "1000000001000000";
			bv.Empty();
			Assert::IsTrue(bv.GetBitLength() == 0);
			Assert::IsTrue(bv == (size_t)0);
		}		

		TEST_METHOD(TestMethod_SetSize)
		{			
			CsBV bv = "100110";
			bv.SetSize(3);
			Assert::IsTrue(bv.GetBitLength() == 3);
			Assert::IsTrue(bv == "100");
		}

		TEST_METHOD(TestMethod_CharBit)
		{			
			CsBV bv = "100110";
			bv.SetSize(3);
			Assert::IsTrue(bv.GetBitLength() == 3);
			Assert::IsTrue(bv == "100");
		}

		TEST_METHOD(TestMethod_CsBM_SetSize)
		{
			CsBM m = CsBM("");
			m.SetSize(33, 4, 4);
			Assert::IsTrue(m.GetCountC() == 4);
		}

		TEST_METHOD(TestMethod_BitCharstr)
		{

			CsBM bm = CsBM("1010100101");
			CsBM bm1 = CsBM("1010100101");
			
			CStringArray s1;
			CString s2;
			CString s3;
			bm.BitChar(s1);
			s2 = bm.BitChar();
			s3 = (*s1.GetData());
			Assert::IsFalse(strcmp(s2.GetBuffer(), s3.GetBuffer()) == 0);
		}

		TEST_METHOD(TestMethod_GetRowBv){
			CsBM bm = CsBM("10100101\r\n1010010");
			CsBV bv = CsBV("10100101");
			Assert::IsTrue(bv == bm.GetRowBv(0));
		}

		TEST_METHOD(TestMethod_GetRowBvmask){
			CsBM bm = CsBM("11111101\r\n1010010");
			CsBV bv = CsBV("11000000");
			Assert::IsTrue(bv == bm.GetRowBv(0, 0xc000000000000000));
		}

		TEST_METHOD(TestMethod_GetColumn){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBV bv = CsBV("1111");
			Assert::IsTrue(bv == bm.GetColumnBv(1));						
		}

		TEST_METHOD(TestMethod_ExtractRows){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBM bm1 = bm.ExtractRows(1,2);
			Assert::IsTrue(bm1.GetCountR() == 2);		
		}

		TEST_METHOD(CsBMTestMethod_Extract){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBM bm1 = bm.Extract(1,1,1,2);
			Assert::IsTrue(bm1.GetCountC() == 2 && bm1.GetCountR() == 1);
		}//stoped at minor method

		TEST_METHOD(CsBVTestMethod_GetBitAt)
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

		TEST_METHOD(CsBMTestMethod_GetRowBvmask){
			CsBM bm = CsBM("11111101\r\n1010010");
			CsBV bv = CsBV("11000000");
			Assert::IsTrue(bv == bm.GetRowBv(0, 0xc000000000000000));
		}

		TEST_METHOD(CsBMTestMethod_GetColumn){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBV bv = CsBV("1111");
			Assert::IsTrue(bv == bm.GetColumnBv(1));						
		}

		TEST_METHOD(CsBMTestMethod_ExtractRows){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBM bm1 = bm.ExtractRows(1,2);
			Assert::IsTrue(bm1.GetCountR() == 2);		
		}

		TEST_METHOD(CsBMCsBMTestMethod_Extract){
			CsBM bm = CsBM("1101\r\n1101\r\n1101\r\n1101");
			CsBM bm1 = bm.Extract(1,1,1,2);
			Assert::IsTrue(bm1.GetCountC() == 2 && bm1.GetCountR() == 1);
		}//stoped at minor method

		TEST_METHOD(CsBVTestMethod_Empty)
		{			
			CsBV bv = "1000000001000000";
			bv.Empty();
			Assert::IsTrue(bv.GetBitLength() == 0);
			Assert::IsTrue(bv == (size_t)0);
		}		

		TEST_METHOD(CsBVTestMethod_SetSize)
		{			
			CsBV bv = "100110";
			bv.SetSize(3);
			Assert::IsTrue(bv.GetBitLength() == 3);
			Assert::IsTrue(bv == "100");
		}

		TEST_METHOD(CsBVTestMethod_CharBit)
		{			
			CsBV bv = "100110";
			bv.SetSize(3);
			Assert::IsTrue(bv.GetBitLength() == 3);
			Assert::IsTrue(bv == "100");
		}

		TEST_METHOD(CsBMTestMethod_CsBM_SetSize)
		{
			CsBM m = CsBM("");
			m.SetSize(33, 4, 4);
			Assert::IsTrue(m.GetCountC() == 4);
		}

		TEST_METHOD(CsBMTestMethod_BitCharstr){
			CsBM bm = CsBM("1010100101");
			CStringArray s1;
			CString s2;
			CString s3;
			bm.BitChar(s1);
			s2 = bm.BitChar();
			s3 = (*s1.GetData());
			Assert::IsFalse(strcmp(s2.GetBuffer(), s3.GetBuffer()) == 0);
		}

		TEST_METHOD(CsBMTestMethod_GetRowBv){
			CsBM bm = CsBM("10100101\r\n1010010");
			CsBV bv = CsBV("10100101");
			Assert::IsTrue(bv == bm.GetRowBv(0));
		}
	};
}