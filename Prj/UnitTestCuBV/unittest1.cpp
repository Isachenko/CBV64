#include "stdafx.h"
#include "CppUnitTest.h"
#include "../ComBool/UlongBool.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestCubv
{		
	TEST_CLASS(UnitTestCubv)
	{
	public:
		
		TEST_METHOD(TestMethod_GetBitLength)
		{
			CuBV cubv("100101101101");
            Assert::IsTrue(12 == cubv.GetBitLength());
		}

        TEST_METHOD(TestMethod_GetLongLength)
        {
            CuBV cubv("100101101101");
            Assert::IsTrue(1 == cubv.GetLongLength());
        }

        TEST_METHOD(TestMethod_GetAllocLength)
        {
            CuBV cubv("100101101101");
            Assert::IsTrue(1 == cubv.GetAllocLength());
        }

        TEST_METHOD(TestMethod_OperatorSquareBrackets)
        {
            ptrdiff_t arr[] = {0x74394abd, 0x129342bd, 0xacde45bd, 0x3295be5a};
            CuBV cubv(arr, 256);
            Assert::IsTrue(0x74394abd == cubv[0] && 0x129342bd == cubv[1] && 
                0xacde45bd == cubv[2] && 0x3295be5a == cubv[3]);
        }

        TEST_METHOD(TestMethod_GetLongAt)
        {
            ptrdiff_t arr[] = {0x74394abd, 0x129342bd, 0xacde45bd, 0x3295be5a};
            CuBV cubv(arr, 256);
            Assert::IsTrue(0x74394abd == cubv.GetLongAt(0));
            Assert::IsTrue(0xacde45bd == cubv.GetLongAt(2));
        }

        TEST_METHOD(TestMethod_GetBitAt)
        {
            CuBV cubv("100101101101");
            Assert::IsTrue(TRUE == cubv.GetBitAt(5));
            Assert::IsTrue(FALSE == cubv.GetBitAt(10));
        }

        TEST_METHOD(TestMethod_BitChar)
        {
            ptrdiff_t arr[] = {0x3b67a92d4e1ce83b, 0x4bc290d7e1c5b83d};
            CuBV cubv(arr, 128);
            Assert::IsTrue(cubv.BitChar() == L"00111011011001111010100100101101010011100001110011101000001110110100101111000010100100001101011111100001110001011011100000111101");
        }

        TEST_METHOD(TestMethod_SetBitAt)
        {
            CuBV cubv("10010110110101");
            cubv.SetBitAt(6, FALSE);
            Assert::IsTrue(FALSE == cubv.GetBitAt(6));
        }

        TEST_METHOD(TestMethod_SetLongAt)
        {
            CuBV cubv("10010110110101");
            cubv.SetLongAt(0, 0xf6);
            Assert::IsTrue(0xf6 == cubv.GetLongAt(0));
        }

        TEST_METHOD(TestMethod_AssignmentCubv)
        {
            CuBV source("1001000101110101");
            CuBV destination = source;
            Assert::IsTrue(L"1001000101110101" == destination.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentWordArray)
        {
            ptrdiff_t arr[] = {0x74394abd, 0x129342bd, 0xacde45bd, 0x3295be5a};
            CuBV cubv(256);
            cubv = arr;
            Assert::IsTrue(0x74394abd == cubv[0] && 0x129342bd == cubv[1] && 
                           0xacde45bd == cubv[2] && 0x3295be5a == cubv[3]);
        }

        TEST_METHOD(TestMethod_AssignmentCharArray)
        {
            CuBV cubv = "1001000101110101";
            Assert::IsTrue(L"1001000101110101" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_Zero)
        {
            CuBV cubv("10110101010");
            cubv.Zero();
            Assert::IsTrue(L"00000000000" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_One)
        {
            CuBV cubv("10110101010");
            cubv.One();
            Assert::IsTrue(L"11111111111" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentCubvOr)
        {
            CuBV source("1001100101");
            CuBV destination("0111010110");
            destination |= source;
            Assert::IsTrue(L"1111110111" == destination.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentCubvAnd)
        {
            CuBV source("1001100101");
            CuBV destination("0111010110");
            destination &= source;
            Assert::IsTrue(L"0001000100" == destination.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentCubvXor)
        {
            CuBV source("1001100101");
            CuBV destination("0111010110");
            destination ^= source;
            Assert::IsTrue(L"1110110011" == destination.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentLeftShift)
        {
            CuBV cubv("0111010110");
            cubv <<= 3;
            Assert::IsTrue(L"1010110000" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_AssignmentRightShift)
        {
            CuBV cubv("0111010110");
            cubv >>= 3;
            Assert::IsTrue(L"0000111010" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_CubvOrCubv)
        {
            CuBV one("011101010101");
            CuBV two("101010110100");
            CuBV res = one | two;
            Assert::IsTrue(L"111111110101" == res.BitChar());
        }

        TEST_METHOD(TestMethod_CubvAndCubv)
        {
            CuBV one("011101010101");
            CuBV two("101010110100");
            CuBV res = one & two;
            Assert::IsTrue(L"001000010100" == res.BitChar());
        }

        TEST_METHOD(TestMethod_CubvXorCubv)
        {
            CuBV one("011101010101");
            CuBV two("101010110100");
            CuBV res = one ^ two;
            Assert::IsTrue(L"110111100001" == res.BitChar());
        }

        TEST_METHOD(TestMethod_CubvSubCubv)
        {
            CuBV one("01100110");
            CuBV two("01010001");
            CuBV res = one - two;
            CuBV expected("00100110");
            Assert::IsTrue(TRUE == (expected == res));
        }

        TEST_METHOD(TestMethod_OperatorInvert)
        {
            CuBV cubv      ("011101010101");
            CuBV res = ~cubv;
            Assert::IsTrue(L"100010101010" == res.BitChar());
        }

        TEST_METHOD(TestMethod_OperatorLeftShift)
        {
            CuBV cubv      ("011101010101");
            CuBV res = cubv << 3;
            Assert::IsTrue(L"101010101000" == res.BitChar());
        }

        TEST_METHOD(TestMethod_OperatorRightShift)
        {
            CuBV cubv      ("011101010101");
            CuBV res = cubv >> 3;
            Assert::IsTrue(L"000011101010" == res.BitChar());
        }

        TEST_METHOD(TestMethod_LoopLeftShift)
        {
            CuBV cubv("011101010101");
            cubv.LoopLeftShift(3);
            Assert::IsTrue(L"101010101011" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_LoopRightShift)
        {
            CuBV cubv("011101010101");
            cubv.LoopRightShift(3);
            Assert::IsTrue(L"101011101010" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_CountBit)
        {
            CuBV cubv("011101010101");
            int bitCount = cubv.CountBit();
            Assert::IsTrue(bitCount == 7);
        }

        TEST_METHOD(TestMethod_LeftOne)
        {
            CuBV cubv("011101010101");
            Assert::IsTrue(1 == cubv.LeftOne());
            Assert::IsTrue(5 == cubv.LeftOne(3));
        }

        TEST_METHOD(TestMethod_LeftOneLong)
        {
            CuBV cubv("011101010101");
            Assert::IsTrue(1 == cubv.LeftOneLong());
            Assert::IsTrue(5 == cubv.LeftOneLong(3));
        }

        TEST_METHOD(TestMethod_RightOne)
        {
            CuBV cubv("011101010101");
            Assert::IsTrue(11 == cubv.RightOne());
            Assert::IsTrue(3 == cubv.RightOne(5));
        }

        TEST_METHOD(TestMethod_ConcatCubv)
        {
            CuBV one("101011010");
            CuBV two("0101101101");
            one.Concat(two);
            Assert::IsTrue(L"1010110100101101101" == one.BitChar());
        }

        TEST_METHOD(TestMethod_ConcatCubvCubv)
        {
            CuBV one("111101010101");
            CuBV two("00110100101010");
            CuBV res(26);
            res.Concat(one, two);
            Assert::IsTrue(L"11110101010100110100101010" == res.BitChar());
        }

        TEST_METHOD(TestMethod_ConcatBit)
        {
            CuBV cubv("011101010101");
            cubv.Concat(TRUE);
            Assert::IsTrue(L"0111010101011" == cubv.BitChar());
            cubv.Concat();
            Assert::IsTrue(L"01110101010110" == cubv.BitChar());
        }

        TEST_METHOD(TestMethod_IsEmpty)
        {
            CuBV cubv("011101010101");
            Assert::IsTrue(FALSE == cubv.IsEmpty());
            CuBV cubv2;
            Assert::IsTrue(TRUE == cubv2.IsEmpty());
        }

        TEST_METHOD(TestMethod_IsZero)
        {
            CuBV cubv("0000000000");
            Assert::IsTrue(TRUE == cubv.IsZero());
            cubv.Concat(TRUE);
            Assert::IsTrue(FALSE == cubv.IsZero());
        }

        TEST_METHOD(TestMethod_IsOne)
        {
            CuBV cubv("1111111111");
            Assert::IsTrue(TRUE == cubv.IsOne());
            cubv.Concat(FALSE);
            Assert::IsTrue(FALSE == cubv.IsOne());
        }

        TEST_METHOD(TestMethod_OperatorCubvEqualCubv)
        {
            CuBV one("10101010110101");
            CuBV two("10101010110101");
            Assert::IsTrue(TRUE == (one == two));
            one.SetBitAt(1, TRUE);
            Assert::IsTrue(FALSE == (one == two));
        }

        TEST_METHOD(TestMethod_OperatorCubvNotEqualCubv)
        {
            CuBV one("10101010110101");
            CuBV two("11101010110101");
            Assert::IsTrue(TRUE == (one != two));
            two.SetBitAt(1, FALSE);
            Assert::IsTrue(FALSE == (one != two));
        }

        TEST_METHOD(TestMethod_OperatorCubvGreaterCubv)
        {
            CuBV one("10101010110101");
            CuBV two("10101010110100");
            Assert::IsTrue(TRUE == (one > two));
            two.SetBitAt(13, TRUE);
            Assert::IsTrue(FALSE == (one > two));
            one.SetBitAt(13, FALSE);
            Assert::IsTrue(FALSE == (one > two));
        }

        TEST_METHOD(TestMethod_OperatorCubvLessCubv)
        {
            CuBV one("10101010110100");
            CuBV two("10101010110101");
            Assert::IsTrue(TRUE == (one < two));
            two.SetBitAt(13, FALSE);
            Assert::IsTrue(FALSE == (one < two));
            one.SetBitAt(13, TRUE);
            Assert::IsTrue(FALSE == (one < two));
        }

        TEST_METHOD(TestMethod_OperatorCubvGreaterOrEqualCubv)
        {
            CuBV one("10101010110101");
            CuBV two("10101010110100");
            Assert::IsTrue(TRUE == (one >= two));
            two.SetBitAt(13, TRUE);
            Assert::IsTrue(TRUE == (one >= two));
            one.SetBitAt(13, FALSE);
            Assert::IsTrue(FALSE == (one >= two));
        }

        TEST_METHOD(TestMethod_OperatorCubvLessOrEqualCubv)
        {
            CuBV one("10101010110100");
            CuBV two("10101010110101");
            Assert::IsTrue(TRUE == (one <= two));
            one.SetBitAt(13, TRUE);
            Assert::IsTrue(TRUE == (one <= two));
            two.SetBitAt(13, FALSE);
            Assert::IsTrue(FALSE == (one <= two));
        }

        TEST_METHOD(TestMethod_Empty)
        {
            CuBV cubv("10101010110101");
            Assert::IsTrue(FALSE == cubv.IsEmpty());
            cubv.Empty();
            Assert::IsTrue(TRUE == cubv.IsEmpty());
        }

        TEST_METHOD(TestMethod_ReleaseBuffer)
        {
            CuBV cubv("10101010110101");
            cubv.ReleaseBuffer(5);
            Assert::IsTrue(5 == cubv.GetBitLength());
        }

        TEST_METHOD(TestMethod_AssignDiz)
        {
            CuBV cubv(64, TRUE);
            ptrdiff_t arr1[] = {0x3b67a92d4e1ce83b};
            ptrdiff_t arr2[] = {0x4bc290d7e1c5b83d};
            cubv.AssignDiz(64, arr1, arr2);
            ptrdiff_t expectedArr[] = {0x7BE7B9FFEFDDF83F};
            CuBV expected(expectedArr, 64);
            Assert::IsTrue(TRUE == (expected == cubv));
        }

        TEST_METHOD(TestMethod_AssignCon)
        {
            CuBV cubv(64, TRUE);
            ptrdiff_t arr1[] = {0x3b67a92d4e1ce83b};
            ptrdiff_t arr2[] = {0x4bc290d7e1c5b83d};
            cubv.AssignCon(64, arr1, arr2);
            ptrdiff_t expectedArr[] = {0xB4280054004A839};
            CuBV expected(expectedArr, 64);
            Assert::IsTrue(TRUE == (expected == cubv));
        }

        TEST_METHOD(TestMethod_AssignXor)
        {
            CuBV cubv(64, TRUE);
            ptrdiff_t arr1[] = {0x3b67a92d4e1ce83b};
            ptrdiff_t arr2[] = {0x4bc290d7e1c5b83d};
            cubv.AssignXor(64, arr1, arr2);
            ptrdiff_t expectedArr[] = {0x70A539FAAFD95006};
            CuBV expected(expectedArr, 64);
            Assert::IsTrue(TRUE == (expected == cubv));
        }

        TEST_METHOD(TestMethod_AssignDif)
        {
            CuBV cubv(64, TRUE);
            ptrdiff_t arr1[] = {0x3b67a92d4e1ce83b};
            ptrdiff_t arr2[] = {0x4bc290d7e1c5b83d};
            cubv.AssignDif(64, arr1, arr2);
            ptrdiff_t expectedArr[] = {0x302529280E184002};
            CuBV expected(expectedArr, 64);
            Assert::IsTrue(TRUE == (expected == cubv));
        }
	};

	TEST_CLASS(UnitTestCubm)
	{
	public:

		TEST_METHOD(TestMethod_SetSize)
		{
			CuBM testobj(4,4,false);
			testobj.SetSize(5,5,-1,0);
			Assert::IsTrue(testobj.GetLongLength() == LEN_LONG(5) && testobj.GetCountC() == 5  && testobj.GetCountR() == 5);
		}

		TEST_METHOD(TestMethod_FreeExtra)
		{
			CuBM testobj(4,4,false);
			testobj.SetSize(5,5,5,1);
			testobj.FreeExtra();
			Assert::IsTrue(testobj.GetLongLength() == testobj.GetAllocLength());
		}
		
		TEST_METHOD(TestMethod_GetRowBv)
		{
			CuBM testobj(4,4,false);
			CuBV bv = testobj.GetRowBv(3);
			Assert::IsTrue(bv == "0000");
			ptrdiff_t mask [4];
			mask[0] = 0;
			mask[1] = 1;
			mask[2] = 0;
			mask[3] = 1;
			bv = testobj.GetRowBv(3, mask);
			Assert::IsTrue(bv == "0000");
		}
		
		TEST_METHOD(TestMethod_GetColumnBv)
		{
			CuBM testobj(4,4,false);
			CuBV bv = testobj.GetColumnBv(3);
			Assert::IsTrue(bv == "0000");
			ptrdiff_t mask [4];
			mask[0] = 0;
			mask[1] = 1;
			mask[2] = 0;
			mask[3] = 1;
			bv = testobj.GetColumnBv(3, mask);
			Assert::IsTrue(bv == "0000");
		}
		
		TEST_METHOD(TestMethod_ExtractRows)
		{
			CuBM testobj(4,4,true);
			CuBM testobj2 = testobj.ExtractRows(0,3);
			for(int i =0; i<3; i++)
			{
				Assert::IsTrue(testobj.GetRowBv(i) == testobj2.GetRowBv(i));
			}
		}
		TEST_METHOD(TestMethod_ExtractColumns)
		{
			CuBM testobj(4,4,true);
			CuBM testobj2 = testobj.ExtractColumns(0,3);
			for(int i =0; i<3; i++)
			{
				Assert::IsTrue(testobj.GetColumnBv(i) == testobj2.GetColumnBv(i));
			}
		}
		TEST_METHOD(TestMethod_Extract)
		{
			CuBM testobj(4,4,true);
			CuBM testobj2 = testobj.Extract(0,0,2,2);
			for(int i =0; i<2; i++)
			{
				for(int j =0; j<2; j++)
				{
					Assert::IsTrue(1 == testobj2.GetBitAt(i,j));
				}
				
			}
		}
		TEST_METHOD(TestMethod_OperatorAdvBitOr)
		{
			CuBM testobj3(4,4,true);
			CuBM testobj(4,4,true);
			CuBM testobj2(4,4,false);
			testobj |=testobj2;
			Assert::IsTrue(testobj == testobj3);
		}
	};
}
