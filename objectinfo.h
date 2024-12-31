#pragma execution_character_set("utf-8")
#ifndef OBJECTINFO_H
#define OBJECTINFO_H
#include <QString>
#include <QDebug>

class User
{
public:
    User(){
        name = nullptr;
        pwd = nullptr;
        role = nullptr;
        is_used = nullptr;
    }
    friend QDebug& operator<<(QDebug out, const User& info)
    {
        out << "账号：" << info.name << "密码：" << info.pwd << "角色：" << info.role << "是否可用：" << info.is_used;
        return out;
    }
public:
    QString name;
    QString pwd;
    QString role;
    QString is_used;

};

class ParametersInfo
{
public:
    ParametersInfo()
    {
        fileName = nullptr;
        typeNum = nullptr;
        unitNum = 0;
        status = nullptr;
        date = nullptr;
        detectContentOne = nullptr;
        detectObjOne = nullptr;
        detectContentTwo = nullptr;
        detectObjTwo = nullptr;


    }

public:
    QString fileName;
    QString typeNum;
    int unitNum;
    QString status;
    QString date;
    QString detectContentOne;
    QString detectObjOne;
    QString detectContentTwo;
    QString detectObjTwo;
    int samplingPointNum;

};

class SensorDetectDataOne
{
public:
    SensorDetectDataOne()
    {
        typeNum = nullptr;
        pointsNum = 0;
        angle = 0;
        detectData = 0;
        date = nullptr;
    }

public:
    QString typeNum;
    int pointsNum;
    double angle;
    double detectData;
    QString date;

};

class SensorDetectDataTwo
{
public:
    SensorDetectDataTwo()
    {
        typeNum = nullptr;
        pointsNum = 0;
        angle = 0;
        detectData = 0;
        date = nullptr;
    }

public:
    QString typeNum;
    int pointsNum;
    double angle;
    double detectData;
    QString date;

};

class AxialDataEvaluate
{
public:
    AxialDataEvaluate()
    {
        typeNum = nullptr;
        status = nullptr;
        detectContent = nullptr;
        dates = nullptr;
        detectObj = nullptr;
        unitNum = 0;
        maxValue,maxAngle,minValue,minAngle,beatValue,beatAngle,verticalValue,verticalAngle = 0;
        concentricity,concentriticAngle,eccentricity,eccentricAngle,flatness,roundness = 0;
    }
public:

    QString typeNum,status,detectContent,dates,detectObj;
    int unitNum;
    double maxValue,maxAngle,minValue,minAngle,beatValue,beatAngle,verticalValue,verticalAngle;
    double concentricity,concentriticAngle,eccentricity,eccentricAngle,flatness,roundness;
    QVector<QPointF> axialPolarPoints;

};

class RadialDataEvaluate
{
public:
    RadialDataEvaluate()
    {
        typeNum = nullptr;
        status = nullptr;
        detectContent = nullptr;
        dates = nullptr;
        detectObj = nullptr;
        unitNum = 0;
        maxValue,maxAngle,minValue,minAngle,beatValue,beatAngle,verticalValue,verticalAngle = 0;
        concentricity,concentriticAngle,eccentricity,eccentricAngle,flatness,roundness = 0;
    }
public:

    QString typeNum,status,detectContent,dates,detectObj;
    int unitNum;
    double maxValue,maxAngle,minValue,minAngle,beatValue,beatAngle,verticalValue,verticalAngle;
    double concentricity,concentriticAngle,eccentricity,eccentricAngle,flatness,roundness;
    QVector<QPointF> radialPolarPoints;

};

class AlgorithmSet
{
public:
    AlgorithmSet(){
        CalibrationValue_side = 0.0 ;
        CalibrationValue_waist = 0.0;
        product_r = 0.0;
        binary_thresh  = 0;
        product_annulus_width= 0.0;
        hole_annulus_width = 0.0;
        exposure_time  = 0.0;
        max_ratio = 0.0;
        min_ratio = 0.0;
        pt_x = 0.0;
        pt_y = 0.0;
        thread_space = 0.0;
        new_product_top_roi_left_x = 0 ;
        new_product_top_roi_left_y = 0 ;
        new_product_top_roi_right_x = 0;
        new_product_top_roi_right_y = 0;
        new_step_line_left_x = 0 ;
        new_step_line_left_y = 0 ;
        new_step_line_right_x = 0;
        new_step_line_right_y = 0;
        new_neck_line_left_x = 0 ;
        new_neck_line_left_y = 0 ;
        new_neck_line_right_x = 0;
        new_neck_line_right_y = 0;

        product_top_roi_left_x = 0   ;
        product_top_roi_left_y = 0   ;
        product_top_roi_right_x = 0  ;
        product_top_roi_right_y = 0  ;
        step_line_left_x = 0 ;
        step_line_left_y = 0 ;
        step_line_right_x = 0;
        step_line_right_y = 0;
        neck_line_left_x = 0 ;
        neck_line_left_y = 0 ;
        neck_line_right_x = 0;
        neck_line_right_y = 0;
        height = 0.0;
    }
    ~AlgorithmSet(){}

public:

    double CalibrationValue_side;   // 侧视相机标定值
    double CalibrationValue_waist;  // 俯视相机标定值
    double product_r;               // 产品半径
    int binary_thresh;              // 俯视二值化阈值
    double product_annulus_width;   // 俯视找产品边缘点的环带大小
    double hole_annulus_width;      // 找孔的边缘点时环带大小
    double exposure_time;           // 俯视相机曝光时间
    double max_ratio;               // 找内圆边缘点时设置环带最大半径
    double min_ratio;               // 找内圆边缘点时设置环带最大半径
    double pt_x;                    // 俯视工位产品圆心 x 坐标
    double pt_y;                    // 俯视工位产品圆心 y 坐标
    double thread_space;            // 定位螺纹 ROI 时的间距大小
    double height;                  // 产品高度

    int new_product_top_roi_left_x;
    int new_product_top_roi_left_y;
    int new_product_top_roi_right_x;
    int new_product_top_roi_right_y;
    int new_step_line_left_x;
    int new_step_line_left_y;
    int new_step_line_right_x;
    int new_step_line_right_y;
    int new_neck_line_left_x;
    int new_neck_line_left_y;
    int new_neck_line_right_x;
    int new_neck_line_right_y;

    int product_top_roi_left_x;
    int product_top_roi_left_y;
    int product_top_roi_right_x;
    int product_top_roi_right_y;
    int step_line_left_x ;
    int step_line_left_y ;
    int step_line_right_x;
    int step_line_right_y;
    int neck_line_left_x ;
    int neck_line_left_y ;
    int neck_line_right_x;
    int neck_line_right_y;


};



//控制系统读取对象
class ControllerObject
{
public:
    ControllerObject()
    {
        axisOnePosition = 0.0;// 轴1当前位置
        axisTwoPosition = 0.0; // 轴2当前位置
        axisThreePosition = 0.0; // 轴3当前位置
        turnTableAngle    = 0.0; // 气浮转台实时角度
        lastStableAngle = 0.0;
        compressionStatus = false; //压紧状态
        loosingStatus = false; //松开状态
        turnTableLocationFinish = false; //气浮转台定位完成
        axisOneLocationFinish = false; //轴1定位完成
        axisTwoLocationFinish = false; //轴2定位完成
        axisThreeLocationFinish = false; //轴3定位完成
        jerkStatus = false; //急停状态
        warningStatus = false; //报警状态
        axisHeightRange = 0.0; //AKD升降轴高度范围
        turnTableActionFeedBack = false;
        turnTableWarning = false;
        turnTableEmergency = false;
        turnTableControlAxisWarning = false;

    }
public:
    float axisOnePosition;
    float axisTwoPosition;
    float axisThreePosition;
    float turnTableAngle;
    float lastStableAngle;
    bool compressionStatus;
    bool loosingStatus;
    bool turnTableLocationFinish;
    bool axisOneLocationFinish;
    bool axisTwoLocationFinish;
    bool axisThreeLocationFinish;
    bool jerkStatus;
    bool warningStatus;
    float axisHeightRange;
    bool turnTableActionFeedBack;
    bool turnTableWarning; // baojing
    bool turnTableEmergency; //gaojing
    bool turnTableControlAxisWarning;


};


//算法参数对象
class ParaSet
{
public:
    ParaSet(){
        name = nullptr;
        bigDiameter = 0;
        bigDiameter_up = 0;
        bigDiameter_down = 0;

        middleDiameter = 0;
        middleDiameter_up = 0;
        middleDiameter_down = 0;

        smallDiameter = 0;
        smallDiameter_up = 0;
        smallDiameter_down = 0;

        threadPitch = 0;
        threadPitch_up = 0;
        threadPitch_down = 0;

        whorlAngle = 0;
        whorlAngle_up = 0;
        whorlAngle_down = 0;


        driving_hole_dist = 0;
        driving_hole_dist_up = 0;
        driving_hole_dist_down = 0;

        driving_hole_radius = 0;
        driving_hole_radius_up = 0;
        driving_hole_radius_down = 0;

        height = 0;
        height_up = 0;
        height_down = 0;

        inner_radius = 0;
        inner_radius_up = 0;
        inner_radius_down = 0;

        neck = 0   ;   //颈部
        neck_up = 0   ;//颈部上公差
        neck_down = 0 ; //颈部下公差

        step_height = 0;     //台阶
        step_height_up= 0;     //台阶上公差
        step_height_down = 0;    //台阶下公差

        continuousNGCount = 0;
        NGCount = 0;
    }
public:
    QString name;//名字
    double bigDiameter;//大径
    double bigDiameter_up;//大径上公差
    double bigDiameter_down;//大径下公差

    double middleDiameter;//中径
    double middleDiameter_up;//中径上公差
    double middleDiameter_down;//中径下公差

    double smallDiameter;//小径
    double smallDiameter_up;//小径上公差
    double smallDiameter_down;//小径下公差

    double threadPitch;//螺距
    double threadPitch_up;//螺距上公差
    double threadPitch_down;//螺距下公差

    double whorlAngle;//螺纹角
    double whorlAngle_up;//螺纹角上公差
    double whorlAngle_down;//螺纹角下公差

    double driving_hole_dist;       //扳手孔孔径
    double driving_hole_dist_up;    //扳手孔孔径上公差
    double driving_hole_dist_down;  //扳手孔孔径下公差

    double driving_hole_radius;       //扳手孔半径
    double driving_hole_radius_up;    //扳手孔半径上公差
    double driving_hole_radius_down;  //扳手孔半径下公差

    double height;       //高度
    double height_up;    //高度上公差
    double height_down;  //高度下公差

    double inner_radius;       //内径
    double inner_radius_up;    //内径上公差
    double inner_radius_down;  //内径下公差

    double neck;       //颈部
    double neck_up;    //颈部上公差
    double neck_down;  //颈部下公差

    double step_height;      //台阶
    double step_height_up;      //台阶上公差
    double step_height_down;    //台阶下公差

    int continuousNGCount;//连续NG数量
    int NGCount;//NG总数
};

class MeasureData
{
public:
    MeasureData(){
        batchNum = nullptr;
        okCount = 0;
        ngCount = 0;
        date = nullptr;
    }
public:
    QString batchNum;//批号
    int okCount;//ok数量
    int ngCount;//ng数量
    int rework_num;
    QString date;//保存日期
};


class SingleRecord
{
public:
    SingleRecord(){
        batchNum = nullptr;
        date = nullptr;
        type = nullptr;
        bigDiameter = 0.0;    //大径
        effective_diameter = 0.0; //中径
        minor_diameter = 0.0;  //小径
        threadPitch = 0.0;    //螺距
        Angle = 0.0;     //螺纹角
        driving_hole_dist = 0.0;       //扳手孔孔径
        driving_hole_radius_1 = 0.0;   //扳手孔半径
        driving_hole_radius_2 = 0.0;   //扳手孔半径
        height = 0.0;       //高度
        inner_radius = 0.0; //内径
        neck_width = 0.0;         //颈部
        step_height = 0.0;  //台阶
        grade = -1;
    }
public:
    QString batchNum;//批号
    QString date;    //保存日期
    QString type;    //产品种类
    double bigDiameter;    //大径
    double effective_diameter; //中径
    double minor_diameter;  //小径
    double threadPitch;    //螺距
    double Angle;     //螺纹角
    double driving_hole_dist;       //扳手孔孔径
    double driving_hole_radius_1;   //扳手孔半径
    double driving_hole_radius_2;   //扳手孔半径
    double height;       //高度
    double inner_radius; //内径
    double neck_width;         //颈部
    double step_height;  //台阶
    int grade;
};


class Log
{
public:
    Log(){
        content = nullptr;
        role = nullptr;
        name = nullptr;
        time = nullptr;
        level = nullptr;
    }

public:
    QString content;
    QString role;
    QString name;
    QString time;
    QString level;
};




//尺寸检测对象
class DiameterM
{
public:
    DiameterM(){
        size_m_model = nullptr;
        length = 0;
        lengthInterval = 0;
        pointGrabNum = 0;
        realDiamerter = 0;
    }
    friend QDebug& operator<<(QDebug out, const DiameterM& info)
    {
        out << "型号：" << info.size_m_model << "长度：" << info.length << "长度间隔：" << info.lengthInterval
            << "点位采集张数：" << info.pointGrabNum << "真实直径：" << info.realDiamerter;
        return out;
    }
public:
    QString size_m_model;
    double length;
    double lengthInterval;
    int pointGrabNum;//单点位采集张数
    double realDiamerter;
    double diameterValidStart;
    double diameterValidEnd;
};

//伽马扫描对象
class GamaScan
{
public:
    GamaScan(){
        model = nullptr;
        length = 0;
        width = 0;
        thickness = 0;
        diameter = 0;
    }
    friend QDebug& operator<<(QDebug out, const GamaScan& info)
    {
        out << "型号：" << info.model << "长度：" << info.length << "宽度：" << info.width
            << "厚度：" << info.thickness << "直径：" << info.diameter;
        return out;
    }
public:
    QString model;
    double length;
    double width;
    double thickness;
    double diameter;
};























//板片定义
class Pro
{
public:
    Pro(){
        code = nullptr;
        materials = nullptr;
        length = 0;
        width = 0;
        holeDiameterStd = 0;
        holeUp = 0;
        holeDown = 0;
        sphereDiameterStd = 0;
        sphereUp = 0;
        sphereDown = 0;
        xHoleCount = 0;
        yHoleCount = 0;
    }
    friend QDebug& operator <<(QDebug out, const Pro& p){
        out << p.code << p.materials << p.length << p.width << p.holeDiameterStd << p.holeUp << p.holeDown
            << p.sphereDiameterStd << p.sphereUp << p.sphereDown << p.xHoleCount << p.yHoleCount;
        return out;
    }
public:
    QString code;//编号
    QString materials;//材质
    double length;//长度
    double width;//宽度
    double holeDiameterStd;//孔径标准
    double holeUp;
    double holeDown;
    double sphereDiameterStd;//球径标准
    double sphereUp;
    double sphereDown;
    int xHoleCount;//x方向球个数
    int yHoleCount;//y方向球个数
};
//测量工程 主要是为了存储时间
class MeasurePro
{
public:
    MeasurePro(){
        code = nullptr;
        time = nullptr;
    }
    friend QDebug& operator <<(QDebug out, const MeasurePro& p){
        out << p.code << p.time;
        return out;
    }
public:
    QString code;//编号
    QString time;//检测时间
};
//孔径对象
class HoleDiameter
{
public:
    HoleDiameter(){
        code = nullptr;//编号
        x = 0;//x坐标
        y = 0;//y坐标
        holeDiameterVal = 0;//孔直径
    }
    friend QDebug& operator <<(QDebug out, const HoleDiameter& p){
        out << p.code << p.x << p.y << p.holeDiameterVal;
        return out;
    }
public:
    QString code;//编号
    double x;//x坐标
    double y;//y坐标
    double holeDiameterVal;//孔直径
};

//球径对象
class SphereDiameter
{
public:
    SphereDiameter(){
        code = nullptr;//编号
        x = 0;//x坐标
        y = 0;//y坐标
        sphereDiameterVal = 0;//球直径
    }
    friend QDebug& operator <<(QDebug out, const SphereDiameter& p){
        out << p.code << p.x << p.y << p.sphereDiameterVal;
        return out;
    }
public:
    QString code;//编号
    double x;//x坐标
    double y;//y坐标
    double sphereDiameterVal;//球直径
};

//小球堆叠对象
class HeapUp
{
public:
    HeapUp(){
        code = nullptr;//编号
        x = 0;//x坐标
        y = 0;//y坐标
    }
    friend QDebug& operator <<(QDebug out, const HeapUp& p){
        out << p.code << p.x << p.y;
        return out;
    }
public:
    QString code;//编号
    double x;//x坐标
    double y;//y坐标
};

//板片植球率
class Cover
{
public:
    Cover(){
        code = nullptr;//编号
        coverVal = 0;
        outerLength = 0;//外层长度
        outerWidth = 0;//外层宽度
        innerLength = 0;//内层长度
        innerWidth = 0;//内层宽度
        leftWidth = 0;
        rightWidth = 0;
        upWidth = 0;
        downWidth = 0;
        holeCount = 0;
        sphereCount = 0;
        headUpCount = 0;
    }
    friend QDebug& operator <<(QDebug out, const Cover& p){
        out << p.code << p.coverVal << p.outerLength << p.outerWidth <<
               p.outerLength << p.innerWidth << p.leftWidth << p.rightWidth <<
               p.upWidth << p.downWidth <<
               p.holeCount << p.sphereCount << p.headUpCount;
        return out;
    }
public:
    QString code;//编号
    double coverVal;//植球率
    double outerLength;//外层长度
    double outerWidth;//外层宽度
    double innerLength;//内层长度
    double innerWidth;//内层宽度
    double leftWidth;//左宽度
    double rightWidth;//右宽度
    double upWidth;//上宽度
    double downWidth;//下宽度

    int holeCount;//孔个数
    int sphereCount;//球个数
    int headUpCount;//堆叠个数
};


//算法参数
class ArithmeticPara
{
public:
    ArithmeticPara(){
        name = nullptr;
        measureHeight = 0;
        measureExposureTime = 0;
        //预处理参数(56,0)(120,1)
        preproc_thresh = 0;
        prepro_threshType = 0;     // 0 THRESH_BINARY, 1    THRESH_BINARY_INV
        prepro_kernel= 0;          //10

        innerROI_morphShapes = 0;  //0 MORPH_RECT, 1 MORPH_CROSS
        innerROI_kernel1 = 0;      //11
        innerROI_kernel2= 0;       //31
        innerROI_thresh= 0;        //120
        innerROI_threshType= 0;    //0
    }
    friend QDebug& operator <<(QDebug out, const ArithmeticPara& p){
        out << p.name << p.measureHeight << p.measureExposureTime
            << p.preproc_thresh << p.prepro_threshType << p.prepro_kernel
            << p.innerROI_morphShapes << p.innerROI_kernel1 << p.innerROI_kernel2
            << p.innerROI_thresh << p.innerROI_threshType;
        return out;
    }
public:
    QString name;//名称
    double measureHeight;
    int measureExposureTime;
    //预处理参数(56,0)(120,1)
    int preproc_thresh;
    int prepro_threshType;// 0 THRESH_BINARY, 1    THRESH_BINARY_INV
    int prepro_kernel;

    int innerROI_morphShapes;  //0 MORPH_RECT, 1 MORPH_CROSS
    int innerROI_kernel1;      //11
    int innerROI_kernel2;      //31
    int innerROI_thresh;       //120
    int innerROI_threshType;   //0

};

#endif // OBJECTINFO_H
