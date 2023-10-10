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
void Vim::adjustCursor()		//������������ǰ�ı�����λ��
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
	system("CLS");			//������ͬʱ�Ὣ����ʾ�ģ����������(0,0)
	for (int i = 0; i < fileBuffer.size(); ++i)
	{
		printf("%s", fileBuffer[i].c_str());
		if (i != fileBuffer.size() - 1)
			printf("\n");
	}
	updateCursor();
}
void Vim::updateCursor()		//���¹��λ��
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