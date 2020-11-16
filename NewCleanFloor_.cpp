#include <iostream>
#include <fstream>
#define Q_SIZE 1009
#include <list>
#include <vector>
#include <queue>
#include <algorithm>
#include <stack>

using namespace std;

class Coordinate
{
public:
    int x;
    int y;

};

//set the celltype for every cell
enum CellType
{
    dirty = 0,
    clean,
    wall,
    charger
};

//chech the cell
class Cell
{
    public:
    CellType state;
	Coordinate _position;
};


class FloorMap
{
public:
    FloorMap(int row=0, int col=0, int battery = 0);
    Cell **floor;
    int trow, tcol;
    Cell batteryStationCell;
    void printFloor();
    std::list<Coordinate*> remaining;
    void RemoveFromRemaining(Coordinate* c);

};

FloorMap::FloorMap(int row, int col, int battery)
{
    //remaining = new std::list <Coordinate>();
    trow = row;
    tcol = col;
    floor = new Cell*[trow];
    for(int i=0; i<trow; i++)
    {
        floor[i] = new Cell [tcol];
    }
    for(int i=0; i<trow; i++)
    {
        for(int j=0; j<tcol; j++)
        {
            floor[i][j];
        }
    }
}

void FloorMap::printFloor()
{
    for(int i=0;i<trow;i++)
    {
        for(int j=0;j<tcol;j++)
        {
            cout << floor[i][j].state << " ";
        }
        cout << endl;
    }
}

void FloorMap::RemoveFromRemaining(Coordinate* c)
{
	// 检查，看c有没有记录在remaining list内
	bool found = (std::find(remaining.begin(), remaining.end(), c) !=remaining.end());
	// 如果存在，就删除
	if(found)
	{
		remaining.remove(c);
	}

}
class Robot
{
public:
    Robot(int battery=0);
    int maxBattery, curBattery;
    Coordinate currentPosition;
    vector<Coordinate>StepHistory;
    void SpiralMove(FloorMap &floor); //移動
    void MoveToTarget(Cell c, FloorMap &floor); //特定的位子
    bool NoBattery();
    void StepRecord(); //記錄走到的位子
};

Robot::Robot(int battery)
{
    maxBattery = battery;
    curBattery = battery;
}

void Robot::SpiralMove(FloorMap &f)
{
    //check currentPosition's left cell's state
    if(f.floor[currentPosition.x][currentPosition.y-1].state == dirty)
    {
        f.floor[currentPosition.x][currentPosition.y-1].state = clean;  //claer
        currentPosition.y = currentPosition.y-1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's upper cell's state
    else if(f.floor[currentPosition.x-1][currentPosition.y].state == dirty)
    {
        f.floor[currentPosition.x-1][currentPosition.y].state = clean;  //claer
        currentPosition.x = currentPosition.x-1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's right cell's state
    else if(f.floor[currentPosition.x][currentPosition.y+1].state == dirty)
    {
        f.floor[currentPosition.x][currentPosition.y+1].state = clean;  //claer
        currentPosition.y = currentPosition.y+1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's bottom cell's state
    else if(f.floor[currentPosition.x+1][currentPosition.y].state == dirty)
    {
        f.floor[currentPosition.x+1][currentPosition.y].state = clean;  //claer
        currentPosition.x = currentPosition.x+1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    else //四周都清理过了，但地图某处还肮脏
    {
        if(f.remaining.empty() == false)
        {
            Coordinate* newPos= f.remaining.front(); //get current first coordinate
            f.remaining.pop_front(); //remove
            MoveToTarget(f.floor[newPos->x][newPos->y], f);
        }
    }
}

void Robot::StepRecord()
{
    curBattery -= 1;
    StepHistory.push_back(currentPosition);
}

bool Robot::NoBattery()
{
    if(curBattery == maxBattery /2 )
	{
		return true;
	}
	return false;
}

queue<Coordinate> q;
int next[4][2] = {{-1,0},{0,1},{1,0},{0,-1}};

void Robot::MoveToTarget(Cell c, FloorMap &fm)
{
    int cacheX = fm.trow;
    int cacheY = fm.tcol;
    //cout << cacheX << endl;
	Coordinate parent[cacheX][cacheY];//parent後面用於儲存父節點
    stack<Coordinate> path; //路徑

	// 初始化，清空父节点
	for(int i=0; i<fm.trow; i++)
	{
		for(int j=0; j<fm.tcol; j++)
		{
			parent[i][j].x = -1; // -1 等于还未走过
			parent[i][j].y = -1;
		}
	}

    while(!q.empty())
	{
		q.pop();//清空佇列
	}

	while(!path.empty())
    {
        path.pop();
    }

    Coordinate start;
    start.x = currentPosition.x, start.y = currentPosition.y;

    q.push(start);//加入佇列的首位

	while(!(q.front().x== c._position.x && q.front().y==c._position.x))//條件為搜查到終點，q最新的element不是目的地，就继续
	{
		if(NoBattery() && c.state!=charger) //行动前要确认还有电量，没电池就去充电
		{
			//其余的也不走了
			return;
		}
		Coordinate from = q.front();
		q.pop();//把佇列首位賦值給from再刪除

		for(int i=0; i<4; i++)
		{
			Coordinate to;//to為下一步

			//扫地机器人只能走下，右，上，左
			to.x = from.x + next[i][0];
			to.y = from.y + next[i][1];//走下一步賦值

			if(to.x<0 && to.x>=fm.trow && to.y<0 && to.y>=fm.tcol)
			{
				continue;//如果越界就不走下一步，不執行
			}

			if(fm.floor[to.x][to.y].state == wall && parent[to.x][to.y].x != -1)
			{
				continue;//如果下一步為障礙或者下一步之前走過，不執行
			}

			q.push(to);//把to加入佇列

			parent[to.x][to.y].x = from.x;
			parent[to.x][to.y].y = from.y;//把to節點的父節點from的座標賦值給pre
		}
	}
	cout << "Ello" << endl;
	Coordinate from = parent[c._position.x][c._position.y];
	path.push(from);//記錄終點

	//到達目的地，現在追溯回到起點
	while(!(path.top().x==start.x && path.top().y==start.x))
	{
	    Coordinate to; //to為下一步
	    to.x = parent[from.x][from.y].x;
	    to.y = parent[from.x][from.y].y;

	    path.push(to);
	    from = to;
	}

	path.pop(); //移除現在的位置

    //q储存着从现在地到目的地的coordinates
	while(!path.empty()) //当q还没被清空，继续行走
	{
		if(NoBattery() && c.state!=charger) //行动前要确认还有电量，没电池就去充电，目標是充電站就無視這
		{
			//其余的也不走了
			return;
		}

		Coordinate pos = path.top();//从q取出第一节点
		path.pop(); //清理
		fm.floor[pos.x][pos.y].state == clean;  //还没清理的话，就顺便清理了
        currentPosition = pos; //update new position, 现在开始向目的地行走了
        StepRecord(); //record step，记录节点，之后output
        cout << "hi2" << endl;
		//如果在remaining list内，顺便清理了也就要删除
        fm.RemoveFromRemaining(&currentPosition);
	}
	//到达目的地
}

int main()
{

    int trow, tcol, battery;
    ifstream inFile("test.txt");
    ofstream outFile;

    inFile.is_open();
        inFile >> trow >> tcol >> battery;
        FloorMap m(trow, tcol, battery);
        Robot r(battery);
        char temp;
        for(int i=0; i<trow; i++)
        {
            for(int j=0; j<tcol; j++)
            {
                inFile >> temp;

				Coordinate c;
				c.x = i;
				c.y = j;
                if(temp == '0')
                {
                    m.remaining.push_back(&c); //push remaining to Coordinate
                    m.floor[i][j].state = dirty;

                }
                else if(temp == '1')
                {
                    m.floor[i][j].state = wall;
                }
                else if(temp == 'R')
                {
                    Coordinate c;
                    c.x = i;
                    c.y = j;
                    r.currentPosition = c;
                    m.floor[i][j].state = charger;
                }

				m.floor[i][j]._position = c; // let cell hold its own coordinate
            }
        }
        cout << m.remaining.size() << endl;
        int counter = 0;
        while(counter < 14)
        {
            if(r.NoBattery()) //没电池就去充电
            {
                cout << "Move Target" << endl;
                r.MoveToTarget(m.batteryStationCell,m);
            }
            else
            {
                cout << "Move Spiral" << endl;
                //从外围慢慢进入中心
                r.SpiralMove(m);
            }
            counter ++;
        }
        m.printFloor();
    return 0;
}
