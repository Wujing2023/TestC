//
// Created by wj on 22-3-28.
//

#include "HEAD.h"

int record[ROW][COL]; //记录位置
int playtime;
XX *H;
int max,score;//最高分和分数，全局变量

int max,score;//最高分和分数，全局变量
void HideCursor();
void CursorSet(int x, int y);
void InitInterface();
void color(int c);
void RScore();
void WScore();
void InitSnake();
void FoodRand();
void ScoreAndEnd(int x, int y);
void AppearSnake(int flag);
void MoveSnake(int x, int y);
void Game();
void saveTandS(XX *Head,int score);
void PrintScore();


void PrintScore()
{
    int i;
    XX *p=H;
    system("cls");
    printf("游玩次数:%d\n",playtime);
    for(i=0;i<playtime;i++)
    {
        p=p->next;
        struct tm *t;
        t=localtime(&(p->a.T));
        printf("%s分数:%d\n\n",asctime(t),p->a.score);
    }
    system("pause");
}

void saveTandS(XX *He,int score)
{
    playtime++;
    XX *temp;
    temp=(XX*)malloc(sizeof(XX));
    temp->next=He->next;
    He->next=temp;
    temp->a.T=time(NULL);
    struct tm *t;
    t=localtime(&(temp->a.T));
    temp->a.score=score;
}

void HideCursor()//隐藏光标
{
    CONSOLE_CURSOR_INFO curInfo; //定义光标信息
    curInfo.dwSize = 1; //赋值后才能隐藏光标
    curInfo.bVisible = FALSE; //隐藏光标
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
    SetConsoleCursorInfo(handle, &curInfo); //设置隐藏光标
}

void CursorSet(int x, int y)//设置光标位置,为了使光标处于一个定值，使页面更流畅好看
{
    COORD pos; //定义光标位置
    pos.X = x; //光标横坐标
    pos.Y = y; //光标纵坐标
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
    SetConsoleCursorPosition(hOut, pos); //设置光标位置
}

void InitInterface()//界面初始化
{
    int i,j;
    color(9);//颜色设置
    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            if (j == 0 || j == COL - 1)
            {
                record[i][j] = WALL; //记录为墙
                CursorSet(2 * j, i);
                printf("■");
            }
            else if (i == 0 || i == ROW - 1)
            {
                record[i][j] = WALL; //记录为墙
                CursorSet(2 * j, i);
                printf("■");
            }
            else
            {
                record[i][j] = EMPTY; //记录为无
            }
        }
    }
    color(15); //颜色设置
    CursorSet(0, ROW);
    printf("得分:%d", score);
    CursorSet(COL, ROW);
    printf("最高得分:%d", max);
}

void color(int c)//设置颜色
{
    HANDLE hOut=GetStdHandle(STD_OUTPUT_HANDLE);//获取控制台句柄
    SetConsoleTextAttribute(hOut, c);
}

void RScore()//读取最高分
{
    int i;
    FILE* pf = fopen("贪吃蛇最高得分记录.txt", "r"); //以只读的方式打开文件
    if (pf == NULL) //打开文件失败
    {
        pf = fopen("贪吃蛇最高得分记录.txt", "w"); //以只写的方式打开文件
        fwrite(&max, sizeof(int), 1, pf); //将max写入文件（此时max为0），即将最高得分初始化为0
    }
    fseek(pf, 0, SEEK_SET); //使文件指针pf指向文件开头
    fscanf(pf,"%d",&playtime);
    for (i=0;i<playtime;i++)
    {
        XX *temp;
        temp=(XX*)malloc(sizeof(XX));
        temp->next=H->next;
        H->next=temp;
        fscanf(pf,"%ld %d",&temp->a.T,&temp->a.score);
        if(max<temp->a.score)
        {
            max=temp->a.score;
        }
    }
    fclose(pf); //关闭文件
    pf = NULL; //指针及时置空
}

void WScore()//写入最高分
{
    int i;
    FILE* pf = fopen("贪吃蛇最高得分记录.txt", "w"); //以只写的方式打开文件
    if (pf == NULL) //打开文件失败
    {
        printf("保存最高得分记录失败\n");
        exit(0);//正常运行程序并退出程序
    }
    XX *p=H;
    fprintf(pf,"%d\n",playtime);
    for (i=0;i<playtime;i++)
    {
        p=p->next;
        fprintf(pf,"%ld %d\n",p->a.T,p->a.score);
    }
    fclose(pf); //关闭文件
    pf = NULL; //指针及时置空
}

void InitSnake()//初始化蛇
{
    head.len = 2; //蛇的身体长度初始化为2
    head.x = COL / 2; //蛇头位置的横坐标
    head.y = ROW / 2; //蛇头位置的纵坐标
    //蛇身坐标初始化
    body[0].x = COL / 2 - 1;
    body[0].y = ROW / 2;
    body[1].x = COL / 2 - 2;
    body[1].y = ROW / 2;
    //记录蛇头和蛇身位置
    record[head.y][head.x] = HEAD;
    record[body[0].y][body[0].x] = BODY;
    record[body[1].y][body[1].x] = BODY;
}

void FoodRand()//食物随机生成
{
    int i, j;
    do
    {
        i = rand() % ROW;//随机生成食物的横坐标
        j = rand() % COL;//随机生成食物的纵坐标
    } while (record[i][j] != EMPTY); //食物在"无"生成，不在"无"则重新生成
    record[i][j] = FOOD; //记录食物位置
    color(4); //颜色设置
    CursorSet(2 * j, i); //设置光标位置
    printf("●"); //显示食物
}

void ScoreAndEnd(int x, int y)//游戏分数和游戏结束
{
    if (record[head.y + y][head.x + x] == FOOD)//若蛇头即将到达的位置是食物，则得分
    {
        head.len++; //蛇身加长
        score += 10; //更新得分
        color(15); //颜色设置
        CursorSet(0, ROW);//设置光标位置
        printf("当前得分:%d", score); //重新输出当前得分
        FoodRand(); //重新随机生成食物
    }
    else if (record[head.y + y][head.x + x] == WALL || record[head.y + y][head.x + x] == BODY)
        //若蛇头即将到达的位置是墙或者蛇身，则游戏结束
    {
        Sleep(1000); //留给玩家反应时间,休眠函数
        system("cls"); //清空屏幕
        color(15); //颜色设置
        CursorSet(2 * (COL / 3), ROW / 2 - 3);//设置光标位置
        if (score > max)
        {
            printf("新纪录！！！，最高记录更新为%d", score);
            max=score;
        }
        else if (score == max)
        {
            printf("差点打破最高记录了，加油！！！", score);
        }
        else
        {
            printf("加油，当前与最高记录相差%d", max - score);
        }
        saveTandS(H,score);
        WScore();
        CursorSet(2 * (COL / 3), ROW / 2);//设置光标位置
        printf("GAME OVER");
        while (1) //询问玩家是否再来一局(一个死循环)
        {
            char ch;
            CursorSet(2 * (COL / 3), ROW / 2 + 3);
            printf("再来一局?(y/n),或查看游戏记录(r):");
            scanf("%c", &ch);
            if (ch == 'r')
            {
                PrintScore();
            }
            else if (ch == 'y')
            {
                system("cls");
                main();//重新运行一遍主函数
            }
            else if (ch == 'n')
            {
                CursorSet(2 * (COL / 3), ROW / 2 + 5);
                exit(0);//正常运行并退出程序
            }
            else
            {
                CursorSet(2 * (COL / 3), ROW / 2 + 5);
                printf("请重新选择");
            }
            system("cls");
        }
    }
}

void AppearSnake(int flag)//显示蛇
{
    int i;
    if (flag == 1)//显示移动前的蛇
    {
        color(2); //设置颜色
        CursorSet(2 * head.x, head.y);
        printf("●"); //显示蛇头
        for (i = 0; i < head.len; i++)
        {
            color(11);
            CursorSet(2 * body[i].x, body[i].y);
            printf("●"); //显示蛇身
        }
    }
    else //显示移动后的蛇，蛇在移动中头前进则尾巴消失
    {
        if (body[head.len - 1].x != 0)//将蛇尾变为"无"
        {
            CursorSet(2 * body[head.len - 1].x, body[head.len - 1].y);
            printf("  ");
        }
    }
}

void MoveSnake(int x, int y)//移动蛇的函数
{
    int i;
    AppearSnake(0); //显示移动后的蛇
    record[body[head.len - 1].y][body[head.len - 1].x] = EMPTY; //记录蛇移动后蛇尾位置为无
    record[head.y][head.x] = BODY; //记录蛇移动后蛇头位置为蛇身
    for (i = head.len - 1; i > 0; i--)//更新了蛇移动后的坐标
    {
        body[i].x = body[i - 1].x;
        body[i].y = body[i - 1].y;
    }
    body[0].x = head.x;//蛇移动后第一个蛇身纵坐标
    body[0].y = head.y;//蛇移动后第一个蛇身横坐标
    head.x = head.x + x;//蛇移动后蛇头纵坐标
    head.y = head.y + y;//蛇移动后蛇头横坐标
    AppearSnake(1); //显示移动后的蛇
}

void run(int x, int y)//如何执行
{
    int t = 0;
    while (1)//死循环
    {
        if (t == 0)
            t = 6000; //蛇的速度值，值越小，蛇移动速度越快
        while (--t)
        {
            if (kbhit() != 0) //若键盘被敲击，则退出循环
                break;
        }
        if (t == 0) //键盘未被敲击
        {
            ScoreAndEnd(x, y); //游戏得分与游戏结束
            MoveSnake(x, y); //移动蛇
        }
        else //键盘被敲击
        {
            break; //返回Game函数
        }
    }
}

void Game()//游戏逻辑
{
    int n = RIGHT; //开始游戏时，默认向右移动
    int temp = 0; //记录蛇的移动方向
    goto first; //第一次进入循环先向默认方向前进
    while (1)
    {
        n = getch(); //读取键值
        switch (n)//程序执行前，调整读取的按键
        {
            case UP:
            case DOWN: //如果敲击的是“上”或“下”
                if (temp != LEFT&&temp != RIGHT) //并且上一次蛇的移动方向不是“左”或“右”
                {
                    n = temp; //那么下一次蛇的移动方向设置为上一次蛇的移动方向
                }
                break;
            case LEFT:
            case RIGHT: //如果敲击的是“左”或“右”
                if (temp != UP&&temp != DOWN) //并且上一次蛇的移动方向不是“上”或“下”
                {
                    n = temp; //那么下一次蛇的移动方向设置为上一次蛇的移动方向
                }
            case SPACE:
            case ESC:
            case 'r'://刷新
            case 'R'://刷新
                break;
            default:
                n = temp; //其他键无效，默认为上一次蛇移动的方向
                break;
        }
        first: //第一次进入循环先向默认方向前进
        switch (n)
        {
            case UP: //方向键：上
                run(0, -1); //向上移动
                temp = UP; //记录当前蛇的移动方向
                break;
            case DOWN: //方向键：下
                run(0, 1); //向下移动
                temp = DOWN; //记录当前蛇的移动方向
                break;
            case LEFT: //方向键：左
                run(-1, 0); //向左移动
                temp = LEFT; //记录当前蛇的移动方向
                break;
            case RIGHT: //方向键：右
                run(1, 0); //向右移动
                temp = RIGHT; //记录当前蛇的移动方向
                break;
            case SPACE: //暂停
                system("pause>nul"); //暂停后按任意键继续
                break;
            case ESC: //退出
                system("cls"); //清空屏幕
                color(15); //颜色设置
                CursorSet(COL - 8, ROW / 2);
                printf("  游戏结束  ");
                CursorSet(COL - 8, ROW / 2 + 2);
                exit(0);
            case 'r'://刷新
            case 'R'://刷新
                system("cls"); //清空屏幕
                main(); //重新运行一遍主函数
        }
    }
}

