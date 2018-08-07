#ifndef SYSHEAD_H
#define SYSHEAD_H
#include <QString>

/*  modified 2012-05-11 */
//用于存放最近一次的线路信息
typedef struct __lineInfoSelect
{
    int    nLineIndex;
    int    nDirection;//上行0；下行1
    int    nStationIndex;
    int    nPoleIndex;
    void inline clear(void )
    {
        nLineIndex=0;
        nDirection=0;
        nStationIndex=0;
        nPoleIndex=0;
    }
}LineInfoSelect;

typedef struct __onePole
{
    QString sPoleId;
    QString sTunnel;
    int nPoleIndex;
}onePole;
typedef struct __stationSubPole
{
    QString sStation;
//    QVector<QString> sPoleID;
    QVector<onePole> onepole;
}stationSubPole;
enum CLOUDCONTROL
{
//    tilt_UP=21,
//    tilt_DOWN=22,
//    pan_LEFT=23,
//    pan_RIGHT=24,
//    up_LEFT=25,
//    up_RIGHT=26,
//    down_LEFT=27,
//    down_RIGHT=28,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    RIGHTUP,
    RIGHTDOWN,
    LEFTUP,
    LEFTDOWN,
    STOP,
    RESET
};
enum VIDEOTYPE
{
    VIDEORIGHT=1,
    VIDEOMID=2,
    VIDEOLEFT=4

};
typedef struct
{
    int id;
    QString station;            //站区名称
    QString tunnel;             //隧道名称
    QString poleID;             //支柱号
    float poleDist;             //跨距
    int structFlag;             //结构标志
    int curveFlag;              //取消标志
    QString distFlag;           //公里标
    QString powerSupply;        //供电段
    QString factory;            //车间名称
    QString workArea;           //工区
    int wireHight;              //导高标准值
    int offset;                 //拉出值标准值
}oneLineData_t;

//zbw:2012-09
struct MeasureDebugData
{
    //1id 2时间 3 里程4轨距      5超高       6 拉出值   7 导高      8 X倾角  9Y倾角
    //id  tm   nWalkDistance  railwidth railheight wirewidth wireheigth fXangle structureHeight
    //11-13
    // 0 ID 1时间    2里程   3公里标  4站区      5杆号      6轨距        7超高
    //"id   tm      dist    km      station  PoleID     railWidth   railHeight
    //8拉出值    9导高      10拉出值1     11导高1       12平行线间距     13垂直距离       14承力索      15结构高度       16 500高差   17数据类型
    //wireWidth   wireHeight wireWidth1   wireHeight1,parallelDist    verticalDist    carrier     structureHeight wire500     dataType");
    int id;                 //1     id
    QString tm;             //2     时间
    double nWalkDistance;   //3     里程
    float dKm;              //4     公里标
    QString station;        //5     站区
    QString poleID;         //6     杆号
    int nRailWidth;         //7     轨距
    int nRailHeight;        //8     超高
    QPoint wire1;           //9，10 QPoint(拉出值,导高)
    QPoint wire2;           //11，12 QPoint(拉出值,导高)
    int parallelDist;       //13    平行距离
    int verticalDist;       //14    垂直距离
    int carrier;            //15    承力索
    float structureHeight;  //16    Y倾角,05-27:结构高度
    int wire500;            //17    500高差
    //    int nSide;              //18    侧面界限
    QString dataType;       //19    数据类型
    float baseAngle;
    bool bDropper;
    bool bMidPole;
    bool wire2Valid;

    MeasureDebugData()
    {
        carrier=0;
        baseAngle=0;
        bDropper=false;
        bMidPole=false;
        wire2Valid=false;
        //        nSide=0;
        wire500=0;
        structureHeight=0;
        parallelDist=0;
        verticalDist=0;
        nWalkDistance=0;
        nRailHeight=0;
        nRailWidth=0;
        tm="";
    }
};
typedef struct
{
    int     nRailWidth;     //7 轨距
    int     nRailHeight;    //8 超高
    float sideWidth;        //9 侧面界限
    float sideHeight;       //10 红线
    float structHeight;     //11 结构高度
    float dWireWidth;       //12 定拉
    float dWireHeight;      //13 定高
    float slope;            //14 坡度
    float  gaocha;          //15 高差
    float midWireHeight;    //16 中高
    float midWireWidth;     //17 中拉
    float wireWidth1;       //18 非支拉出值
    float wireHeight1;      //19 非支导高1
    float wiresWidth;       //20 水平距离
    float wiresHeight;      //21 垂直距离
    float wire500Height;    //22 500高差
}OnePolePar;
/* 2012-05-14 */    //id,时间，里程，站区，杆号，最大导高，最小导高，高差，最大拉出值，定位器描述
//(id INT, tm varchar(30),nWalkDistance float,station varchar(30),poleid varchar(10),maxDaogao float, minDaogao float, gaocha float, maxLachuzhi float,LocationRemark varchar(100))

struct MeasureIdEx_t
{
    int id;                 //1 ID
    QString tm;             //2 时间
    float nWalkDistance;             //3 里程
    float dKm;              //4 公里标
    QString station;        //5 站区
    QString poleid;         //6 杆号

    OnePolePar  onePolePar;
    QString locationRemark; //23 定位器描述
    QByteArray PicPath;     //24 图片存放位置
    //    float  maxdaogao;       //6 最大导高
    //    float  mindaogao;       //7 最小导高
    //    float  maxLachuzhi;     //9 拉出值
    int sideLimit;     //10 侧面界限

    //ID, 2 时间, 3 里程(米), 4公里标(千米),  5站区,   6杆号,  7轨距,       8超高,
    //id, tm,    nWalkDistance       ,km ,         station, poleid,railwidth ,railheight
    //9侧面界限,  10 红线 11 结构高度   12定拉       13定高,       14坡度,      高差,    16中高,
    //sideWidth,Height,structHeight ,dWireWidth ,dWireHeight ,slope float,gaocha, midWireHeight,
    //17中拉,       18非支拉出值,19非支导高1, 20 水平距离,21垂直距离,   22 500高差,      23 定位器描述,   24 巡视图片;
    //midWireWidht,wirewidth1,wireheight1,wiresWidth,wiresHeight,wire500Height,LocationRemark,picPath");

    QString railwayName;
    QString direction;
    MeasureIdEx_t()
    {
        sideLimit=0;
    }
};
typedef struct
{
    QString poleindex;
    QString station;
}tableStruct_t;

typedef enum
{
    DISCONNECTED,
    CONNECTED,
    CONNECTING,
    GO,
    BACK
}DEVICE_CONNECT_STATUS;
typedef enum
{
    OPEN_LED=0x01,
    CLOSE_LED=0x02,
    OPEN_DATA=0x03,
    CLOSE_DATA=0x04,
    CLEAR_WALK=0x05,
    OPEN_RADAR=0X06,
    CLOSE_RADAR=0X07,
    OPEN_WIFI=0x08,
    CLOSE_WIFI=0x09
}COM_COMMAND_TYPE;
typedef enum
{
    LED_OPENED=0x01,
    LED_CLOSED=0x02,
    DATA_OPENED=0x03,
    DATA_CLOSED=0x04,
    WALK_CLEARED=0x05,
    RADAR_OPENED=0X06,
    RADAR_CLOSED=0X07,
    WIFI_OPENED=0x08,
    WIFI_CLOSED=0x09
}COM_REPLAY_TYPE;

struct deviceInfo
{
    QString GPSJ;
    QString GPSW;
    QString wAndS;
    QString dianliang;
};
#define PI 3.1415926
#define MAXABS_DISTANCE 50000
#define MAX_DISTANCE 65535
#define ZHENG_RADAR_COM 1
#define ZHENG_RAIL_COM 0
#define LIU_RAIL_COM 1
//typedef enum
//{
//    WIRE_WIDTH=0x01,//温度
//    WIRE_HEIGHT=0x02,//湿度
//    RAIL_WIDTH=0x04,//电压，电量
//    RAIL_HEIGHT=0x08,//
//    NO_VERIFY=0x00
//}PAR_VERIFY;
//#define VERIFY_WIREWIDTH 0x01

class RunProfile
{
public:
    RunProfile(){position = 0.0f;};
    QString lineName;       //线路名称
    int     lineIdx;     //线路索引
    QString tableName;   //上行、下行表名

    QString station;    //站区
    int     prevStationIdx;

    QString poleStr;   //支柱号
    int    prevPole;
    float position;   //位置调整
    int direction;    //方向 0，上行，1,下行
    int nPoleTableID;
    QString dbpath;

    //运行过程中参数
    double m_tinyspacer;
    int m_currIdx;
    double m_disapperSpacer;
    double m_startDist;
    double startKm;
    double currentKm;
    bool bDoPoleInc;//行走过程中实现杆号递增,true,行走过程中杆号递增,false,行走过程中杆号递减;
    bool bTablePoleInc;//表格中杆号递增,true,杆号递增,false,杆号递减
    unsigned int nPoleSum; //表示基础数据中杆号是否为递增
};
struct wireStandardValues
{
    int nVerifyWireHeight;
    int nVerifyWireWidth;
    double nVerifyRailHeight;
    double nVerifyRailWidth;
    int nVerifySide;
    QString strVerifyType;//true,导高及拉出值的biaoding;false,侧面界限的标定
    //1217,side,标定侧面界限；rail，标定轨距及超高；wire，标定拉出值及导高
     wireStandardValues()
     {
         strVerifyType="";
     }
};
struct sensorInitialValues
{
    int nInitialWireHeight;
    int nInitialWireWidth;
    int nInitialRailHeight;
    int nInitialRailWidth;
    int nSide;
};
typedef enum
{
    RADOR_STATUS,       //雷达状态指示
    POLE_STATUS,        //支柱状态指示
    DIRECTION_STATUS    //行车方向指示
}STATUS_TYPE;
enum curveType
{
    curveColor=1,
    backgroundColor=2,
    axisColor=3,
    Alarm1=4,
    Alarm2=5,
    Dropper=6,
    Carrier=7,
    DisplayStyle=8,
    pole=9,
    wire2=10
};
#endif // SYSHEAD_H
