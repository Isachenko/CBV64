#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Common/BaseBool.h"
#include <string>
#include <fstream>


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
			Assert::IsFalse(a.IsEmpty() == TRUE);
			a.Empty();
			Assert::IsTrue(a.IsEmpty() == TRUE);

		}

		TEST_METHOD(TestMethod_GetBuffer){
			CBV a = CBV("101010");
			BYTE *z = a.GetBuffer(6);
			Assert::IsFalse(a.IsEmpty() == TRUE);
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
			v1<<=nshift;
			Assert::IsTrue(v1=="01010000");
		}
		TEST_METHOD(TestMethod_RightShiftInPlace)
		{
			CBV v1 = CBV("01110101");
			size_t nshift = 4;
			v1>>=nshift;
			Assert::IsTrue(v1=="00000111");
		}
		TEST_METHOD(TestMethod_DizInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1|=v2;
			Assert::IsTrue(v1=="11111100");
		}
		TEST_METHOD(TestMethod_ConInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1&=v2;
			Assert::IsTrue(v1=="11000000");
		}
		TEST_METHOD(TestMethod_Add2InPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1^=v2;
			Assert::IsTrue(v1=="00111100");
		}
		TEST_METHOD(TestMethod_ConNotInPlace)
		{
			CBV v1 = CBV("11001100");
			CBV v2 = CBV("11110000");
			v1-=v2;
			Assert::IsTrue(v1=="00001100");
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
        
        TEST_METHOD(TestMethod_GetByteAt)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(0x4e == cbv.GetByteAt(0));
            Assert::IsTrue(0x9c == cbv.GetByteAt(1));
        }

        TEST_METHOD(TestMethod_OperatorSquareBrackets)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(0x9c == cbv[1]);
            Assert::IsTrue(0x80 == cbv[2]);
        }

        TEST_METHOD(TestMethod_GetBitAt)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(TRUE == cbv.GetBitAt(4));
            Assert::IsTrue(FALSE == cbv.GetBitAt(10));
        }

        TEST_METHOD(TestMethod_BitChar)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(L"010011101001110010" == cbv.BitChar());
        }

        TEST_METHOD(TestMethod_Extract)
        {
            CBV src("010011101001110010");
            CBV res("01110100111");
            Assert::IsTrue(res == src.Extract(3, 11));
        }

        TEST_METHOD(TestMethod_GetBitLength)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(18 == cbv.GetBitLength());
        }

        TEST_METHOD(TestMethod_GetByteLength)
        {
            CBV cbv("010011101001110010");
            Assert::IsTrue(3 == cbv.GetByteLength());
        }

		///////////////////////////////////////////////////////////
		TEST_METHOD(TestMethod_GetRandN) {
			int i, j;
			size_t n, average;
			CString str;

			int sum[64];
			for (i = 0; i < 64; i++) 
				sum[i] = 0;

			Logger::WriteMessage("--------------- The 1st test ----------------------\n");			
			SetRgrain(1);
			for (j = 0; j < 1000; j++) {
				n =  GetRandN();
				for (i = 0; i < 64; i++) {
					if ((n & OB4[i]) != 0)
						sum[i]++;
				}
			}
			Logger::WriteMessage("sum[i]: ");
			for (i = 0; i < 64; i++) {
				str.Format("%d", sum[i]);
				Logger::WriteMessage(str + "; ");
			}

			average = 0;
			for (i = 0; i < 64; i++)
			average += sum[i];
			average /= 64; 
			str.Format("%d", average);
			Logger::WriteMessage("\nAverage = " + str);


			Logger::WriteMessage("\n--------------- The 2nd test ----------------------\n");

			for (i = 0; i < 64; i++) 
				sum[i] = 0;

			SetRgrain(11);
			for (j = 0; j < 1000; j++) {
				n =  GetRandN();
				for (i = 0; i < 64; i++) {
					if ((n & OB4[i]) != 0)
						sum[i]++;
				}
			}
			Logger::WriteMessage("sum[i]: ");
			for (i = 0; i < 64; i++) {
				str.Format("%d", sum[i]);
				Logger::WriteMessage(str + "; ");
			} 

			average = 0;
			for (i = 0; i < 64; i++)
			average += sum[i];
			average /= 64; 
			str.Format("%d", average);
			Logger::WriteMessage("\nAverage = " + str);

			Logger::WriteMessage("\n--------------- The 3rd test ----------------------\n");	

			for (i = 0; i < 64; i++) 
				sum[i] = 0;

			SetRgrain(30);
			for (j = 0; j < 1000; j++) {
				n =  GetRandN();
				for (i = 0; i < 64; i++) {
					if ((n & OB4[i]) != 0)
						sum[i]++;
				}
			}
			Logger::WriteMessage("sum[i]: ");
			for (i = 0; i < 64; i++) {
				str.Format("%d", sum[i]);
				Logger::WriteMessage(str + "; ");

			average = 0;
			for (i = 0; i < 64; i++)
			average += sum[i];
			average /= 64; 
			str.Format("%d", average);
			Logger::WriteMessage("\nAverage = " + str);
			} 
		} // GetRandN

		TEST_METHOD(TestMethod_GenRbvN_1000) {
			int n = 100;
			CBV v = CBV();
			int i, j, kol_edin;
			CString str;
					
			for (j = 0; j < 1000; j++) {								
				v = v.GenRbvN(n);

				str.Format("%d", j);
				Logger::WriteMessage("\n----- Vector number " + str);
				Logger::WriteMessage("\nPlaces of 1: ");

				kol_edin = 0;
				for (i = 0; i < n; i++) {
					if (v.GetBitAt(i) == 1) {
						str.Format("%d", i);
						Logger::WriteMessage(str + "; ");
						kol_edin++;
					}
				}
				str.Format("%d", kol_edin);
				Logger::WriteMessage("\nNumber of 1 = " + str + "\n");

			}
			
		} // GenRbvN_1000

		TEST_METHOD(TestMethod_GetRbvN_Series){
			int kol_series = 5; 
			const int vect_series = 200;
			const int n = 100;
			int h_Rgrain = 10;
			int i, j, k, ammount;
			CBV v = CBV();

			ofstream f1("sum.csv");
			ofstream f2("sumS.csv");
			CString str;

			int sum[n];
			int kol_ed[vect_series];

			SetRgrain(1);
			for (i = 0; i < kol_series; i++){
				SetRgrain(GetRgrain() + h_Rgrain);
				
				for (k = 0; k < n ; k++) {
					sum[k] = 0;
				}

				for (j = 0; j < vect_series; j++){
					kol_ed[j] = 0;
					v = v.GenRbvN(n);
					ammount = 0;
					for (k = 0; k < n; k++){
						if (v.GetBitAt(k) == 1){
							sum[k]++;
							ammount++;
						}
					}
					kol_ed[ammount]++; 
				}
				// положить sum в файл
				str.Format("%d", i);
				f1 << "Series" + str << ';';
				for (k = 0; k < n ; k++) {
					f1 << sum[k] << ';';
				}
				f1 << endl;

				// положить kol_ed в файл
				str.Format("%d", i);
				f2 << "Series" + str << ';';
				for (k = 0; k < n ; k++) {
					f2 << kol_ed[k] << ';';
				}
				f2 << endl;
			} // for (i = 0; i < kol_series
			f1.close();
			f2.close();
		} // TEST_METHOD(TestMethod_GetRbvN_Series)
		
			TEST_METHOD(TestMethod_GetRbvN_piece){
			int n = 64;
			int kol_rand = 20;
			int r;
			bool sovp;
			int nomer_sovp;
			int summa;
			
			ofstream f("sovp_piece.csv");
			CString str;

			CBV v0 = CBV();
			CBV v = CBV();
			v0 = v0.GenRbvN(n);

			for (int i = 4; i <= 16; i++){ // длина куска
				str.Format("%d", i);
				f << "Length of piece = " + str << ';';
				for (int j = 0; j < kol_rand; j++){ // количество рандомов
					do {
						r = rand() % 64; //GetRandN();
					} while ((r + i) >= n);

					summa = 0;
					for (int l = 0; l < 10; l++){
						nomer_sovp = 0;
						do {
							v = v.GenRbvN(n);
							nomer_sovp++;
							sovp = true;
							for (size_t k = r; k < (r+i); k++){ // проверка на совпадение кусков
								if (v.GetBitAt(k) != v0.GetBitAt(k)){
									sovp = false;
									break;
								}
							}
						} while (!sovp);

						// здесь знаю число nomer_sovp, за которое нашлось совпадение
						summa += nomer_sovp ;		

					} // for (int l = 0; l < 10

					f << summa/10 << ';'; // вывод в файл среднего числа 

				} // for (int j = 0; j < kol_rand
				f << endl;

			} // for (int i = 4; i <= 16

		} // TEST_METHOD(TestMethod_GetRbvN_piece)
	};
}