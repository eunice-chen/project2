#include <iostream>
#include <fstream>
#define Q_SIZE 1000009
#include <list>
#include <vector>

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
    //bool isDirty;
    //bool isBump;
};

class FloorMap
{
public:
    FloorMap(int row=0, int col=0, int battery = 0);
    Cell **floor;
    int trow, tcol;
    Coordinate batteryStationCoordinate;
    void printFloor();
    std::list<Coordinate*> remaining;
    void RemoveFromRemaining(Coordinate* c);
    void bfs();
};

FloorMap::FloorMap(int row, int col, int battery)
{
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
    remaining.remove(c);
}

void

class Robot
{
public:
    Robot(int battery=0);
    int maxBattery, curBattery;
    Coordinate currentPosition;
    vector<Coordinate>StepHistory;
    void SpiralMove(FloorMap &floor); //移動
    void MoveToTarget(Coordinate c); //特定的位子
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
        f.floor[currentPosition.x][currentPosition.y-1].state == clean;  //claer
        currentPosition.y = currentPosition.y-1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's upper cell's state
    else if(f.floor[currentPosition.x-1][currentPosition.y].state == dirty)
    {
        f.floor[currentPosition.x-1][currentPosition.y].state == clean;  //claer
        currentPosition.x = currentPosition.x-1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's right cell's state
    else if(f.floor[currentPosition.x][currentPosition.y+1].state == dirty)
    {
        f.floor[currentPosition.x][currentPosition.y+1].state == clean;  //claer
        currentPosition.y = currentPosition.y+1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    //check currentPosition's bottom cell's state
    else if(f.floor[currentPosition.x+1][currentPosition.y].state == dirty)
    {
        f.floor[currentPosition.x+1][currentPosition.y].state == clean;  //claer
        currentPosition.x = currentPosition.x+1;    //update new position
        StepRecord(); //record step
        f.RemoveFromRemaining(&currentPosition);
    }
    /*else
    {
        if(f.remaining.empty() == false)
        {
            MoveToTarget();
        }
    }*/
}

bool Robot::NoBattery()
{
    if(curBattery == maxBattery/2)
    {
        return true;
    }
    return false;
}

//push the coordinate to stepHistory
void Robot::StepRecord()
{
    StepHistory.push_back(currentPosition);
}

int main()
{

    int trow, tcol, battery;
    ifstream inFile("floor3.data");
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
                if(temp == '0')
                {
                    Coordinate c;
                    c.x = i;
                    c.y = j;
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
            }
        }
        /*while(m.remaining.size()>0)
        {
            if(r.NoBattery())
            {
                r.MoveToTarget(m.batteryStationCoordinate)
            }
            else
            {
                if(r.canMoveSpiral)
                {
                    r.SpiralMove();
                }
                else
                {
                    r.MoveToTarget()
                }
            }
        }*/
        m.printFloor();
    return 0;
}
