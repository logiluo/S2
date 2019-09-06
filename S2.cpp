#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include <unordered_map>
#include <sys/time.h>
#include <unistd.h>
#include <cstdio>
using namespace std;

#define INF unsigned(-1)

const unsigned long long SEC = (unsigned long long) 1000000;
const unsigned long long MIN = (unsigned long long) 60 * SEC;
const unsigned long long HOUR = (unsigned long long) 60 * MIN;
const unsigned long long DAY = (unsigned long long) 24 * HOUR;

string qwerty = "qwertyuiopasdfghjklzxcvbnm"; // QWERTY layout
string qwertyHandArray = "LLLLLRRRRRLLLLLRRRRLLLLRRR"; // first, second and third row in order, corresponding to QWERTY layout

vector<string> word_list;
int LR, LC, RR, RC;
int sizeL, sizeR;

unordered_map<string, double> top_list;
// original coordinates
double coor_org_row[26];
double coor_org_column[26];
// stretched coordinates
double coor_L_row[26];
double coor_L_column[26];
double coor_R_row[26];
double coor_R_column[26];
// calculated distance map
double L_distmap[26][26];
double R_distmap[26][26];


// unordered_map<unsigned, string> handList;
// unordered_map<unsigned, vector<string *>> handAndWord;

inline long long time_stamp() {
    struct timeval query_time;
    gettimeofday(&query_time, nullptr);
    return query_time.tv_sec * 1000000 + query_time.tv_usec;
}

int mapLetter(char c) {
	switch(c) {
	case 'q':
		return 0;
	case 'w':
		return 1;
	case 'e':
		return 2;
	case 'r':
		return 3;
	case 't':
		return 4;
	case 'y':
		return 5;
	case 'u':
		return 6;
	case 'i':
		return 7;
	case 'o':
		return 8;
	case 'p':
		return 9;
	case 'a':
		return 10;
	case 's':
		return 11;
	case 'd':
		return 12;
	case 'f':
		return 13;
	case 'g':
		return 14;
	case 'h':
		return 15;
	case 'j':
		return 16;
	case 'k':
		return 17;
	case 'l':
		return 18;
	case 'z':
		return 19;
	case 'x':
		return 20;
	case 'c':
		return 21;
	case 'v':
		return 22;
	case 'b':
		return 23;
	case 'n':
		return 24;
	case 'm':
		return 25;
	}
}

// 读取文件中的单词，并存储在word_list中
void readWords(string filename) {
	ifstream fin(filename);
	string line;
	// new 
	string numS;
	int count = 0;
	while (getline(fin, line)) {
		if (count >= 3500) {
			break;
		}
		int i;
		for (i = 0; i < line.length(); i++) {	
			if (line[i] == ',') {
				break;
			}
		}
		numS = line.substr(i+1);
		double num = stod(numS) *100 ;
		word_list.push_back(line.substr(0, i));
		top_list[line.substr(0, i)] = num;
		count++;
	}
	fin.close();
}

// calculate LR, LC, RR, RC
void maxCR() {
	LR = 0;
	RR = 0;

	int maxL1 = -1, minL1 = 30;
	int maxL2 = -1, minL2 = 30;
	int maxL3 = -1, minL3 = 30;

	int maxR1 = -1, minR1 = 30;
	int maxR2 = -1, minR2 = 30;
	int maxR3 = -1, minR3 = 30;

	// first row
	for (int i = 0; i < 10; i++) {
		if (qwertyHandArray[i] == 'L') {
			LR = 1;
			maxL1 = maxL1 < i ? i : maxL1;
			minL1 = minL1 < i ? minL1 : i;
		} else {
			RR = 1;
			maxR1 = maxR1 < i ? i : maxR1;
			minR1 = minR1 < i ? minR1 : i;
		}
	}
	// second row
	for (int i = 10; i < 19; i++) {
		if (qwertyHandArray[i] == 'L') {
			LR = 2;
			maxL2 = maxL2 < i ? i : maxL2;
			minL2 = minL2 < i ? minL2 : i;
		} else {
			RR = 2;
			maxR2 = maxR2 < i ? i : maxR2;
			minR2 = minR2 < i ? minR2 : i;
		}
	}
	// third row
	for (int i = 19; i < 26; i++) {
		if (qwertyHandArray[i] == 'L') {
			LR = 3;
			maxL3 = maxL3 < i ? i : maxL3;
			minL3 = minL3 < i ? minL3 : i;
		} else {
			RR = 3;
			maxR3 = maxR3 < i ? i : maxR3;
			minR3 = minR3 < i ? minR3 : i;
		}
	}

	int L3 = maxL3 - minL3;
	int L2 = maxL2 - minL2;
	int L1 = maxL1 - minL1;
	int tmp = L3 > L2 ? L3 : L2;
	LC = tmp > L1 ? tmp : L1; // find max of L1, L2, L3
	LC++;

	int R3 = maxR3 - minR3;
	int R2 = maxR2 - minR2;
	int R1 = maxR1 - minR1;
	tmp = R3 > R2 ? R3 : R2;
	RC = tmp > R1 ? tmp : R1; // find max of R1, R2, R3
	RC++;

	sizeL = (14.2/LR) * (22.8/LC);
	sizeR = (14.2/RR) * (22.8/RC);
}

// 提前计算正常QWERTY键位坐标
void calOriginalCoor() {
	// first row
	for (int i = 0; i < 10; i++) {
		int char_idx = qwerty[i] - 'a';
		coor_org_row[char_idx] = 0;
		coor_org_column[char_idx] = i;
	}
	// second row
	for (int i = 10; i < 19; i++) {
		int char_idx = qwerty[i] - 'a';
		coor_org_row[char_idx] = 1;
		coor_org_column[char_idx] = i - 10 + 0.5;
	}
	// third row
	for (int i = 19; i < 26; i++) {
		int char_idx = qwerty[i] - 'a';
		coor_org_row[char_idx] = 2;
		coor_org_column[char_idx] = i - 19 + 1.5;
	}
}

// 提前计算拉伸后的键位坐标
void calRealCoor() {
	// stretch the keyboard
	for (int i = 0; i < 26; i++) {
		int char_idx = qwerty[i] - 'a';
		if (qwertyHandArray[i] == 'L') {
			coor_L_row[char_idx] = coor_org_row[char_idx] * 14.2 / LR;
			coor_L_column[char_idx] = coor_org_column[char_idx] * 22.8 / LC;
		} else {
			coor_R_row[char_idx] = coor_org_row[char_idx] * 14.2 / RR;
			coor_R_column[char_idx] = coor_org_column[char_idx] * 22.8 / RC;
		}
	}
}

// 两个键之间的距离
double euc_dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

// 提前计算两两键之间的距离
void calDist() {
	for (int i = 0; i < 26; i++) {
		for (int j = 0; j < 26; j++) {
			if (i < j) {
				L_distmap[i][j] = euc_dist(coor_L_row[i], coor_L_column[i], coor_L_row[j], coor_L_column[j]);
				R_distmap[i][j] = euc_dist(coor_R_row[i], coor_R_column[i], coor_R_row[j], coor_R_column[j]);
			} else if (j < i) {
				L_distmap[i][j] = L_distmap[j][i];
				R_distmap[i][j] = R_distmap[j][i];
			} else {
				L_distmap[i][j] = 0;
				R_distmap[i][j] = 0;
			}
		}
	}
}

// 算layouttime
double getLayoutTime() {
	double total = 0;
	// 算每个词所用时间
	for (string& word : word_list) {
		cout << "word:" << word <<endl;
		for (int i = 0; i < word.length()-1; i++ ) {
			// word[i] 和 word[i+1]是current letter和next letter
			// 先根据layout和两个字母 判断是否要换边
			char current_hand = qwertyHandArray[mapLetter(word[i])];
			char next_hand = qwertyHandArray[mapLetter(word[i+1])];
			// 不换边的话
			if (current_hand == next_hand){
				if (current_hand == 'L'){
					// 算出两个键在左手的距离 mm为单位
					// 这里我只算了距离 ->这个可以当作是D 然后根据 MT公式算出所得最后正确时间
					total += L_distmap[word[i]-'a'][word[i+1]-'a'];
					cout << "c&n bothL == " << L_distmap[word[i]-'a'][word[i+1]-'a'] << endl;
					cout << "total: " << total << endl;
				}else{
					// 算出两个键在右手的距离 mm为单位
					// 这里我只算了距离 ->这个可以当作是D 然后根据 MT公式算出所得最后正确时间
					total += R_distmap[word[i]-'a'][word[i+1]-'a'];
					cout << "c&n bothR == " << L_distmap[word[i]-'a'][word[i+1]-'a'] << endl;
					cout << "total: " << total << endl;
				}
			}else{
				// 换边情况全算了1
				cout << "switch " << endl;
				total += 1;
				cout << "total: " << total << endl;
			}
		}
	}
	return total;
}

int main(int argc, char const *argv[])
{
	// process keyboard
	calOriginalCoor();
	long long start = time_stamp();
	maxCR();
	calRealCoor();
	calDist();
	cout << "keyboard processed" << endl;
	cout << "LC: " << LC << "LR:" << LR << "RC:" << RC <<"RR:" <<RR <<endl;

	// process word list
	readWords("qwerty_hashlist.txt");

	// get the ans
	double ans = getLayoutTime();
	cout << "ans: " << ans << endl;
	long long end = time_stamp();
	printf("simulation time: %lf\n", (end - start) / (double)1000000);
	cout << "simulation done!" << endl;



	return 0;
}