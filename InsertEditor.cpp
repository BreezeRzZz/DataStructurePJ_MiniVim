#include "miniVim.h"

void Vim::insertAction(int ch)
{
	switch (ch)
	{
	case DIRECTION:			//上下左右移动
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
	default:			//输入文本
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
	if (coord.Y + 1 < fileBuffer.size() && fileBuffer[coord.Y + 1].size() > coord.X)		//正下方有内容
		++coord.Y;
	else if (coord.Y + 1 < fileBuffer.size() && fileBuffer[coord.Y + 1].size() <= coord.X)		//不是最后一行但是正下方没有内容
	{
		++coord.Y;
		coord.X = fileBuffer[coord.Y].size();			//光标移到下一行末尾
	}
}
void Vim::cursorMoveUp()
{
	if (coord.Y > 0 && fileBuffer[coord.Y - 1].size() > coord.X)			//正上方还有内容
		--coord.Y;
	else if (coord.Y > 0 && fileBuffer[coord.Y - 1].size() <= coord.X)		//不是第一行，但是正上方没有内容
	{
		--coord.Y;
		coord.X = fileBuffer[coord.Y].size();			//回到上一行末尾
	}
}
void Vim::cursorMoveRight()
{
	if (coord.Y < fileBuffer.size() && coord.X < fileBuffer[coord.Y].size())		//没有超过本行
		++coord.X;
}
void Vim::startNewLine()
{
	if (coord.Y < fileBuffer.size())
	{
		std::string& currentLine = fileBuffer[coord.Y];
		std::string RHS(currentLine.begin() + coord.X, currentLine.end());
		currentLine.erase(coord.X, currentLine.size());			//删除x及后面的内容
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
void Vim::deletech(int mode)	//这里仿照原版vim对insert模式的BACKSPACE的删除和normal模式下的x删除做了区分，前者移动光标，后者不会
{
	int x = coord.X, y = coord.Y;			//删除第y行x-1列的字符(删除光标左边的字符）
	if (fileBuffer.size() > y)
	{
		std::string& str = fileBuffer[y];
		if (x > 0 && x < str.size())			//x在文本中间，左右都有内容
		{
			str.replace(str.begin() + x - 1, str.end(), str.substr(x, str.size() - x));		//用[x,end)的内容替换[x-1,end)
			if (mode == insert)
				cursorMoveLeft();
		}
		else if (!str.empty() && x == str.size())	//此时光标在该行末尾
		{
			str.pop_back();			//直接删去末尾
			if (mode == insert)
				cursorMoveLeft();		//手动左移
		}
		else if (mode == insert && x == 0)	//此时需要将该行内容与上一行合并(包括该行为空需要删去的情况)
		{
			if (y != 0)		//不能是第一行
			{
				coord.X = fileBuffer[y - 1].size();			//将光标更新到上一行末尾（未加入该行前）
				fileBuffer[y - 1].append(str);		//将该行加入到上一行
				fileBuffer.erase(fileBuffer.begin() + y);	//删去第y行
				--coord.Y;
			}
		}
		if (x > str.size())	//超界时将光标调整回来（这种情况仅在normal模式下用x删除出现）
			coord.X = str.size();
	}
}
void Vim::insertch(char ch)
{
	int x = coord.X, y = coord.Y;
	if (y < fileBuffer.size())		//光标不超过现有的文字
	{
		std::string& currentLine = fileBuffer[y];		//考察第y行文字
		if (x < currentLine.size())		//光标所指未超过末尾
			currentLine.insert(x, 1, ch);		//用insert插入
		else
			currentLine.push_back(ch);			//直接在末尾插入
		cursorMoveRight();			//手动操控光标右移
	}
	else			//需要新加一行
		fileBuffer.emplace_back("");
}