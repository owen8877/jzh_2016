#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace std;

const double regionEducation[3][3] = { { 0.0501287413332061, 0.5732839571870683, 0.3765873014797256 },
								{ 0.1107661743670357, 0.6910291586157585, 0.1982046670172058 },
								{ 0.2478316669695342, 0.6930682275402885, 0.0591001054901774 } };
const double sexRatio = 1.049;

int pop[1201][3][3]; //人口初始数据
int newBirthData[3][3];
double averageBirthRateInitial[101]; //每年平均生育率
double averageDeathRateInitial[101][3]; //每年平均死亡率
double averageBirthRate[1201]; //月均生育率
double averageDeathRate[1201][3]; //月平均死亡率
double lambdaRegionEducation[3][3]; //lambda_k_l的矩阵

namespace enumExt {
	// 让枚举也可以用这些运算了
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
		administrator = 0, //国家机关、党群组织、企业、事业单位负责人
		clerk = 1, //专业技术人员及办事人员和有关人员
		buservice = 2, //商业、服务业人员
		farmer = 3, //农、林、牧、渔、水利业生产人员
		labour = 4, //生产、运输设备操作人员及有关人员
	};
	enum popRegion {
		city = 0, //城
		town = 1, //镇
		country = 2, //乡
	};
	enum popEducation {
		primary = 0, //小学及以下
		middle = 1, //初中及高中
		senior = 2, //大学及以上
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
	PopInput(); //人口数据初始化
	BirthRateInput(); //平均出生率初始化
	DeathRateInput(); //平均死亡率初始化
	NewBirthSeparate(); //获得在r_k地区的e_l受教育者人数
	CreateLambdaRegionEducation(); //获得lambda_k_l的矩阵
	return 0;
}