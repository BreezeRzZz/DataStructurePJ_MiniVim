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
	COORD coord;			//光标坐标
	std::vector<std::string> fileBuffer;
	std::stack<std::vector<std::string>> undostack;
	std::stack<std::vector<std::string>> redostack;
public:
	//构造函数
	Vim();
	//处理输入
	void handleInput(int ch);
	//模式选择
	void normalAction(int ch);
	void insertAction(int ch);
	//文件处理
	void handleFile(const std::string filename);
	void saveFile(const std::string filename);
	//光标处理
	void cursorMoveLeft();
	void cursorMoveDown();
	void cursorMoveUp();
	void cursorMoveRight();
	bool ifValid();
	void adjustCursor();
	void adjustCursor(const COORD c);
	void updateCursor();
	//插入、删除、换行、显示
	void deletech(int mode);
	void insertch(char ch);
	void startNewLine();
	void displayContent();
	//处理normal模式下符号开头的命令
	std::string adjustCode(char ch);
	COORD findPattern(const std::string& code, COORD coord);
	//撤销、重做
	void undoredo(std::stack<std::vector<std::string>>& stack1, std::stack<std::vector<std::string>>& stack2);
	void undo();
	void redo();
	//向上、向下翻页
	void pageUp();
	void pageDown();
};