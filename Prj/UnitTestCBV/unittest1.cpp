#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Common/BaseBool.h"
#include <string>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestCBV
{		
	TEST_CLASS(UnitTest)
	{
	public:
		
		TEST_METHOD(TestMethod_AssignDiz)
		{
			CBV a = CBV("00000000");
			unsigned char x1 = 2;
			unsigned char x2 = 4;
			a.AssignDiz(8, &x1, &x2);
			CString c = a.BitChar();
			Assert::IsTrue(L"00000110" == c);
		}

		TEST_METHOD(TestMethod_AssignCon)
		{
			CBV a = CBV("00000000");
			unsigned char x1 = 6;
			unsigned char x2 = 4;
			a.AssignCon(8, &x1, &x2);
			CString c = a.BitChar();
			Assert::IsTrue(L"00000100" == c);
		}

		TEST_METHOD(TestMethod_AssignXor)
		{
			CBV a = CBV("00000000");
			unsigned char x1 = 6;
			unsigned char x2 = 4;
			a.AssignXor(8, &x1, &x2);
			CString c = a.BitChar();
			Assert::IsTrue(L"00000010" == c);
		}

		TEST_METHOD(TestMethod_AssignDif)
		{
			CBV a = CBV("00000000");
			unsigned char x1 = 6;
			unsigned char x2 = 4;
			a.AssignDif(8, &x1, &x2);
			CString c = a.BitChar();
			Assert::IsTrue(L"00000010" == c);
		}
		
		TEST_METHOD(TestMethod_Empty){
			CBV a = CBV("101001");
			Assert::IsFalse(a.IsEmpty());
			a.Empty();
			Assert::IsTrue(a.IsEmpty());

		}

		TEST_METHOD(TestMethod_GetBuffer){
			CBV a = CBV("101010");
			BYTE *z = a.GetBuffer(6);
			Assert::IsFalse(a.IsEmpty());
		}

		TEST_METHOD(TestMethod_ReleaseBuffer){
			CBV a = CBV("101010");
			a.ReleaseBuffer(10);
			Assert::IsTrue(a.GetBitLength() == 10);
		}

		TEST_METHOD(TestMethod_SetSize){
			CBV a = CBV("101010");
			a.SetSize(10,20);
			Assert::IsTrue(a.GetBitLength() == 10);
			Assert::IsTrue(a.GetAllocLength() == 3);
		}
		//Advanced assignment functions
		TEST_METHOD(TestMethod_LeftShiftInPlace)
		{
			CBV v1 = CBV("01110101");
			size_t nshift = 4;
			v1.LeftShiftInPlace(nshift);
			Assert::IsTrue(v1="01010000");
		}
		TEST_METHOD(TestMethod_RightShiftInPlace)
		{
			CBV v1 = CBV("01110101");
			size_t nshift = 8;
			v1.RightShiftInPlace(nshift);
			Assert::IsTrue(v1="00000111");
		}
		TEST_METHOD(TestMethod_DizInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1.DizInPlace(v2,8);
			Assert::IsTrue(v1="11111100");
		}
		TEST_METHOD(TestMethod_ConInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1.ConInPlace(v2,8);
			Assert::IsTrue(v1="11000000");
		}
		TEST_METHOD(TestMethod_Add2InPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1.Add2InPlace(v2,8);
			Assert::IsTrue(v1="00111100");
		}
		TEST_METHOD(TestMethod_ConNotInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1.ConNotInPlace(v2,8);
			Assert::IsTrue(v1="00001100");
		}
		
        TEST_METHOD(TestMethod_IsZero)
		{
			CBV a = CBV("0");

			Assert::IsTrue(a.IsZero() == TRUE);
		}

		TEST_METHOD(TestMethod_IsOne)
		{
			CBV a = CBV("1");

			Assert::IsTrue(a.IsOne() == TRUE);
		}

		TEST_METHOD(TestMethod_EqualeCBVandCBV)
		{
			CBV a = CBV("10101");
			CBV b = CBV("10101");
			Assert::IsTrue(a == b);
		}

		TEST_METHOD(TestMethod_EqualeCBVandBYTE)
		{
			BYTE c[2] = {255,0};
			const BYTE* a = c;

			CBV b = CBV("1111111100");
			Assert::IsTrue(b == a);
		}


		TEST_METHOD(TestMethod_EqualeBYTEandCBV)
		{
			BYTE c[2] = {255,0};
			const BYTE* a = c;

			CBV b = CBV("1111111100");
			Assert::IsTrue(a == b);
		}

		TEST_METHOD(TestMethod_isNotEqualeCBVAndCBV)
		{
			CBV a = CBV("10100010");
			CBV b = CBV("00000101");
			Assert::IsTrue(a != b);
		}

		TEST_METHOD(TestMethod_isNotEqualeCBVandBYTE)
		{

			BYTE c[2] = {4,1};
			const BYTE* a = c;

			CBV b = CBV("00000101");
			Assert::IsTrue(b != a);
		}

		TEST_METHOD(TestMethod_isNotEqualeBYTEandCBV)
		{
			BYTE c[2] = {4,1};
			const BYTE* a = c;

			CBV b = CBV("00000101");
			Assert::IsTrue(a != b);
		}


		TEST_METHOD(TestMethod_GreaterCBVandCBV)
		{
			CBV a = CBV("1111111");
			CBV b = CBV("0110110");
			Assert::IsTrue(a > b);
		}

		TEST_METHOD(TestMethod_GreaterCBVandBYTE)
		{			
			BYTE c[2] = {4,24};
			const BYTE* a = c;

			CBV b = CBV("10001111");
			Assert::IsTrue(b > a);
		}

		TEST_METHOD(TestMethod_GreaterBYTEandCBV)
		{
			BYTE c[2] = {255,1};
			const BYTE* a = c;

			CBV b = CBV("000011110");
			Assert::IsTrue(a > b);
		}

		TEST_METHOD(TestMethod_LessCBVandCBV)
		{
			CBV a = CBV("101011");
			CBV b = CBV("101010");
			Assert::IsTrue(b < a);
		}

		TEST_METHOD(TestMethod_LessCBVandBYTE)
		{
			BYTE b[3] = {255, 31, 123};
			const BYTE* a = b;

			CBV c = CBV("11011");
			Assert::IsTrue(c < a);
		}

		TEST_METHOD(TestMethod_LessBYTEandCBV)
		{
			BYTE c[2] = {0,1};
			const BYTE* a = c;

			CBV b = CBV("111110");
			Assert::IsTrue(a < b);
		}


		TEST_METHOD(TestMethod_GreaterOrEqualeCBVandCBV)
		{
			CBV a = CBV("1111111");
			CBV b = CBV("1111111");
			Assert::IsTrue(a >= b);
		}

		TEST_METHOD(TestMethod_GreaterOrEqualeCBVandBYTE)
		{
			BYTE c[2] = {4,1};
			const BYTE* a = c;

			CBV b = CBV("11111111");
			Assert::IsTrue(b >= a);
		}

		TEST_METHOD(TestMethod_GreaterOrEqualeBYTEandCBV)
		{
			BYTE c[2] = {255,1};
			const BYTE* a = c;

			CBV b = CBV("000110");
			Assert::IsTrue(a >= b);
		}

		TEST_METHOD(TestMethod_LessOrEqualeCBVandCBV)
		{
			CBV a = CBV("0000010");
			CBV b = CBV("0011010");
			Assert::IsTrue(a <= b);
		}

		TEST_METHOD(TestMethod_LessOrEqualeCBVandBYTE)
		{
			BYTE c[2] = {255,13};
			const BYTE* a = c;

			CBV b = CBV("0011");
			Assert::IsTrue(b <= a);
		}

		TEST_METHOD(TestMethod_LessOrEqualeBYTEandCBV)
		{
			BYTE c[2] = {4,1};
			const BYTE* a = c;

			CBV b = CBV("11111111");
			Assert::IsTrue(a <= b);
		}
	};
}