#include "miniVim.h"

void Vim::insertAction(int ch)
{
	switch (ch)
	{
	case DIRECTION:			//���������ƶ�
		switch ((ch = getch()))
		{
		case LEFT:
			cursorMoveLeft();
			break;
		case DOWN:
			cursorMoveDown();
			break;
		case UP:
			cursorMoveUp();
			break;
		case RIGHT:
			cursorMoveRight();
			break;
		default:
			break;
		}
		break;
	case ESC:
		mode = normal;
		break;
	case ENTER:
		startNewLine();
		break;
	case BACKSPACE:
		deletech(mode);
		break;
	case TAB:
		for (int i = 0; i < 4; ++i)
			insertch(' ');
		break;
	default:			//�����ı�
		insertch(ch);
		break;
	}
	displayContent();
}
void Vim::cursorMoveLeft()
{
	if (coord.X > 0)
		--coord.X;
}
void Vim::cursorMoveDown()
{
	if (coord.Y + 1 < fileBuffer.size() && fileBuffer[coord.Y + 1].size() > coord.X)		//���·�������
		++coord.Y;
	else if (coord.Y + 1 < fileBuffer.size() && fileBuffer[coord.Y + 1].size() <= coord.X)		//�������һ�е������·�û������
	{
		++coord.Y;
		coord.X = fileBuffer[coord.Y].size();			//����Ƶ���һ��ĩβ
	}
}
void Vim::cursorMoveUp()
{
	if (coord.Y > 0 && fileBuffer[coord.Y - 1].size() > coord.X)			//���Ϸ���������
		--coord.Y;
	else if (coord.Y > 0 && fileBuffer[coord.Y - 1].size() <= coord.X)		//���ǵ�һ�У��������Ϸ�û������
	{
		--coord.Y;
		coord.X = fileBuffer[coord.Y].size();			//�ص���һ��ĩβ
	}
}
void Vim::cursorMoveRight()
{
	if (coord.Y < fileBuffer.size() && coord.X < fileBuffer[coord.Y].size())		//û�г�������
		++coord.X;
}
void Vim::startNewLine()
{
	if (coord.Y < fileBuffer.size())
	{
		std::string& currentLine = fileBuffer[coord.Y];
		std::string RHS(currentLine.begin() + coord.X, currentLine.end());
		currentLine.erase(coord.X, currentLine.size());			//ɾ��x�����������
		fileBuffer.insert(fileBuffer.begin() + coord.Y + 1, RHS);
		cursorMoveDown();
		coord.X = 0;
	}
	else
	{
		fileBuffer.emplace_back("");
		cursorMoveDown();
	}
}
void Vim::deletech(int mode)	//�������ԭ��vim��insertģʽ��BACKSPACE��ɾ����normalģʽ�µ�xɾ���������֣�ǰ���ƶ���꣬���߲���
{
	int x = coord.X, y = coord.Y;			//ɾ����y��x-1�е��ַ�(ɾ�������ߵ��ַ���
	if (fileBuffer.size() > y)
	{
		std::string& str = fileBuffer[y];
		if (x > 0 && x < str.size())			//x���ı��м䣬���Ҷ�������
		{
			str.replace(str.begin() + x - 1, str.end(), str.substr(x, str.size() - x));		//��[x,end)�������滻[x-1,end)
			if (mode == insert)
				cursorMoveLeft();
		}
		else if (!str.empty() && x == str.size())	//��ʱ����ڸ���ĩβ
		{
			str.pop_back();			//ֱ��ɾȥĩβ
			if (mode == insert)
				cursorMoveLeft();		//�ֶ�����
		}
		else if (mode == insert && x == 0)	//��ʱ��Ҫ��������������һ�кϲ�(��������Ϊ����Ҫɾȥ�����)
		{
			if (y != 0)		//�����ǵ�һ��
			{
				coord.X = fileBuffer[y - 1].size();			//�������µ���һ��ĩβ��δ�������ǰ��
				fileBuffer[y - 1].append(str);		//�����м��뵽��һ��
				fileBuffer.erase(fileBuffer.begin() + y);	//ɾȥ��y��
				--coord.Y;
			}
		}
		if (x > str.size())	//����ʱ�������������������������normalģʽ����xɾ�����֣�
			coord.X = str.size();
	}
}
void Vim::insertch(char ch)
{
	int x = coord.X, y = coord.Y;
	if (y < fileBuffer.size())		//��겻�������е�����
	{
		std::string& currentLine = fileBuffer[y];		//�����y������
		if (x < currentLine.size())		//�����ָδ����ĩβ
			currentLine.insert(x, 1, ch);		//��insert����
		else
			currentLine.push_back(ch);			//ֱ����ĩβ����
		cursorMoveRight();			//�ֶ��ٿع������
	}
	else			//��Ҫ�¼�һ��
		fileBuffer.emplace_back("");
}