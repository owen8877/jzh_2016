#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace std;

const double regionEducation[3][3] = { { 0.0501287413332061, 0.5732839571870683, 0.3765873014797256 },
								{ 0.1107661743670357, 0.6910291586157585, 0.1982046670172058 },
								{ 0.2478316669695342, 0.6930682275402885, 0.0591001054901774 } };
const double sexRatio = 1.049;

int pop[1201][3][3]; //�˿ڳ�ʼ����
int newBirthData[3][3];
double averageBirthRateInitial[101]; //ÿ��ƽ��������
double averageDeathRateInitial[101][3]; //ÿ��ƽ��������
double averageBirthRate[1201]; //�¾�������
double averageDeathRate[1201][3]; //��ƽ��������
double lambdaRegionEducation[3][3]; //lambda_k_l�ľ���

namespace enumExt {
	// ��ö��Ҳ��������Щ������
	template<typename T>
	inline T operator |=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator |(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) | static_cast<int>(b));
	}
	template<typename T>
	inline T operator &=(T &a, const T &b)
	{
		return a = static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator &(const T &a, const T &b)
	{
		return static_cast<T>(static_cast<int>(a) & static_cast<int>(b));
	}
	template<typename T>
	inline T operator ++(T &a)
	{
		return a = static_cast<T>(static_cast<int>(a) + 1);
	}
	template<typename T>
	inline T operator ~(const T &a)
	{
		return static_cast<T>(~static_cast<int>(a));
	}
}

namespace enumType {
	enum popWork {
		administrator = 0, //���һ��ء���Ⱥ��֯����ҵ����ҵ��λ������
		clerk = 1, //רҵ������Ա��������Ա���й���Ա
		buservice = 2, //��ҵ������ҵ��Ա
		farmer = 3, //ũ���֡������桢ˮ��ҵ������Ա
		labour = 4, //�����������豸������Ա���й���Ա
	};
	enum popRegion {
		city = 0, //��
		town = 1, //��
		country = 2, //��
	};
	enum popEducation {
		primary = 0, //Сѧ������
		middle = 1, //���м�����
		senior = 2, //��ѧ������
	};
}

void PopInput() {
	using namespace enumExt;
	using namespace enumType;

	int popInitial[1201][3];

	ifstream inputFile;
	inputFile.open("input.csv");
	for (popRegion region = city; region <= country; ++region) {
		for (int age = 0; age <= 100; age++) inputFile >> popInitial[age * 12][region];
	}
	inputFile.close();
	for (int age = 0; age <= 100; age++)
		for (popRegion region = city; region <= country; ++region) 
			for (popEducation education = primary; education <= senior; ++education) pop[age * 12][region][education] = int(popInitial[age * 12][region] * regionEducation[region][education]);
}

void BirthRateInput() {
	using namespace enumExt;
	using namespace enumType;

	ifstream inputFile;
	inputFile.open("inputbirthrate.csv");
	for (int age = 0; age <= 100; age++) {
		int birthBaby, birthWomen, totalMen = 0;
		char dot;
		if (age < 15 || age > 49) {
			averageBirthRateInitial[age] = 0;
			continue;
		}
		inputFile >> birthBaby >> dot >> birthWomen;
		for (popRegion region = city; region <= country; ++region)
			for (popEducation education = primary; education <= senior; ++education) {
				totalMen += pop[age * 12][region][education];
			}
		averageBirthRateInitial[age] = birthBaby * (1 + sexRatio) / (totalMen * 1.0) ;
	}
	for (int age = 0; age <= 99; age++)
		for (int month = 1; month <= 12; month++) averageBirthRate[age * 12 + month - 1] = 1 - pow(1 - averageBirthRateInitial[age], 1 / 12.0);
	averageBirthRate[1200] = 0;
	inputFile.close();
	/*
	cout << setprecision(10);
	for (int age = 0; age <= 100; age++) cout << averageBirthRateInitial[age] << endl;
	*/
} 

void DeathRateInput() {
	ifstream inputFile;
	inputFile.open("inputbirthrate.csv");
}

void NewBirthSeparate() {
	using namespace enumExt;
	using namespace enumType;
	
	ifstream inputFile;
	inputFile.open("inputbirthdata.csv");
	for (popRegion region = city; region <= country; ++region) {
		char dot;
		for (popEducation education = primary; education <= senior; ++education) {
			inputFile >> newBirthData[region][education];
			if (education < senior) inputFile >> dot;
		}
	}
	inputFile.close();
	/*
	for (popRegion region = city; region <= country; ++region) {
		for (popEducation education = primary; education <= senior; ++education) {
			cout << newBirthData[region][education] << ' ';
		}
		cout << endl;
	}
	system("pause");
	*/
}

void CreateLambdaRegionEducation() {
	using namespace enumExt;
	using namespace enumType;

	for (popRegion region = city; region <= country; ++region)
		for (popEducation education = primary; education <= senior; ++education) {
			double averageData = 0;
			for (int age = 0; age <= 100; age++) averageData += pop[age * 12][region][education] * averageBirthRateInitial[age];
			lambdaRegionEducation[region][education] = newBirthData[region][education]  * (1 + sexRatio) / averageData;
		}
	cout << setprecision(10);
	for (popRegion region = city; region <= country; ++region) {
		for (popEducation education = primary; education <= senior; ++education) cout << lambdaRegionEducation[region][education] << ' ';
		cout << endl;
	}
	system("pause");
}

int main() {
	PopInput(); //�˿����ݳ�ʼ��
	BirthRateInput(); //ƽ�������ʳ�ʼ��
	DeathRateInput(); //ƽ�������ʳ�ʼ��
	NewBirthSeparate(); //�����r_k������e_l�ܽ���������
	CreateLambdaRegionEducation(); //���lambda_k_l�ľ���
	return 0;
}