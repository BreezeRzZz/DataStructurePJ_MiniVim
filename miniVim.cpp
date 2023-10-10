#include "miniVim.h"

Vim::Vim()
{
	mode = normal;
	coord = { 0,0 };
	fileBuffer.emplace_back("");
	SetConsoleTitle(DEFAULT_TITLE);
}

void Vim::handleInput(int ch)
{
	switch (mode)
	{
	case normal:
		normalAction(ch);
		break;
	case insert:
		insertAction(ch);
		break;
	}
}
void Vim::adjustCursor()		//将光标调整到当前文本最后的位置
{
	coord.Y = fileBuffer.size() - 1;
	coord.X = fileBuffer[coord.Y].size();
}
void Vim::adjustCursor(const COORD c)
{
	coord = c;
}
void Vim::displayContent()
{
	system("CLS");			//清屏，同时会将（显示的）光标坐标置(0,0)
	for (int i = 0; i < fileBuffer.size(); ++i)
	{
		printf("%s", fileBuffer[i].c_str());
		if (i != fileBuffer.size() - 1)
			printf("\n");
	}
	updateCursor();
}
void Vim::updateCursor()		//更新光标位置
{
	SetConsoleCursorPosition(hOut, coord);
}

int main()
{
	Vim vim;
	int ch;
	while (true)
	{
		ch = getch();
		vim.handleInput(ch);
	}
	return 0;
}