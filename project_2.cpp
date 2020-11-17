#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <stack>
#include <algorithm>
#include <limits>

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

enum PathFindingState
{
	START,
	TARGET,
	// return shortest path
	PATH,
	WALL,
	// done picking and not target/start
	DONE,
	// current picking box
	NOW,
	BLANK
};


//chech the cell
class Cell
{
    public:
    CellType state;
	Coordinate _position;
    // path finding
    PathFindingState _pathState;
    Cell* parent;
    int distance;
	bool isPicked;
};


class FloorMap
{
public:
    FloorMap(int row=0, int col=0, int battery = 0);
    Cell **floor;
    int trow, tcol;
    Cell* batteryStationCell;
    void printFloor();
};

FloorMap::FloorMap(int row, int col, int battery)
{
    //remaining = new std::list <Coordinate>();
    trow = row;
    tcol = col;
    floor = new Cell*[trow];
    for(int i=0; i<trow; i++)
    {
        floor[i] = new Cell[tcol];
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

class Robot
{
public:
    Robot(int battery=0);
    int maxBattery, curBattery;
    Coordinate currentPosition;
    vector<Coordinate>StepHistory;
    list<Cell*> remainingCells;
    void SpiralMove(FloorMap* floor); //移動
    void MoveToTarget(Cell* c, FloorMap* floor); //特定的位子
    bool NoBattery();
    void StepRecord(); //記錄走到的位子

    // path finding
    // to check and run box with current distance
	// manually initialize and deinitialize
	static int currentDistance;
	// run the algorithm based on one box instead of whole board
	Cell* currentCell;
	// use it to check all the available boxes' distance
	std::list<Cell*> children;
    // function to point currentBox to null
	void ResetCurrentBox();
    // restart find path algorithmn if target is found
    void RestartFindPath(FloorMap* map);
    void SetSurroundingCell(FloorMap* map);
    void FindPath(Cell* c, FloorMap* fm);

    stack<Cell*> path; //路径

    bool foundPath;

    void CleanUpRemaining();
};

int Robot::currentDistance;

Robot::Robot(int battery)
{
    maxBattery = battery;
    curBattery = battery;
}

void Robot::SpiralMove(FloorMap* f)
{
    //check currentPosition's left cell's state
    if(currentPosition.y-1>=0 && f->floor[currentPosition.x][currentPosition.y-1].state == dirty)
    {
        f->floor[currentPosition.x][currentPosition.y-1].state = clean;  //claer
        currentPosition.y = currentPosition.y-1;    //update new position
        StepRecord(); //record step
    }
    //check currentPosition's upper cell's state
    else if(currentPosition.x-1 >=0 && f->floor[currentPosition.x-1][currentPosition.y].state == dirty)
    {
        f->floor[currentPosition.x-1][currentPosition.y].state = clean;  //claer
        currentPosition.x = currentPosition.x-1;    //update new position
        StepRecord(); //record step
    }
    //check currentPosition's right cell's state
    else if(currentPosition.y+1 < f->tcol && f->floor[currentPosition.x][currentPosition.y+1].state == dirty)
    {
        f->floor[currentPosition.x][currentPosition.y+1].state = clean;  //claer
        currentPosition.y = currentPosition.y+1;    //update new position
        StepRecord(); //record step
    }
    //check currentPosition's bottom cell's state
    else if(currentPosition.x+1 < f->trow && f->floor[currentPosition.x+1][currentPosition.y].state == dirty)
    {
        f->floor[currentPosition.x+1][currentPosition.y].state = clean;  //claer
        currentPosition.x = currentPosition.x+1;    //update new position
        StepRecord(); //record step
    }
    else //四周都清理过了，但地图某处还肮脏
    {
        if(remainingCells.empty() == false)
        {
            Cell* newCell = remainingCells.front(); //取得list的第一Cell
            remainingCells.pop_front(); //清除得到的Cell
            MoveToTarget(&f->floor[newCell->_position.x][newCell->_position.y], f); // pass in得到的cell节点
        }
    }
    CleanUpRemaining();
}

void Robot::StepRecord()
{
    curBattery -= 1;
    StepHistory.push_back(currentPosition);
   // cout << "(" << StepHistory.back().x << "," << StepHistory.back().y << ")" <<endl;
}

bool Robot::NoBattery()
{
    if(curBattery == maxBattery /2 )
	{
		return true;
	}
	return false;
}

void Robot::ResetCurrentBox()
{
    currentCell = NULL;
	currentDistance = 0;
	children.clear();
}

void Robot::RestartFindPath(FloorMap* map)
{
    for(int i=0; i<map->trow; i++)
	{
		for(int j=0; j<map->tcol; j++)
		{
            if(map->floor[i][j].state == wall)
            {
                map->floor[i][j]._pathState = WALL;
            }
            else
            {
                map->floor[i][j]._pathState = BLANK;
            }

            map->floor[i][j].distance = INT_MAX;
            map->floor[i][j].parent = NULL;
			map->floor[i][j].isPicked = false;
		}
	}

    foundPath = false;

	ResetCurrentBox();
}

// 把周围的cell加入openList
void Robot::SetSurroundingCell(FloorMap* map)
{
    // use own distance + 1
	int nextdistance = currentCell->distance + 1;
    // check if the next cell to set up is at the edge
    // check if the next cell is a blank cell
    // check if the next cell distance is INT_MAX
    if(currentCell->_position.x + 1 < map->trow && (map->floor[currentCell->_position.x + 1][currentCell->_position.y]._pathState == BLANK || map->floor[currentCell->_position.x + 1][currentCell->_position.y]._pathState == TARGET ) && map->floor[currentCell->_position.x + 1][currentCell->_position.y].distance == INT_MAX)
    {
        // set its distance to currentDistance + 1
        map->floor[currentCell->_position.x+1][currentCell->_position.y].distance = nextdistance;
        // set its parent
        map->floor[currentCell->_position.x+1][currentCell->_position.y].parent = currentCell;
        // add to children
        children.push_back(&map->floor[currentCell->_position.x+1][currentCell->_position.y]);

    }


    if(currentCell->_position.x - 1 >= 0 && (map->floor[currentCell->_position.x - 1 ][currentCell->_position.y]._pathState == BLANK || map->floor[currentCell->_position.x - 1 ][currentCell->_position.y]._pathState == TARGET ) && map->floor[currentCell->_position.x - 1 ][currentCell->_position.y].distance == INT_MAX)
    {
        // set its distance to currentDistance + 1
        map->floor[currentCell->_position.x-1][currentCell->_position.y].distance = nextdistance;
        // set its parent
        map->floor[currentCell->_position.x-1][currentCell->_position.y].parent = currentCell;
        // add to children
        children.push_back(&map->floor[currentCell->_position.x-1][currentCell->_position.y]);

    }

    if(currentCell->_position.y + 1 < map->tcol &&
    (map->floor[currentCell->_position.x][currentCell->_position.y + 1 ]._pathState == BLANK ||
    map->floor[currentCell->_position.x][currentCell->_position.y + 1 ]._pathState == TARGET ) &&
    map->floor[currentCell->_position.x][currentCell->_position.y + 1 ].distance == INT_MAX)
    {
        // set its distance to currentDistance + 1
        map->floor[currentCell->_position.x][currentCell->_position.y+1].distance = nextdistance;
        // set its parent
        map->floor[currentCell->_position.x][currentCell->_position.y+1].parent = currentCell;
        // add to children
        children.push_back(&map->floor[currentCell->_position.x][currentCell->_position.y+1]);

    }

    if(currentCell->_position.y - 1 >= 0 &&
    (map->floor[currentCell->_position.x][currentCell->_position.y - 1 ]._pathState == BLANK ||
    map->floor[currentCell->_position.x][currentCell->_position.y - 1 ]._pathState == TARGET ) &&
    map->floor[currentCell->_position.x][currentCell->_position.y - 1 ].distance == INT_MAX)
    {
        // set its distance to currentDistance + 1
        map->floor[currentCell->_position.x][currentCell->_position.y-1].distance = nextdistance;
        // set its parent
        map->floor[currentCell->_position.x][currentCell->_position.y-1].parent = currentCell;
        // add to children
        children.push_back(&map->floor[currentCell->_position.x][currentCell->_position.y-1]);

    }
}

void Robot::FindPath(Cell* c, FloorMap* fm)
{
    // 设置终点
    fm->floor[c->_position.x][c->_position.y]._pathState = TARGET;

    // start point, when there is no children
    if (currentCell == NULL)
    {
        currentCell = &fm->floor[currentPosition.x][currentPosition.y];
        currentCell->_pathState = START;
        currentCell->distance = 0;
        SetSurroundingCell(fm);
        // set current distance to 1 manually
        currentDistance = 1;
    }
    // run normal current distance after diagonal
    for (std::list<Cell*>::iterator i = children.begin(); i != children.end(); i++)
    {

        if ((*i)->distance == currentDistance && (*i)->isPicked == false)
        {
            // pick the children as current cell
            currentCell = (*i);

            // change its isPicked to true
            currentCell->isPicked = true;
        //    cout << "Current Path State: " << currentCell->_pathState<<endl;

            // check if it is target or blank, if target, end algorithm and change all parents (exclude START) state to PATH
            if (currentCell->_pathState == TARGET)
            {
         //       cout << "Ello" << endl;
                // do...while (currentCell->state != currentPosition)
                do
                {
                    path.push(currentCell);
                    // set current box as the parent
                    currentCell = currentCell->parent;
                    // set currentBox state to PATH, if it is not START
                    if (currentCell->_pathState != START)
                    {
                        currentCell->_pathState = PATH;
                    }
                } while (currentCell->_pathState != START);
                // end algorithm

                // stop finding path
                foundPath = true;
                return;
            }
            // if blank
            else if (currentCell->_pathState == BLANK)
            {
                // set the children surrounding cell
                SetSurroundingCell(fm);
                // set its state as DONE
                currentCell->_pathState = DONE;
            }
        }
    }

    // erase those that is already picked and push into children list
    for (std::list<Cell*>::iterator l = children.begin(); l != children.end(); l++)
    {
        if ((*l)->distance < currentDistance && (*l)->isPicked == true)
        {

            // erase l from children
            l = children.erase(l);
        }
    }

    // after done with all children (matches with current distance)
    // increase current distance by one
    // increment = 1
    currentDistance += 1;

}

void Robot::MoveToTarget(Cell* c, FloorMap* fm)
{
    while(!path.empty())
    {
        path.pop();//清空佇列
    }

    RestartFindPath(fm);

    foundPath = false;

    do
    {
        FindPath(c, fm);

    } while (!foundPath);
    //path储存着从现在地到目的地的coordinates
    while(!path.empty()) //当q还没被清空，继续行走
    {
        if(NoBattery() && c->state != charger) //行动前要确认还有电量，没电池就去充电
        {
            //其余的也不走了
            return;
        }

        Cell* nextCell = path.top();//从path取出第一节点
        path.pop(); //清理
        fm->floor[nextCell->_position.x][nextCell->_position.y].state = clean;  //还没清理的话，就顺便清理了
        currentPosition = nextCell->_position; //update new position, 现在开始向目的地行走了
        StepRecord(); //record step，记录节点，之后output
        //如果在remaining list内，顺便清理了也就要删除
        CleanUpRemaining();
    }
    //到达目的地
}

void Robot::CleanUpRemaining()
{
    list<Cell*>::iterator itr=remainingCells.begin();
    list<Cell*>temporary;
    while(itr!=remainingCells.end())
    {
        if((*itr)->state == dirty)
        {
            temporary.push_back(*itr);
        }
        itr++;
    }
    remainingCells=temporary;
}

int main()
{

    int trow, tcol, battery;
    ifstream inFile("test.txt");
    ofstream outFile;

    inFile.is_open();
    inFile >> trow >> tcol >> battery;
    FloorMap* m = new FloorMap(trow, tcol, battery);
    Robot* r = new Robot(battery);
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
                //m->remaining.push_back(&m->floor[i][j]); //push remaining to Coordinate
                m->floor[i][j].state = dirty;
                r->remainingCells.push_back(&m->floor[i][j]);

            }
            else if(temp == '1')
            {
                m->floor[i][j].state = wall;
            }
            else if(temp == 'R')
            {
                Coordinate c;
                c.x = i;
                c.y = j;
                r->currentPosition = c;
                r->StepHistory.push_back(c);
              //  cout <<r->currentPosition.x << "," << r->currentPosition.y << endl;
                m->floor[i][j].state = charger;
                m->batteryStationCell = &m->floor[i][j];

            }

            m->floor[i][j]._position = c; // let cell hold its own coordinate
        }
    }
    inFile.close();

    outFile.open("test.path");
    while(r->StepHistory.size()>0)
    {
        cout << "(" << r->StepHistory.back().x << "," << r->StepHistory.back().y << ")" << endl;
        r->StepHistory.pop_back();
    }

    while(r->remainingCells.size() > 0)
    {
       // cout << r->remainingCells.size() << endl;
        if(r->NoBattery()) //没电池就去充电
        {
            r->MoveToTarget(m->batteryStationCell, m);
        }
        else
        {
            //从外围慢慢进入中心
            r->SpiralMove(m);
        }
    }

    if(r->currentPosition.x != m->batteryStationCell->_position.x && r->currentPosition.y != m->batteryStationCell->_position.y)
    {
        r->MoveToTarget(m->batteryStationCell, m);
    }

    // cout << r.StepHistory.size() << endl;
    m->printFloor();
    cout << r->StepHistory.size() << endl;
    while(r->StepHistory.size()>0)
    {
        cout << "(" << r->StepHistory.back().x << "," << r->StepHistory.back().y << ")" << endl;
        r->StepHistory.pop_back();
    }
    // release pointers
    delete r;
    delete m;

    return 0;
}
