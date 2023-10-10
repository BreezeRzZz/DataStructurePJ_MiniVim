#pragma once
#include<iostream>
#include<windows.h>
#include<conio.h>
#include<fstream>
#include<vector>
#include<string>
#include<stack>
constexpr int ESC = 27;
constexpr int BACKSPACE = 8;
constexpr int ENTER = 13;
constexpr int DIRECTION = 224;
constexpr int LEFT = 75;
constexpr int DOWN = 80;
constexpr int UP = 72;
constexpr int RIGHT = 77;
constexpr int TAB = 9;
constexpr char DEFAULT_TITLE[] = "mini-Vim";
enum modes { normal, insert };

class Vim
{
private:
	int mode = normal;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord;			//�������
	std::vector<std::string> fileBuffer;
	std::stack<std::vector<std::string>> undostack;
	std::stack<std::vector<std::string>> redostack;
public:
	//���캯��
	Vim();
	//��������
	void handleInput(int ch);
	//ģʽѡ��
	void normalAction(int ch);
	void insertAction(int ch);
	//�ļ�����
	void handleFile(const std::string filename);
	void saveFile(const std::string filename);
	//��괦��
	void cursorMoveLeft();
	void cursorMoveDown();
	void cursorMoveUp();
	void cursorMoveRight();
	bool ifValid();
	void adjustCursor();
	void adjustCursor(const COORD c);
	void updateCursor();
	//���롢ɾ�������С���ʾ
	void deletech(int mode);
	void insertch(char ch);
	void startNewLine();
	void displayContent();
	//����normalģʽ�·��ſ�ͷ������
	std::string adjustCode(char ch);
	COORD findPattern(const std::string& code, COORD coord);
	//����������
	void undoredo(std::stack<std::vector<std::string>>& stack1, std::stack<std::vector<std::string>>& stack2);
	void undo();
	void redo();
	//���ϡ����·�ҳ
	void pageUp();
	void pageDown();
};