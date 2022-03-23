/*
	������ܿ�����־
	1. ������Ŀ
	2. �����ز�
	3. ������Ϸ����
			ѡ��ͼ�ο�
			������ܻ��ڡ�easyX��ͼ�ο�
			1)��������
			2�����ر�������loadimage
				a. 3���ֱ�����Զ�������Բ�ͬ�ٶ��ƶ�
				b. ѭ��������������ͼƬ�����������ڳ��ȣ�ѭ������
			3��ʵ�ֱ�������putimage
				���⣺����ͼƬpng͸���ʺ�ɫ����tool�ļ���putimagePNG2����
	4. ʵ����ұ���
	5. ʵ�������Ծ�����û�������������
	6. ʵ������ڹ�
	7. �����ϰ���ṹ����������
	8. ʹ���ϰ���ṹ������³�ʼ��
	9. ��װ�����ϰ������ʾ
	10. ʵ��Ӣ���¶�
	11. ʵ���ϰ�����ӡ�
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>
#include <vector>//cpp�е�����ʵ�ֿɱ�����
#include "tools.h"

using namespace std;

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define HORIZON 348    //��ƽ��
#define OBSTACLE_CNT 6//�ϰ�����
#define NUMOFBGIMG 3
#define NUMOFHEROIMG 12
#define NUMOFTORTOISEIMG 7
#define NUMOFLIONIMG 6
#define NUMOFCOLIMG 4
#define WIN_SCORE 1000
//ȫ�ֱ�������Ϸ��������
IMAGE bgimgs[NUMOFBGIMG];
int bgX[NUMOFBGIMG] = { 0 };
int bgSpeed[NUMOFBGIMG] = { 1, 2, 4 };
//Ӣ��ͼƬ
IMAGE heroimgs[NUMOFHEROIMG];
int heroX;
int heroY;
int heroIndex;
bool heroJump;
bool heroDown;
//int jumpMaxHeight;
int jumpOffset;
bool update;//ҳ��ˢ�±�־λ����ʾ�Ƿ���Ҫ����ˢ��ҳ��
//�¶�
IMAGE imgHeroDown[2];
int heroBlood;
int score;

typedef enum
{
	TORTOISE,//�ڹ�	0
	LION,//ʨ��		1
	COL1,//����
	COL2,
	COL3,
	COL4,
	OBS_TYPE_CNT//�ϰ�������
}obstacle_type;
vector<vector<IMAGE>>obstacleImgs;//�ɱ������Ÿ����ϰ���ͼƬ
typedef struct
{
	int type;//����
	int imgIndex;
	int x, y;//����
	int speed;//�ٶ�
	int force;//ɱ����
	bool exist;
	bool hited;
	bool scoreCNT;
}obstacle;
obstacle obs[OBSTACLE_CNT];//�ϰ���������
IMAGE imgsNUM[10];

//��Ϸ��ʼ��
void init()
{
	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	//������Ϸ����
	char filePath[64];
	heroBlood = 100;
	for (int i = 0; i < NUMOFBGIMG; i++)
	{
		//  ./res/bg001.png
		sprintf(filePath, "./res/bg%03d.png", i + 1);
		loadimage(&bgimgs[i], filePath);
	}
	//����Ӣ�۱���
	for (int i = 0; i < NUMOFHEROIMG; i++)
	{
		sprintf(filePath, "./res/hero%d.png", i + 1);
		loadimage(&heroimgs[i], filePath);
	}
	//����Ӣ�۳�ʼλ��
	heroX = (WIN_WIDTH - heroimgs[0].getwidth()) / 2;
	heroY = HORIZON - heroimgs[0].getheight();
	heroIndex = 0;
	//��ʼӢ����Ծ����
	heroJump = false;
	heroDown = false;
	jumpOffset = -16;
	update = true;

	//����Ӣ���¶�
	for (int i = 0; i < 2; i++)
	{
		sprintf(filePath, "./res/d%d.png", i + 1);
		loadimage(&imgHeroDown[i], filePath);
	}

	//�����ڹ����obstacle����
	IMAGE torImg;
	vector<IMAGE> torImgArr;
	for (int i = 0; i < NUMOFTORTOISEIMG; i++)
	{
		sprintf(filePath, "./res/t%d.png", i + 1);
		loadimage(&torImg, filePath);
		torImgArr.push_back(torImg);
	}
	obstacleImgs.push_back(torImgArr);
	//����ʨ�ӽ���obstacle����
	IMAGE lionImgs;
	vector<IMAGE> lionImgsArr;
	for (int i = 0; i < NUMOFLIONIMG; i++)
	{
		sprintf(filePath, "./res/p%d.png", i + 1);
		loadimage(&lionImgs, filePath);
		lionImgsArr.push_back(lionImgs);
	}
	obstacleImgs.push_back(lionImgsArr);
	//�������ӽ���obstacle����
	IMAGE colImgs;
	for (int i = 0; i < NUMOFCOLIMG; i++)
	{
		vector<IMAGE> colImgsArr;
		sprintf(filePath, "./res/h%d.png", i + 1);
		loadimage(&colImgs, filePath, 63, 260, true);
		colImgsArr.push_back(colImgs);
		obstacleImgs.push_back(colImgsArr);
	}
	//��ʼ���ϰ���أ�ȫ��ʼ��Ϊ������
	for (int i = 0; i < OBSTACLE_CNT; i++)
	{
		obs[i].exist = false;
	}
	//Ԥ������Ч
	preLoadSound("./res/hit.mp3");
	mciSendString("play ./res/bg.mp3 repeat", 0, 0, 0);
	score = 0;
	//��������ͼƬ
	for (int i = 0; i < 10; i++)
	{
		sprintf(filePath, "./res/num/%d.png", i);
		loadimage(&imgsNUM[i], filePath);

	}
}


//��Ⱦ����
void updateBg()
{
	putimagePNG2(bgX[0], 0, &bgimgs[0]);
	putimagePNG2(bgX[1], 119, &bgimgs[1]);
	putimagePNG2(bgX[2], 330, &bgimgs[2]);
}

void creatObs()
{
	int i;
	for (i = 0; i < OBSTACLE_CNT; i++)//��δ���ڵ��ϰ���
	{
		if (obs[i].exist == false)
		{
			break;
		}
	}
	if (i == OBSTACLE_CNT)//δ�ҵ�,�������ϰ��ﶼ����
	{
		return;
	}
	obs[i].exist = true;
	obs[i].hited = false;
	obs[i].scoreCNT = false;
	obs[i].imgIndex = 0;
	//obs[i].type = (obstacle_type)(rand() % OBS_TYPE_CNT);
	obs[i].type = rand() % 3;//���������ϰ���
	if (i >= 1 && obs[i].type == LION && obs[i-1].type == COL1 && obs[i-1].x > WIN_WIDTH-800)
	{
		obs[i].type == TORTOISE;
	}
	if (obs[i].type == COL1)//2~5
	{
		obs[i].type += rand() % 4;
	}
	obs[i].x = WIN_WIDTH;
	obs[i].y = HORIZON - obstacleImgs[obs[i].type][0].getheight();
	if (obs[i].type == TORTOISE)
	{
		obs[i].speed = 1;
		obs[i].force = 5;
	}
	else if (obs[i].type == LION)
	{
		obs[i].speed = 4;
		obs[i].force = 20;
	}
	else if (obs[i].type >= COL1 && obs[i].type <= COL4)
	{
		obs[i].speed = 0;
		obs[i].force = 20;
		obs[i].y = 0;
	}
}


void pictureChange()
{
	//����1��2��3ͼ�ƶ�
	for (int i = 0; i < NUMOFBGIMG; i++)
	{
		bgX[i] -= bgSpeed[i];
		if (bgX[i] <= -WIN_WIDTH)
		{
			bgX[i] = 0;
		}
	}
	//ʵ����Ծ����Ծʱ����֡���䣬��������
	if (heroJump)//��Ծ
	{
		heroY += jumpOffset;
		jumpOffset += 1;
		if (heroY > HORIZON - heroimgs[0].getheight())//�����¶��㣬��λ�����ó�ʼ��
		{
			jumpOffset = -16;
			heroY = HORIZON - heroimgs[0].getheight();
			heroJump = false;
		}
	}
	else if(heroDown) //�¶�
	{
		static int cnt = 0;
		int delay[2] = { 8, 32 };
		cnt++;
		if (cnt >= delay[heroIndex])
		{
			cnt = 0;
			heroIndex++;
			if (heroIndex == 2)
			{
				heroIndex = 0;
				heroDown = false;
			}
		}
	}
	else//����
	{
		//Ӣ�۱��ܶ���֡�仯
		heroIndex = (heroIndex + 1) % NUMOFHEROIMG;
	}

	//�����ϰ����ʼ����Ӧ�ϰ���Ĳ���
	static int frameCnt = 0;
	static int enemyFrequ = 50;//100~400
	//srand((unsigned int)time(NULL));
	frameCnt++;
	if (frameCnt > enemyFrequ)
	{
		frameCnt = 0;
		enemyFrequ = 50 + rand() % 201;
		creatObs();
	}

	//�����ϰ������
	for (int i = 0; i < OBSTACLE_CNT; i++)
	{
		if (obs[i].exist)
		{
			obs[i].x -= obs[i].speed + bgSpeed[2];
			obs[i].imgIndex = (obs[i].imgIndex + 1) % obstacleImgs[obs[i].type].size();
		}
		if (obs[i].x < -obstacleImgs[obs[i].type][0].getwidth() * 2)
		{
			obs[i].exist = false;
		}
	}




	update = false;
}
//��Ⱦ�ϰ���
void updateEnemy()
{
	for (int i = 0; i < OBSTACLE_CNT; i++)
	{
		if (obs[i].exist)
		{
			putimagePNG2(obs[i].x, obs[i].y, WIN_WIDTH, &obstacleImgs[obs[i].type][obs[i].imgIndex]);
		}
	}
	
}

void updateHero()
{
	if (heroDown)//�¶�״̬
	{
		int downY = HORIZON - imgHeroDown[heroIndex].getheight();
		putimagePNG2(heroX, downY, &imgHeroDown[heroIndex]);//��ȾӢ���ܲ�
	}
	else//�Ƕף����ܲ�
	{
		putimagePNG2(heroX, heroY, &heroimgs[heroIndex]);//��ȾӢ���ܲ�
	}
}
void updateBloodBar()
{
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroBlood / 100.0);
}
void updateScore()
{
	//������תΪ�ַ����������ж�
	int x = 10;
	int y = 25;
	char scoreStr[8];
	sprintf(scoreStr, "%d", score);
	for (int i = 0; scoreStr[i]; i++)
	{
		int num = scoreStr[i] - '0';
		putimagePNG2(x, y, &imgsNUM[num]);
		x += imgsNUM[num].getwidth() + 5;
	}
}
void cheakHit()//��ײ���
{
	for (int i = 0; i < OBSTACLE_CNT; i++)
	{
		if (obs[i].exist && !obs[i].hited)
		{
			int a1x, a1y, a2x, a2y;
			int off = 22;//ƫ����
			if (heroDown)//�¶�״̬
			{
				a1x = heroX + off;
				a1y = HORIZON - imgHeroDown[heroIndex].getheight() + off;
				a2x = heroX + imgHeroDown[heroIndex].getwidth() - off;
				a2y = HORIZON - off;
			}
			else//�Ƕף����ܡ���Ծ״̬
			{
				a1x = heroX + off;
				a1y = heroY + off;
				a2x = heroX + heroimgs[0].getwidth() - off;
				a2y = heroY + heroimgs[0].getheight() - off;
			}
			int b1x = obs[i].x + off;
			int b1y = obs[i].y + off;
			int b2x = obs[i].x + obstacleImgs[obs[i].type][obs[i].imgIndex].getwidth() - off;
			int b2y = obs[i].y + obstacleImgs[obs[i].type][obs[i].imgIndex].getheight() - off;
			if (rectIntersect(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y))
			{
				obs[i].hited = true;
				heroBlood -= obs[i].force;
				printf("ʣ��Ѫ��%d\n", heroBlood);
				playSound("./res/hit.mp3");
			}
		}
	}
}

void cheakGameOver()
{
	if (heroBlood <= 0)
	{
		loadimage(0, "./res/over.png");
		FlushBatchDraw();
		mciSendString("stop ./res/bg.mp3", 0, 0, 0);
		system("pause");
		//��ͣ��ֱ�ӿ�ʼ��һ��
		init();
		mciSendString("play ./res/bg.mp3 repeat", 0, 0, 0);
	}
}
void gameStart()
{
	loadimage(0, "./res/over.png");
	system("pause");
}
void cheakScore()
{
	for (int  i = 0; i < OBSTACLE_CNT; i++)
	{
		if (obs[i].exist && obs[i].hited == false && 
			heroX > obs[i].x + obstacleImgs[obs[i].type][obs[i].imgIndex].getwidth() &&
			obs[i].scoreCNT == false)
		{
			score += 10;
			obs[i].scoreCNT = true;
			printf("������%d\n", score);
		}
	}
}

void cheakVictory()
{
	if (score >= WIN_SCORE)
	{
		FlushBatchDraw();
		Sleep(1000);
		loadimage(0, "./res/win.png");
		FlushBatchDraw();
		mciSendString("stop ./res/bg.mp3", 0, 0, 0);
		mciSendString("play ./res/win.mp3", 0, 0, 0);
		system("pause");
		//��ͣ��ֱ�ӿ�ʼ��һ��
		init();
		mciSendString("play ./res/bg.mp3 repeat", 0, 0, 0);
	}
}
//�����û�����
void keyEvent()
{
	if (_kbhit())//���а��������ǣ������档������������Ϸ����
	{
		char ch = _getch();//���谴�»س���
		if (ch == ' ' || ch == 72 || ch == 'w' || ch == 'W')
		{
			heroJump = true;
			update = true;
		}
		else if (ch == 80 || ch == 's' || ch == 'S')
		{
			update = true;
			heroDown = true;
			heroIndex = 0;
		}
	}

}
int main()
{
	int timecnt = 0;
	init();
	gameStart();
	while (1)
	{
		keyEvent();
		//����ˢ���ٶ�
		timecnt += getDelay();
		if (timecnt > 30)
		{
			timecnt = 0;
			update = true;
		}
		if (update)
		{
			BeginBatchDraw();
			updateBg();
			updateHero();
			updateEnemy();
			updateBloodBar();
			updateScore();
			cheakVictory();
			EndBatchDraw();

			pictureChange();
			cheakHit();
			cheakGameOver();
			cheakScore();
		}
		
	}

	return 0;
}