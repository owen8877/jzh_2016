#include <iostream>
#include <iomanip>
#include <cmath>
#include <fstream>

using namespace std;

const char* outputFileName = "output_file_0.txt";
const double sexRatio = 1.0347; // 1.0365
const double transformRatio = 1332810869 / 127339585.0; // 19612368	/ 1849475.0	
const double doubleinfantRatio = 372295 / 1190060.0;

double lambdaPolicy[3] = { 1 , 1 , 1 };
double lambdaMove[2] = { 0, 0 }; // VERIFIED! 0.0019669
double lambdaSmarter[2] = { 0, 0 }; // VERIFIED! 0.000578613
double etaAge[100][3];

int totalWomen;
int pop[1201][3][3][3]; //人口初始数据
int womenPop[100][3][3];
double dynamicPop[2401][1201][3][3][3];
int newBirthData[3][3]; 
//int kidsum = 0, sixteensum = 0;
double regionEducation[3][3];
double averageWomenType[1201][3];
double averageBirthRateInitial[101]; //每年平均生育率
double averageDeathRateInitial[101][3]; //每年平均死亡率
double averageBirthRate[1201]; //月均生育率
double averageDeathRate[1201][3]; //月平均死亡率
double lambdaRegionEducation[3][3] = { { 0.8015288534, 0.7959010804, 0.6300911831 },
									   { 1.013375018, 1.103942855, 0.5877072244 },
									   { 1.062756905, 1.342527489, 0.4451663224 } }; //lambda_k_l的矩阵
// double regionValue[3], educationValue[3];

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

void RegionEducationRateInput() {
	using namespace enumExt;
	using namespace enumType;

	ifstream inputFile;
	char dot;
	inputFile.open("inputregioneducationrate.csv");
	for (popRegion region = city; region <= country; ++region)
		for (popEducation education = primary; education <= senior; ++education) {
			inputFile >> regionEducation[region][education];
			if (education < senior) inputFile >> dot;
		}
	inputFile.close();
}

void PopInput() {
	using namespace enumExt;
	using namespace enumType;

	int popInitial[1201][3];

	ifstream inputFile;
	inputFile.open("input.csv");
	for (popRegion region = city; region <= country; ++region)
		for (int age = 0; age <= 100; age++) {
			inputFile >> popInitial[age * 12][region];
		}
	inputFile.close();
	for (int age = 0; age <= 99; age++)
		for (popRegion region = city; region <= country; ++region)
			for (popEducation education = primary; education <= senior; ++education)
				for (int kid = 0; kid <= 2; kid++)
					for (int month = 1; month <= 12; month++) pop[age * 12 + month][region][education][kid] = int(popInitial[age * 12][region] * regionEducation[region][education] * averageWomenType[age * 12][kid] / 12.0);
	for (popRegion region = city; region <= country; ++region)
		for (popEducation education = primary; education <= senior; ++education)
			for (int kid = 0; kid <= 2; kid++) pop[1200][region][education][kid] = int(popInitial[1200][region] * regionEducation[region][education] * averageWomenType[1200][kid]);
}


void WomenTypeInput() {
	using namespace enumExt;
	using namespace enumType;

	int women[3];
	double dinkRatio;
	char dot;

	ifstream inputFile;
	inputFile.open("inputwomentype.csv");
	inputFile >> totalWomen >> dot >> dinkRatio >> dot;
	totalWomen = totalWomen / (1 - dinkRatio);
	women[0] = totalWomen; 
	women[1] = women[2] = 0;
	for (int age = 0; age <= 1200; age++) {
		averageWomenType[age][0] = 1;
		averageWomenType[age][1] = averageWomenType[age][2] = 0;
	}
	for (int age = 15; age <= 49; age++) {
		int t1, t2, t3;
		inputFile >> t1 >> dot >> t2 >> dot >> t3;
		women[0] -= t1;
		women[1] = women[1] + t1 - t2;
		women[2] = women[2] + t2;
		averageWomenType[age * 12][0] = women[0] / (totalWomen * 1.0);
		averageWomenType[age * 12][1] = women[1] / (totalWomen * 1.0);
		averageWomenType[age * 12][2] = women[2] / (totalWomen * 1.0);
	}
	for (int age = 0; age <= 1200; age++)
		for (int i = 0; i <= 2; i++) averageWomenType[age][i] = averageWomenType[(age / 12) * 12][i];
	inputFile.close();
}

void WomenPopInput() {
	using namespace enumExt;
	using namespace enumType;

	ifstream inputFile;
	inputFile.open("inputwomenpop.csv");
	for (popRegion region = city; region <= country; ++region)
		for (int age = 0; age <= 100; age++) {
			if (age < 15 || age > 49) {
				for (popEducation education = primary; education <= senior; ++education) {
					womenPop[age][region][education] = 0;
				}
				continue;
			}
			char dot;
			for (popEducation education = primary; education <= senior; ++education) {
				inputFile >> womenPop[age][region][education];
				if (education < senior) inputFile >> dot;
			}
	}
}

/*
void BirthRateAdjust() {
	using namespace enumExt;
	using namespace enumType;

	for (int age = 0; age <= 1200; age++) {
		if (age < 20 * 12) {
			averageBirthRate[age] *= (1.69 / 1.25);
			continue;
		}
		if (age < 25 * 12) {
			averageBirthRate[age] *= (1.68 / 1.35);
			continue;
		}
		if (age < 30 * 12) {
			averageBirthRate[age] *= (1.73 / 1.49);
			continue;
		}
		if (age < 35 * 12) {
			averageBirthRate[age] *= (1.84 / 1.62);
			continue;
		}
		if (age < 40 * 12) {
			averageBirthRate[age] *= (1.99 / 1.77);
			continue;
		}
		if (age < 45 * 12) {
			averageBirthRate[age] *= (2.08 / 1.82);
			continue;
		}
		if (age < 50 * 12) {
			averageBirthRate[age] *= (2.19 / 1.94);
			continue;
		}
	}
}
*/

void BirthRateInput() {
	using namespace enumExt;
	using namespace enumType;

	ifstream inputFile;
	inputFile.open("inputbirthrate.csv");
	for (int age = 0; age <= 100; age++) {
		if (age < 15 || age > 49) {
			averageBirthRateInitial[age] = 0;
			continue;
		}
		inputFile >> averageBirthRateInitial[age];
	}

	for (int age = 0; age <= 99; age++)
		for (int month = 1; month <= 12; month++) averageBirthRate[age * 12 + month - 1] = 1 - pow(1 - averageBirthRateInitial[age], 1 / 12.0);
	averageBirthRate[1200] = 0;
	inputFile.close();

	/*
	cout << setprecision(10);
	for (int age = 0; age <= 100; age++) cout << averageBirthRateInitial[age] << endl;
	system("pause"); */


}

void DeathRateInput() {
	using namespace enumExt;
	using namespace enumType;

	ifstream inputFile;
	inputFile.open("inputdeathrate.csv");
	for (popRegion region = city; region <= country; ++region)
		for (int age = 0; age <= 100; age++) {
			inputFile >> averageDeathRateInitial[age][region];
			averageDeathRateInitial[age][region] /= 1000.0;
			for (int month = 1; month <= 12; month++) averageDeathRate[age * 12 + month - 1][region] = 1 - pow(1 - averageDeathRateInitial[age][region], 1 / 12.0);
		}
	inputFile.close();
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


void GenerateValue() {
	double Evalue[10000][3];
	double Rvalue[10000][3];
	for (int i = 0; i <= 2; i++) Evalue[0][i] = Rvalue[0][i] = 1;
	for (int i = 0; i < 9999; i++) {
		for (int j = 0; j <= 2; j++) {
			double tmp1 = 0, tmp2 = 0;
			for (int k = 0; k <= 2; k++) tmp1 += Rvalue[i][k] * lambdaRegionEducation[k][j];
			for (int k = 0; k <= 2; k++) tmp2 += Rvalue[i][k] * Rvalue[i][k];
			Evalue[i + 1][j] = tmp1 / tmp2;
			tmp1 = tmp2 = 0;
			for (int k = 0; k <= 2; k++) tmp1 += Evalue[i][k] * lambdaRegionEducation[j][k];
			for (int k = 0; k <= 2; k++) tmp2 += Evalue[i][k] * Evalue[i][k];
			Rvalue[i + 1][j] = tmp1 / tmp2;
		}
	}
	cout << setprecision(5);
	for (int i = 0; i <= 2; i++) {
		for (int j = 0; j <= 2; j++) cout << Rvalue[9999][i] * Evalue[9999][j] << ' ';
		cout << endl;
	}
	cout << endl;
	system("pause");
}


void GenerateLambdaRegionEducation() {
	using namespace enumExt;
	using namespace enumType;

	for (popRegion region = city; region <= country; ++region)
		for (popEducation education = primary; education <= senior; ++education) {
			double averageData = 0;
			for (int age = 0; age <= 100; age++) averageData += (pop[age * 12][region][education][0] + pop[age * 12][region][education][1] + pop[age * 12][region][education][2]) * averageBirthRateInitial[age];
			lambdaRegionEducation[region][education] = newBirthData[region][education] * (1 + sexRatio)/ averageData;
		}
	
	/*cout << setprecision(10);
	
	for (popRegion region = city; region <= country; ++region) {
		for (popEducation education = primary; education <= senior; ++education) cout << lambdaRegionEducation[region][education] << ' ';
		cout << endl;
	}
	system("pause"); */
	
}

void InitializeEtaAge() {
	for (int age = 0; age <= 100; age++)
		for (int kid = 0; kid <= 2; kid++) etaAge[age][kid] = 1;
}

void GenerateEtaAge() {
	const double amendament = 1.8 / 2.04;

	for (int age = 0; age <= 100; age++) {
		if (15 <= age && age <= 19) {
			etaAge[age][1] = (1.69 - 1.25) * amendament;
			continue;
		}
		if (20 <= age && age <= 24) {
			etaAge[age][1] = (1.68 - 1.35) * amendament;
			continue;
		}
		if (25 <= age && age <= 29) {
			etaAge[age][1] = (1.73 - 1.49) * amendament;
			continue;
		}
		if (30 <= age && age <= 34) {
			etaAge[age][1] = (1.84 - 1.62) * amendament;
			continue;
		}
		if (35 <= age && age <= 39) {
			etaAge[age][1] = (1.99 - 1.77) * amendament;
			continue;
		}
		if (40 <= age && age <= 44) {
			etaAge[age][1] = (2.08 - 1.82) * amendament;
			continue;
		}
		if (45 <= age && age <= 49) {
			etaAge[age][1] = (2.19 - 1.94) * amendament;
			continue;
		}
	}
	for (int age = 0; age <= 100; age++)
		for (int kid = 0; kid <= 2; kid++) if (etaAge[age][kid] != 1)  etaAge[age][kid] = (doubleinfantRatio + etaAge[age][kid]) / doubleinfantRatio; else etaAge[age][kid] = 1;
}

double BasicTest(int maximumMonth) {
	using namespace enumExt;
	using namespace enumType;

	for (int month = 0; month <= maximumMonth; month++)
		for (int age = 0; age <= 1200; age++)
			for (popRegion region = city; region <= country; ++region)
				for (popEducation education = primary; education <= senior; ++education)
					for (int kid = 0; kid <= 2; kid++) {
						// if (age == 16 * 12 && month == 0) sixteensum += pop[age][region][education][kid];
						dynamicPop[month][age][region][education][kid] = 0;
					}
	for (int age = 0; age <= 1200; age++)
		for (popRegion region = city; region <= country; ++region)
			for (popEducation education = primary; education <= senior; ++education)
				for (int kid = 0; kid <= 2; kid++) dynamicPop[0][age][region][education][kid] = pop[age][region][education][kid];
	for (int curMonth = 0; curMonth <= maximumMonth - 1; curMonth++) {
		//放开二胎
		// if (curMonth == 60) GenerateEtaAge();
		//自然死亡
		for (int age = 0; age <= 1199; age++)
			for (popRegion region = city; region <= country; ++region)
				for (popEducation education = primary; education <= senior; ++education)
					for (int kid = 0; kid <= 2; kid++) {
						dynamicPop[curMonth + 1][age + 1][region][education][kid] += dynamicPop[curMonth][age][region][education][kid] * (1 - averageDeathRate[age][region]) * (1 - averageBirthRate[age] * etaAge[age / 12][kid]);
						if (kid < 2) dynamicPop[curMonth + 1][age + 1][region][education][kid + 1] += dynamicPop[curMonth][age][region][education][kid] * (1 - averageDeathRate[age][region]) * averageBirthRate[age] * etaAge[age / 12][kid];
						else dynamicPop[curMonth + 1][age + 1][region][education][kid] += dynamicPop[curMonth][age][region][education][kid] * (1 - averageDeathRate[age][region]) * averageBirthRate[age] * etaAge[age / 12][kid];
					}
		//产生新一代
		for (popRegion region = city; region <= country; ++region)
			for (popEducation education = primary; education <= senior; ++education) {
				double temp = 0;
				for (int age = 0; age <= 1200; age++)
					for (int kid = 0; kid <= 2; kid++) {
						temp += averageBirthRate[age] * lambdaPolicy[kid] * lambdaRegionEducation[region][education] * dynamicPop[curMonth][age][region][education][kid] * etaAge[age / 12][kid] / (1 + sexRatio);
						// if (age == curMonth + 12 * 16) kidsum += averageBirthRate[age] * lambdaPolicy[kid] * lambdaRegionEducation[region][education] * dynamicPop[curMonth][age][region][education][kid] / (1 + sexRatio);
					}
				dynamicPop[curMonth + 1][0][region][education][0] = int(temp);
			}
		//迁移
		for (int age = 0; age <= 1200; age++)
			for (popEducation education = primary; education <= senior; ++education)
				for (int kid = 0; kid <= 2; kid++) {
					double delta1, delta2;
					delta1 = dynamicPop[curMonth + 1][age][country][education][kid] * lambdaMove[0];
					delta2 = dynamicPop[curMonth + 1][age][town][education][kid] * lambdaMove[1];
					dynamicPop[curMonth + 1][age][country][education][kid] -= delta1;
					dynamicPop[curMonth + 1][age][town][education][kid] -= (delta2 - delta1);
					dynamicPop[curMonth + 1][age][city][education][kid] += delta2;
				}
		//学历变化
		for (int age = 0; age <= 1200; age++)
			for (popRegion region = city; region <= country; ++region)
				for (int kid = 0; kid <= 2; kid++) {
					double delta1, delta2;
					delta1 = dynamicPop[curMonth + 1][age][region][primary][kid] * lambdaSmarter[0];
					delta2 = dynamicPop[curMonth + 1][age][region][middle][kid] * lambdaSmarter[1];
					dynamicPop[curMonth + 1][age][region][primary][kid] -= delta1;
					dynamicPop[curMonth + 1][age][region][middle][kid] -= (delta2 - delta1);
					dynamicPop[curMonth + 1][age][region][senior][kid] += delta2;
				}
	}
	
	ofstream outputFile;
	int num1 = 0, num2 = 0;
	outputFile.open(outputFileName);
	for (int month = 0; month <= maximumMonth; month++) {
		double curPop = 0;
		for (popRegion region = city; region <= country; ++region) {
			for (int age = 0; age <= 1200; age++)
				for (popEducation education = primary; education <= senior; ++education)
					for (int kid = 0; kid <= 2; kid++) {
						curPop += dynamicPop[month][age][region][education][kid];
						// if (region == country) num1 += dynamicPop[month][age][region][education][kid];	// To Generate Move
						//if (education == senior && age >= 24 * 12) num1 += dynamicPop[month][age][region][education][kid];	// To Generate Smarter
					}
		}
		//if (month == 0) num1 = curPop * transformRatio;	//To Generate Policy
		//if (month == 12) num2 = curPop * transformRatio;	//To Generate Policy
		//if (month == 60) num2 = curPop;	// To Generate Move & Smarter
		outputFile << int(curPop * transformRatio) << ' ';
	}
	outputFile.close();
	//return (num2 / (num1 * 1.0) - 1);		//To Generate Policy
	//return (num1 / (num2 * 1.0));		//To Generate Move  & Smarter
	return 0;
}

void GenerateLambdaPolicy() {
	const double standard = 0.0057;
	double inf = 0, sup = 1, nowResult = 0;
	cout << setprecision(10);
	do {
		lambdaPolicy[0] = 3 - 2 * (inf + sup)/2.0;
		lambdaPolicy[1] = lambdaPolicy[2] = (inf + sup)/2.0;
		nowResult = BasicTest(15);
		if (nowResult > standard) inf = (inf + sup) / 2.0; else sup = (inf + sup) / 2.0;
		cout << nowResult << endl;
	} while (fabs(nowResult - standard) > 0.000001);
	cout << inf << endl;
	system("pause");
}

void GenerateLambdaMove() {
	const double standard = 0.4523;
	double inf = 0.0000, sup = 0.000, nowResult;
	cout << setprecision(10); 
	do {
		lambdaMove[0] = lambdaMove[1] = (inf + sup) / 2.0;
		nowResult = BasicTest(60);
		if (nowResult > standard) inf = (inf + sup) / 2.0; else sup = (inf + sup) / 2.0;
		cout << nowResult << endl;
	} while (fabs(nowResult - standard) > 0.000001);
	cout << inf << endl;
	system("pause");
}

void GenerateLambdaSmarter() {
	const double standard = 0.1357 * (120698 / 1047090.0) / (11490522 / 111601296.0);
	double inf = 0.0000, sup = 0.002, nowResult;
	cout << setprecision(10);
	do {
		lambdaSmarter[0] = lambdaSmarter[1] = (inf + sup) / 2.0;
		nowResult = BasicTest(60);
		if (nowResult < standard) inf = (inf + sup) / 2.0; else sup = (inf + sup) / 2.0;
		cout << nowResult << endl;
	} while (fabs(nowResult - standard) > 0.000001);
	cout << inf << endl;
	system("pause");
}

int main() {
	RegionEducationRateInput(); //某一地区受到某一教育的人的比例	
	WomenTypeInput(); //生育n孩的妇女比
	WomenPopInput(); //各地区各教育程度未孕妇女数量
	PopInput(); //人口数据初始化
	BirthRateInput(); //平均出生率初始化
	DeathRateInput(); //平均死亡率初始化
	NewBirthSeparate(); //获得在r_k地区的e_l受教育者人数
    //GenerateLambdaRegionEducation(); //获得lambda_k_l的矩阵
	InitializeEtaAge();
	//GenerateValue();
	//GenerateLambdaPolicy();
	//GenerateLambdaMove();
	//GenerateLambdaSmarter();
	BasicTest(600);
	// cout << kidsum * (1 + sexRatio) / (sixteensum * 1.0) << endl;
	return 0;
}