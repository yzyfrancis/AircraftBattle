#include <iostream>
#include <graphics.h>
#include <vector>
#include <conio.h>
using namespace std;
constexpr auto swidth = 600;
constexpr auto sheight = 1100;

bool PointInRect(int x, int y, RECT& r)
{
	return(r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}

bool RectCrushRect(RECT &r1, RECT &r2) {
	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;
	return (r.left < r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);
}

//开始

void Welcome()
{
	const wchar_t* title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");

	RECT tplayr, texitr;
	BeginBatchDraw();
	setbkcolor(WHITE);
	cleardevice();
	settextcolor(BLACK);
	settextstyle(60, 0, _T("黑体"));

	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);
	settextstyle(40, 0, _T("黑体"));
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.top = sheight / 5*2.5;
	tplayr.bottom = tplayr.top + textheight(tplay);

	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.right = texitr.left + textwidth(texit);
	texitr.top = sheight / 5*3;
	texitr.bottom = texitr.top + textheight(texit);

	outtextxy(tplayr.left, tplayr.top, tplay);
	outtextxy(texitr.left, texitr.top, texit);

	EndBatchDraw();

	while (true)
	{
		ExMessage message;
		getmessage(&message,EM_MOUSE);
		if (message.lbutton) 
		{
			if (PointInRect(message.x, message.y, tplayr))
			{
				return;
			}
			else if (PointInRect(message.x, message.y, texitr))
			{
				exit(0);
			}
		}
	}

}

//结束
void Over(unsigned long long &kill) {
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str, 128, _T("击杀数：%llu"), kill);

	settextcolor(RED);
	outtextxy(swidth / 2 - textwidth(str) / 2, sheight / 5, str);
	//键盘事件
	LPCTSTR info = _T("按Enter键返回");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(swidth - textwidth(info), sheight - textheight(info), info);

	while (true) {
		ExMessage message;
		getmessage(&message, EM_KEY);
		if (message.vkcode == 0x0D) {
			return;
		}
	}

}
//背景、敌机、英雄、子弹
class BK
{
public:
	BK(IMAGE& img)
		:img(img),y(-sheight)
	{

	}
	void Show()
	{
		if(y==0)
		{
			y=-sheight;
		}
		y += 4;
		putimage(0, y, &img);
	}
private:
	IMAGE& img;
	int y;
};

class Hero
{
public:
	Hero(IMAGE& img)
		:img(img)
	{
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = sheight;

	}
	void Show()
	{
		
		putimage(rect.left, rect.top,&img);
	}
	void Control()
	{
		ExMessage message;
		if (peekmessage(&message, EM_MOUSE))
		{
			rect.left = message.x - img.getwidth() / 2;
			rect.top = message.y - img.getheight() / 2;
			rect.right = rect.left + img.getwidth();
			rect.bottom = rect.top + img.getheight();
		}
	}
	RECT& GetRect() {
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;
};

class Enemy
{
public:
	Enemy(IMAGE& img, int x)
		:img(img)
	{
		rect.left = x;
		rect.top = -img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = 0;

	}
	bool Show()
	{
		if (rect.top >= sheight)
		{
			return false;
		}
		rect.top += 4;
		rect.bottom += 4;
		putimage(rect.left, rect.top, &img);

		return true;
	}
	RECT& GetRect() {
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;
};

class Bullet {
public:
	Bullet(IMAGE& img, RECT pr)
		:img(img)
	{
		rect.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect.right = rect.left + img.getwidth();
		rect.top = pr.top - img.getheight();
		rect.bottom = rect.top + img.getheight();
	}
	bool Show() 
	{
		if (rect.bottom <= 0)
		{
			return false;
		}
		rect.top -= 3;
		rect.bottom -= 3;
		putimage(rect.left, rect.top, &img);
	}
	RECT& GetRect() {
		return rect;
	}
private:
	RECT rect;
	IMAGE& img;
};

bool AddEnemy(vector<Enemy*> &es,IMAGE &enemyimg) {

	Enemy* e = new Enemy(enemyimg, abs(rand()) % (swidth - enemyimg.getwidth()));
	for (auto& i : es)
	{
		if (RectCrushRect(i->GetRect(), e->GetRect())) {
			delete e;
			return false;
		}
	}
	es.push_back(e);
	return true;
}

bool Play()
{
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;
	IMAGE heroimg, enemyimg, bkimg, bulletimg;
	loadimage(&heroimg, _T("D:\\学习资料\\研究生\\自学项目\\c++\\项目\\PLANE\\images\\me1.png"));
	loadimage(&enemyimg, _T("D:\\学习资料\\研究生\\自学项目\\c++\\项目\\PLANE\\images\\enemy1.png"));
	loadimage(&bkimg, _T("D:\\学习资料\\研究生\\自学项目\\c++\\项目\\PLANE\\images\\bk2.png"),swidth,sheight*2);
	loadimage(&bulletimg, _T("D:\\学习资料\\研究生\\自学项目\\c++\\项目\\PLANE\\images\\bullet1.png"));

	BK bk = BK(bkimg);
	Hero hp = Hero(heroimg);

	vector<Enemy*> es;
	vector<Bullet*> bs;
	int bsimg=0;

	unsigned long long kill = 0;
	for (int i = 0; i < 5; i++)
	{
		AddEnemy(es, enemyimg);
	}

	while (is_play)
	{
		bsimg++;
		if (bsimg == 10) {
			bsimg = 0;
			bs.push_back(new Bullet(bulletimg, hp.GetRect()));
		}
		BeginBatchDraw();

		bk.Show();
		Sleep(6);
		flushmessage();
		Sleep(2);
		hp.Control();
		if (_kbhit())
		{
			char v = _getch();
			if (v == 0x20) {
				Sleep(500);
				while (true) {
					if (_kbhit())
					{
						v = _getch();
						if (v == 0x20) {
							break;
						}
					}
					Sleep(16);
				}
			}
		}
		
		hp.Show();

		for (auto& i : bs) {
			i->Show();
		}
		auto it = es.begin();
		while (it != es.end())
		{
			if (RectCrushRect((*it)->GetRect(), hp.GetRect())){
				is_play = false;
			}
			auto bit = bs.begin();
			while (bit != bs.end()) {
				if (RectCrushRect((*bit)->GetRect(), (*it)->GetRect())) {
					delete (*it);
					es.erase(it);
					it = es.begin();
					delete (*bit);
					bs.erase(bit);
					kill++;
					break;

				}
				bit++;
			}
			if (!(*it)->Show())
			{
				delete (*it);
				es.erase(it);
				it = es.begin();
			}
			it++;
		}
		for (int i = 0; i < 5 - es.size(); i++)
		{
			AddEnemy(es, enemyimg);
		}
		EndBatchDraw();
	}
	Over(kill);
	return true;
}

int main()
{
	initgraph(swidth, sheight, EW_NOMINIMIZE | EW_SHOWCONSOLE);
	bool is_live = true;
	while (is_live)
	{
		Welcome();
		//play
		is_live = Play();
	}

	cin.get();

}