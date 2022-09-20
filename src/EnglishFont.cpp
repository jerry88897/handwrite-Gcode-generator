#include <fstream>
#include <string>
#include <iostream>
using namespace std;
#define Max_line 30 //最大行數
#define layer_hight 7.9 //行高
#define scale 1 //字體縮放
#define Max_X 170 //超出繪圖範圍警告
int main() {
	int pause;
	string filename = "";
	cout << "輸入檔案名稱(需與本程式在同一個資料夾，且檔名後不須加.txt)" << endl;
	cin >> filename;
	ifstream in(filename + ".txt");
	ifstream din("din.txt");
	ofstream out(filename + ".gcode");
	if (in && din) // 有該檔案
	{
		char line[100], dinword[100];
		int line_no = 0,page=1,out_bound=0,correct_time=0;
		double space = 0.8;
		out << "G21; Set units to mm\n" << "G90         ; Absolute positioning\n" << "M3 S100; Pen Up" << endl;
		for(int i=0;i< correct_time;i++)
			out << "M3 S50\n" << "G1 X-10.00 Y0.00 S70.00 F3000\n" << "M3 S100\n" << "G0 X0.00 Y0.00\n"<<"M0"<< endl;
		while (in.getline(line, 100)) {//一行一行讀
			int white_space = 0, word_count=0;
			double NowX = 0, LastX = 0;
			string line_s(line);
			cout << line_s << endl;
			if (line_no == Max_line) {
				line_no = 0;
				out << "G0 X0 Y0" << endl;
				out << "M0" << endl;
				for (int i = 0; i < correct_time; i++)
					out << "M3 S50\n" << "G1 X-10.00 Y0.00 S70.00 F3000\n" << "M3 S100\n" << "G0 X0.00 Y0.00\n" << "M0" << endl;
				cout << "\n======================<<Change page>>======================\n" << endl;
				page++;
			}
			for (int i = 0; i < strlen(line)+1; i++) {//一個字一個字讀
				if (line_s[i] == ' ') {
					white_space++;
				} else if (line_s[i] == '\0') {
					line_no++;
					break;
				} else if (line_s[i] == '\t') {
					white_space+=4;
				} else {
					word_count++;
					ifstream din("din.txt");
					while (din.getline(dinword, 100)) {//一行一行翻字典
						if (dinword[0] == 'S' && dinword[1] == line_s[i] || dinword[0] == '?' && dinword[1] == '?') {//找到該字母的字典起始位置
							char din_answer[100];
							while (1) {
								din.getline(din_answer, 100);
								string din_answers(din_answer);
								if (din_answers[0] == 'E' && din_answers[1] == line_s[i] || din_answers[0] == '?' && din_answers[1] == '?')break;
								if (din_answers[0] == ';' || din_answers[0] == 'M' || din_answers[0] == '\n' || din_answers[0] == '\0') {
									out << din_answers << endl;
								} else {
									int Xpos = din_answers.find_first_of('X', 0);
									int XEpos = din_answers.find_first_of('.', Xpos);
									int Ypos = din_answers.find_first_of('Y', 0);
									int YEpos = din_answers.find_first_of('.', Ypos);
									int SEpos = din_answers.find_first_of('\n', 0);
									string temp = din_answers.substr(Xpos + 1, XEpos + 2 - Xpos);
									char temp2[10];
									strcpy(temp2, temp.c_str());
									double X = atof(temp2) * scale;
									temp = din_answers.substr(Ypos + 1, YEpos + 2 - Ypos);
									strcpy(temp2, temp.c_str());
									double Y = atof(temp2) * scale;
									if (word_count == 0) {
										X += white_space * space * 2 + NowX;
									} else {
										X += white_space * space * 2 + 0.5 + NowX;
									}
									Y -= line_no * layer_hight;
									if (X > LastX)LastX = X;
									out << din_answers.substr(0, Xpos + 1) << X << " Y" << Y << din_answers.substr(YEpos + 3, strlen(din_answer) - YEpos - 1) << endl;
								}
							}
							break;
						}
					}
					din.close();
					white_space = 0;
					NowX = LastX;
				}
			}
			if (NowX >= Max_X) {
				cout << "!!!!!!!!!!!!!!!!!!!!<<Out of boundary>>!!!!!!!!!!!!!!!!!!!!\n" << endl;
				out_bound++;
			}
		}
		cout << "完成! 共 "<<page<<" 頁, "<<out_bound<<" 行超出範圍" << endl;
	} else if (!in) // 沒有該檔案
	{
		cout << "no such file" << endl;
	} else {
		cout << "din not found" << endl;
	}
	cin >> pause;
	return 0;
}