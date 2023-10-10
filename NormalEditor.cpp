#include "miniVim.h"

void Vim::normalAction(int ch)
{
	switch (ch)
	{
	case 'i':		//�л���insertģʽ
		mode = insert;
		undostack.push(fileBuffer);			//�������ַ�...(�������insertǰ��״̬��ÿ�л�һ��״̬��Ϊһ�β����Ŀ�ʼ/������
		if (!ifValid())		//����Խ��ʱ����
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
					std::string action(code.begin() + 1, code.begin() + spacePos);		//�Թ���ͷ��":"
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
			adjustCursor(temp);		//�ص�ԭ��λ��
		break;
	}
	case 'u':
		undo();
		adjustCursor();
		break;
	case 'r':			//ԭ����vim��ctrl+r������������r
		redo();
		adjustCursor();
		break;
	case 'x':		//ɾ��
		deletech(mode);
		undostack.push(fileBuffer);			//ɾ���ַ�...(ԭ��vim�ƺ�������normalģʽ�µ�ɾ��)
		break;
	case 'b':		//ԭvim��ctrl+b���Ϸ�ҳ��������b
		pageUp();
		break;
	case 'f':		//ԭvim��ctrl+f���·�ҳ��������f
		pageDown();
		break;
	case 'h':		//�������
		cursorMoveLeft();
		break;
	case 'j':		//�������
		cursorMoveDown();
		break;
	case 'k':		//�������
		cursorMoveUp();
		break;
	case 'l':		//�������
		cursorMoveRight();
		break;
	}
	displayContent();
}
void Vim::handleFile(const std::string filename)		//���ļ����ж�ȡ����������Ϊ��λ�浽fileBuffer��
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
	coord.Y = csbi.srWindow.Bottom;			//�Ƶ����ڵ׶�
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
			if (x > 0 && x != code.size())			//x���ı��м䣬���Ҷ�������
			{
				code.replace(code.begin() + x - 1, code.end(), code.substr(x, code.size() - x));		//��[x,end)�������滻[x-1,end)
				if (coord.X > 0)
					--coord.X;
			}
			else if (!code.empty() && x == code.size())
			{
				code.pop_back();			//ֱ��ɾȥĩβ
				if (coord.X > 0)
					--coord.X;
			}
		}
		else if (c == ENTER)
			break;
		else if (c == DIRECTION)		//������������getch�Ѷ���ļ�����
		{
			getch();
			continue;
		}
		else
		{
			if (x < code.size())		//�����ָδ����ĩβ
				code.insert(x, 1, char(c));		//��insert����
			else
				code.push_back(char(c));			//ֱ����ĩβ����
			++coord.X;
		}
		displayContent();			//��ԭ�ȵ�����(fileBuffer)���س���
		SetConsoleCursorPosition(hOut, { 0,csbi.srWindow.Bottom });		//�����λ�������ڵ׶�����
		std::cout << code;
	}
	return code;	//�����󷵻�
}
COORD Vim::findPattern(const std::string& code, COORD coord)
{
	short x = coord.X, y = coord.Y;		//��¼ԭ�ȹ��λ��
	std::string pattern(code.begin() + 1, code.end());		//�Թ���ͷ��"/"
	std::string text;

	if (fileBuffer.size() > y)		//��֤û��Խ��
	{
		std::string RHS(fileBuffer[y].begin() + x, fileBuffer[y].end());		//��¼��ǰ��ʣ�ಿ��
		text = RHS;
		for (int row = y + 1; row < fileBuffer.size(); ++row)		//��¼����������
			text += fileBuffer[row];
	}
	int pos = text.find(pattern);		//��string��find�㷨
	if (pos != text.npos)			//����ҵ�
	{
		for (int i = 0; i < pos; ++i)
		{
			if (x < fileBuffer[y].size() - 1)		//�����һ����ͣ
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
bool Vim::ifValid()		//�ж������Ƿ���������
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