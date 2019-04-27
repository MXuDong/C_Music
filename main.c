#include <stdio.h>
#include <windows.h>

#define DEFAULT_VOICE_LENGTH 750

/**
 * 使用说明：
 * 文件内容格式：
 * [#/b/-],[1,2,3,4,5,6,7],[0123 4 5678 (音段， 4为正常音段 -1 表示低音.。。)],[音长（负数越小越短，正数越大越长)],[0/1 （有无延音)]
 *
 * 例如：天空之城：
 *
 * 0 表示空拍, 其音节可为 0-8的任意值
 *
-,6,4,-2,0
-,7,4,-2,0
-,1,5,1,1
-,7,4,-2,0
-,1,5,1,0
-,3,5,1,0
-,7,4,2,0
-,0,4,1,0

-,3,4,-2,0
-,3,4,-2,0
-,6,4,1,1
-,5,4,-2,0
-,6,4,1,0
-,1,5,1,0
-,5,4,2,0
-,0,4,1,0

-,3,4,1,0
-,4,4,1,1
-,3,4,-2,0
-,4,4,1,0
-,1,5,1,0
-,3,4,2,0
-,0,4,-2,0

-,1,5,-2,0
-,1,5,-2,0
-,1,5,-2,0
-,7,4,1,1
#,4,4,-2,0
-,4,4,1,0
-,7,4,1,0
-,7,4,2,0

-,0,4,1,0
-,6,4,-2,0
-,7,4,-2,0
-,1,5,1,1
-,7,4,-2,0
-,1,5,1,0
-,3,5,1,0
-,7,4,2,0

-,0,4,1,0
-,3,4,-2,0
-,3,4,-2,0
-,6,4,1,1
-,5,4,-2,0
-,6,4,1,0
-,1,5,1,0
-,5,4,2,0
-,0,4,1,0


 */

/**
 * 音节信息结构体
 * VoiceStruct
 */
typedef struct {
    int voiceDimension;     //八度 如：高八度 等 用 0123 4 5678表示 (4 表示正常音节， 为默认）
    int voiceLength;        //音长
    int voiceIsHasLen;      //? 延音符
    char voiceHeigh[2];     //音节 如:1\2\3\4\5\6\7

    //钢琴最低音为：四低音6
    //钢琴最高音为：四高音1

} VoiceStruct;

//平音频率对照
long VOICE_LIST[] = {261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493};

//声明音长大小
int VOICE_LENGTH_INPUT = 0;

/**
 * Part desc : 函数声明
 * auth: Dong
 * desc: 声明程序中的函数
 */
int GET_VOICE_HEIGHT(char VOICE_FLAG, char VOICE_LEVEL, int dimension); //获取实际音高频率
int CHECK_VOICE(VoiceStruct voiceStruct);                               //检查音节信息是否正确 如果正常则返回实际音高，否则-1
void DO_BEEP_PRINT(VoiceStruct voiceStruct);                            //打印并输出音节
int getLength(int length, int defaultLength, int isHasLen);             //获取音符长度
void MAIN_RUN(FILE * aimFile);                                          //主函数


/**
 * func: main
 * auth: Dong
 * desc: 程序的唯一入口， 不需要参数
 */
int main() {

    printf("Please input the file path which end with :[.cmusic] -：");
    char filePath[1000];
    gets(filePath);

    printf("%s", filePath);

    FILE * aimFirstFile;

    aimFirstFile = fopen(filePath, "r+");

    MAIN_RUN(aimFirstFile);


    fclose(aimFirstFile);
    return 0;
}

void MAIN_RUN(FILE * aimFile){

    VoiceStruct voiceStruct;
    printf("\n");

    while((fscanf(aimFile, "%c,%c,%d,%d,%d\n",
            &voiceStruct.voiceHeigh[0],
            &voiceStruct.voiceHeigh[1],
            &voiceStruct.voiceDimension,
            &voiceStruct.voiceLength,
            &voiceStruct.voiceIsHasLen)) != -1){

        DO_BEEP_PRINT(voiceStruct);
    }
}


/**
 * func: DO_BEEP_PRINT
 * auth: Dong
 * desc: 蜂鸣并打印某位音节
 */
void DO_BEEP_PRINT(VoiceStruct voiceStruct) {
    int voiceNum = CHECK_VOICE(voiceStruct);
    if (voiceNum != -1) {
        int voiceLength = getLength(voiceStruct.voiceLength,
                                    VOICE_LENGTH_INPUT == 0 ? DEFAULT_VOICE_LENGTH : VOICE_LENGTH_INPUT,
                                    voiceStruct.voiceIsHasLen);
        printf("Voice Height : %c %c , Dim: %d, Voice Length : %d(ms). \tRealNum : %d\n",
               voiceStruct.voiceHeigh[0], voiceStruct.voiceHeigh[1], voiceStruct.voiceLength - 4, voiceLength,
               voiceNum);

        Beep(voiceNum, voiceLength);
    }
}

/**
 * func: CHECK_VOICE
 * auth: Dong
 * desc: 检查音节是否正确
 * return : 如果正确返回实际音高频率，否则返回-1
 */
int CHECK_VOICE(VoiceStruct voiceStruct) {
    int voice = GET_VOICE_HEIGHT(voiceStruct.voiceHeigh[0], voiceStruct.voiceHeigh[1], voiceStruct.voiceDimension);
    //音节状态不正确
    if (voice == -1) {
        return -1;
    }

    // 不允许0出现， 如果为负数，则为短音符 例如；-4 = 四分之音符
    if (voiceStruct.voiceLength == 0) {
        return -1;
    }

    // 不能超过8 不能小于-16
    if (voiceStruct.voiceLength < -16 || voiceStruct.voiceLength > 8) {
        return -1;
    }

    return voice;
}

/**
 * func: GET_VOICE_HEIGHT
 * auth: Dong
 * param VOICE_FLAG : 是否降半音、升半音等
 * param VOICE_LEVEL: 该音节等级
 * desc: 获取当前音节的实际BEEP数值
 *
 * return : -1 : 不能正常转换； other: 转换后的数值
 */
int GET_VOICE_HEIGHT(char VOICE_FLAG, char VOICE_LEVEL, int dimension) {
    int voiceLevel;
    int voiceFlag;

    // 0 空拍， 1234567 正常音阶
    if (VOICE_LEVEL >= '0' && VOICE_LEVEL <= '7') {
        voiceLevel = VOICE_LEVEL - '0';
    } else {
        // 异常音阶
        return -1;
    }

    if (voiceLevel == 0) {
        return 0;
    }

    // # 升半调， b # 降半调
    voiceFlag = VOICE_FLAG == '#' ? 1 : (VOICE_FLAG == 'b' ? -1 : 0);

    int voiceListIndex = 0;

    //音阶转换为半音
    switch (voiceLevel) {
        case 1: {
            voiceListIndex = 0;
            break;
        }
        case 2: {
            voiceListIndex = 2;
            break;
        }
        case 3: {
            voiceListIndex = 4;
            break;
        }
        case 4: {
            voiceListIndex = 5;
            break;
        }
        case 5: {
            voiceListIndex = 7;
            break;
        }
        case 6: {
            voiceListIndex = 9;
            break;
        }
        case 7: {
            voiceListIndex = 11;
            break;
        }
        default:
            return -1;
    }

    //音高范围判断
    if (dimension < 0 || dimension > 8) {
        return -1;
    }

    //钢琴最低音判断
    // 其实小于200HZ基本听不见。。。。。。 而重低音A（La 6）才20多。。。
    if (dimension == 0) {
        if (voiceListIndex + voiceFlag < 9) {
            //小于最低音
            return -1;
        }
    }

    //钢琴最高音判断
    if (dimension == 8) {
        if (voiceListIndex + voiceFlag > 0) {
            //超过最高音
            return -1;
        }
    }

    //添加升降半音
    voiceListIndex += voiceFlag;
    if (voiceListIndex == -1) {
        voiceListIndex = 11;
    } else if (voiceListIndex == 12) {
        voiceListIndex = 0;
    }

    // 音高实际对应频率
    long voiceNumber = VOICE_LIST[voiceListIndex];

    //低高八度处理， 每降一次八度，频率除2
    int demp = dimension - 4;
    if (demp > 0) {
        while (demp != 0) {
            voiceNumber *= 2;
            demp--;
        }
    }
    if (demp < 0) {
        while (demp != 0) {
            voiceNumber /= 2;
            demp++;
        }
    }

    return voiceNumber;
}

/**
 * func: getLength
 * auth: Dong
 * desc: 获取音符长度
 */
int getLength(int length, int defaultLength, int isHasLen) {


    if (length < 0) {
        length *= -1;
        return defaultLength / length;
    }

    if (isHasLen) {
        return defaultLength * length + defaultLength / 2;
    }
    return defaultLength * length;
}