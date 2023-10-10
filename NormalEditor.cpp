#include "miniVim.h"

void Vim::normalAction(int ch)
{
	switch (ch)
	{
	case 'i':		//切换到insert模式
		mode = insert;
		undostack.push(fileBuffer);			//插入多个字符...(保存进入insert前的状态，每切换一次状态视为一次操作的开始/结束）
		if (!ifValid())		//坐标越界时调整
			adjustCursor();
		break;
	case ':':
	case '/':
	{
		auto temp = coord;
		auto code = adjustCode(ch);
		if (!code.empty())
		{
			if (code == ":q")
				exit(0);
			else if (code[0] == ':')
			{
				auto spacePos = code.find(' ');
				if (spacePos != code.npos)
				{
					std::string action(code.begin() + 1, code.begin() + spacePos);		//略过开头的":"
					std::string filename(code.begin() + spacePos + 1, code.end());

					if (action == "open")
						handleFile(filename);
					else if (action == "w")
						saveFile(filename);
				}
				adjustCursor();
			}
			else if (code[0] == '/')
				coord = findPattern(code, temp);
		}
		else
			adjustCursor(temp);		//回到原来位置
		break;
	}
	case 'u':
		undo();
		adjustCursor();
		break;
	case 'r':			//原本的vim用ctrl+r重做，这里用r
		redo();
		adjustCursor();
		break;
	case 'x':		//删除
		deletech(mode);
		undostack.push(fileBuffer);			//删除字符...(原版vim似乎仅撤销normal模式下的删除)
		break;
	case 'b':		//原vim用ctrl+b向上翻页，这里用b
		pageUp();
		break;
	case 'f':		//原vim用ctrl+f向下翻页，这里用f
		pageDown();
		break;
	case 'h':		//光标左移
		cursorMoveLeft();
		break;
	case 'j':		//光标下移
		cursorMoveDown();
		break;
	case 'k':		//光标上移
		cursorMoveUp();
		break;
	case 'l':		//光标右移
		cursorMoveRight();
		break;
	}
	displayContent();
}
void Vim::handleFile(const std::string filename)		//从文件流中读取，内容以行为单位存到fileBuffer中
{
	std::ifstream infile;
	char c;
	infile.open(filename);
	if (infile.is_open())
	{
		fileBuffer.clear();
		fileBuffer.emplace_back("");
		while ((c = infile.get()) != EOF)
		{
			if (c != '\n')
				fileBuffer[fileBuffer.size() - 1] += c;
			else
				fileBuffer.emplace_back("");
		}
		adjustCursor();
		infile.close();
	}
}
void Vim::saveFile(const std::string filename)
{
	std::ofstream outfile(filename);
	for (int i = 0; i < fileBuffer.size(); ++i)
	{
		if (i == fileBuffer.size() - 1)
			outfile << fileBuffer[i];
		else
			outfile << fileBuffer[i] << std::endl;
	}
	outfile.close();
}
std::string Vim::adjustCode(char ch)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hOut, &csbi);
	coord.Y = csbi.srWindow.Bottom;			//移到窗口底端
	coord.X = 0;
	updateCursor();
	printf("%c", ch);
	coord.X = 1;
	updateCursor();
	int c;
	std::string code{ char(ch) };
	while (!code.empty())
	{
		c = getch();
		int x = coord.X;
		if (c == BACKSPACE)
		{
			if (x > 0 && x != code.size())			//x在文本中间，左右都有内容
			{
				code.replace(code.begin() + x - 1, code.end(), code.substr(x, code.size() - x));		//用[x,end)的内容替换[x-1,end)
				if (coord.X > 0)
					--coord.X;
			}
			else if (!code.empty() && x == code.size())
			{
				code.pop_back();			//直接删去末尾
				if (coord.X > 0)
					--coord.X;
			}
		}
		else if (c == ENTER)
			break;
		else if (c == DIRECTION)		//如果方向键，用getch把多余的键读掉
		{
			getch();
			continue;
		}
		else
		{
			if (x < code.size())		//光标所指未超过末尾
				code.insert(x, 1, char(c));		//用insert插入
			else
				code.push_back(char(c));			//直接在末尾插入
			++coord.X;
		}
		displayContent();			//将原先的内容(fileBuffer)加载出来
		SetConsoleCursorPosition(hOut, { 0,csbi.srWindow.Bottom });		//将输出位置设置在底端最左
		std::cout << code;
	}
	return code;	//结束后返回
}
COORD Vim::findPattern(const std::string& code, COORD coord)
{
	short x = coord.X, y = coord.Y;		//记录原先光标位置
	std::string pattern(code.begin() + 1, code.end());		//略过开头的"/"
	std::string text;

	if (fileBuffer.size() > y)		//保证没有越界
	{
		std::string RHS(fileBuffer[y].begin() + x, fileBuffer[y].end());		//记录当前行剩余部分
		text = RHS;
		for (int row = y + 1; row < fileBuffer.size(); ++row)		//记录后面所有行
			text += fileBuffer[row];
	}
	int pos = text.find(pattern);		//用string的find算法
	if (pos != text.npos)			//如果找到
	{
		for (int i = 0; i < pos; ++i)
		{
			if (x < fileBuffer[y].size() - 1)		//到最后一个就停
				++x;
			else
			{
				x = 0;
				++y;
			}
		}
	}
	return { x,y };
}
bool Vim::ifValid()		//判断坐标是否在文字内
{
	return coord.Y < fileBuffer.size() && coord.X <= fileBuffer[coord.Y].size();
}
void Vim::undoredo(std::stack<std::vector<std::string>>& stack1, std::stack<std::vector<std::string>>& stack2)
{
	if (!stack1.empty())
	{
		stack2.push(fileBuffer);
		fileBuffer = stack1.top();
		stack1.pop();
	}
}
void Vim::undo()
{
	undoredo(undostack, redostack);
}
void Vim::redo()
{
	undoredo(redostack, undostack);
}
void Vim::pageUp()
{
	int pageSize = 30;
	if (coord.Y >= pageSize)
		coord.Y = coord.Y - pageSize / 2;
}
void Vim::pageDown()
{
	int pageSize = 30;
	if (fileBuffer.size() > pageSize && coord.Y + pageSize / 2 < fileBuffer.size())
		coord.Y = coord.Y + pageSize / 2;
}