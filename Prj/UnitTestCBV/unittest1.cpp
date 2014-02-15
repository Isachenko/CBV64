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
		//-----------------------------------
	};
}