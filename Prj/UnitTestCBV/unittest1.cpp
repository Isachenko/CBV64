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

		//--------------GenRbv tests --------------
		
		TEST_METHOD(TestMethod_GenRbv) {
			size_t n = 100;
			CBV v = v.GenRbv(n);

			size_t a = 0, b = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
				if (v[i] == 1)
					b++;
			}
			Assert::IsTrue((a+b) == 100);
			Assert::IsTrue(a == b);
		}

		TEST_METHOD(TestMethod_GenRbvN) {
			size_t n = 100;
			CBV v = v.GenRbvN(n);

			size_t a = 0, b = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
				if (v[i] == 1)
					b++;
			}
			Assert::IsTrue((a+b) == 100);
			Assert::IsTrue(a == b);
		}

		TEST_METHOD(TestMethod_GenRbvMid) {
			int n = 100;

			CBV v = v.GenRbvMid(n, n+10);
			size_t a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 1)
					a++;
			}
			Assert::IsTrue(a == 100);

			CBV v1 = v1.GenRbvMid(n, 0);
			a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
			}
			Assert::IsTrue(a == 100);

			CBV v2 = v2.GenRbvMid(n, 30);
			size_t b = 0;
			a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
				if (v[i] == 1)
					b++;
			}
			Assert::IsTrue((a+b) == 100);
			Assert::IsTrue(b == 50);
		}

		TEST_METHOD(TestMethod_GenRbvFix) {
			int n = 100;

			CBV v = v.GenRbvFix(n, n+10);
			size_t a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 1)
					a++;
			}
			Assert::IsTrue(a == 100);

			CBV v1 = v1.GenRbvMid(n, 0);
			a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
			}
			Assert::IsTrue(a == 100);

			CBV v2 = v2.GenRbvMid(n, 30);
			size_t b = 0;
			a = 0;
			for (size_t i = 0; i < n; i++) {
				if (v[i] == 0)
					a++;
				if (v[i] == 1)
					b++;
			}
			Assert::IsTrue((a+b) == 100);
			Assert::IsTrue(b == 30);
		}

		//------------------------------------------

	};
}