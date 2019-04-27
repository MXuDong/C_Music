#include <stdio.h>
#include <windows.h>

#define DEFAULT_VOICE_LENGTH 750

/**
 * ʹ��˵����
 * �ļ����ݸ�ʽ��
 * [#/b/-],[1,2,3,4,5,6,7],[0123 4 5678 (���Σ� 4Ϊ�������� -1 ��ʾ����.����)],[����������ԽСԽ�̣�����Խ��Խ��)],[0/1 ����������)]
 *
 * ���磺���֮�ǣ�
 *
 * 0 ��ʾ����, �����ڿ�Ϊ 0-8������ֵ
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
 * ������Ϣ�ṹ��
 * VoiceStruct
 */
typedef struct {
    int voiceDimension;     //�˶� �磺�߰˶� �� �� 0123 4 5678��ʾ (4 ��ʾ�������ڣ� ΪĬ�ϣ�
    int voiceLength;        //����
    int voiceIsHasLen;      //? ������
    char voiceHeigh[2];     //���� ��:1\2\3\4\5\6\7

    //���������Ϊ���ĵ���6
    //���������Ϊ���ĸ���1

} VoiceStruct;

//ƽ��Ƶ�ʶ���
long VOICE_LIST[] = {261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493};

//����������С
int VOICE_LENGTH_INPUT = 0;

/**
 * Part desc : ��������
 * auth: Dong
 * desc: ���������еĺ���
 */
int GET_VOICE_HEIGHT(char VOICE_FLAG, char VOICE_LEVEL, int dimension); //��ȡʵ������Ƶ��
int CHECK_VOICE(VoiceStruct voiceStruct);                               //���������Ϣ�Ƿ���ȷ ��������򷵻�ʵ�����ߣ�����-1
void DO_BEEP_PRINT(VoiceStruct voiceStruct);                            //��ӡ���������
int getLength(int length, int defaultLength, int isHasLen);             //��ȡ��������
void MAIN_RUN(FILE * aimFile);                                          //������


/**
 * func: main
 * auth: Dong
 * desc: �����Ψһ��ڣ� ����Ҫ����
 */
int main() {

    printf("Please input the file path which end with :[.cmusic] -��");
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
 * desc: ��������ӡĳλ����
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
 * desc: ��������Ƿ���ȷ
 * return : �����ȷ����ʵ������Ƶ�ʣ����򷵻�-1
 */
int CHECK_VOICE(VoiceStruct voiceStruct) {
    int voice = GET_VOICE_HEIGHT(voiceStruct.voiceHeigh[0], voiceStruct.voiceHeigh[1], voiceStruct.voiceDimension);
    //����״̬����ȷ
    if (voice == -1) {
        return -1;
    }

    // ������0���֣� ���Ϊ��������Ϊ������ ���磻-4 = �ķ�֮����
    if (voiceStruct.voiceLength == 0) {
        return -1;
    }

    // ���ܳ���8 ����С��-16
    if (voiceStruct.voiceLength < -16 || voiceStruct.voiceLength > 8) {
        return -1;
    }

    return voice;
}

/**
 * func: GET_VOICE_HEIGHT
 * auth: Dong
 * param VOICE_FLAG : �Ƿ񽵰�������������
 * param VOICE_LEVEL: �����ڵȼ�
 * desc: ��ȡ��ǰ���ڵ�ʵ��BEEP��ֵ
 *
 * return : -1 : ��������ת���� other: ת�������ֵ
 */
int GET_VOICE_HEIGHT(char VOICE_FLAG, char VOICE_LEVEL, int dimension) {
    int voiceLevel;
    int voiceFlag;

    // 0 ���ģ� 1234567 ��������
    if (VOICE_LEVEL >= '0' && VOICE_LEVEL <= '7') {
        voiceLevel = VOICE_LEVEL - '0';
    } else {
        // �쳣����
        return -1;
    }

    if (voiceLevel == 0) {
        return 0;
    }

    // # ������� b # �����
    voiceFlag = VOICE_FLAG == '#' ? 1 : (VOICE_FLAG == 'b' ? -1 : 0);

    int voiceListIndex = 0;

    //����ת��Ϊ����
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

    //���߷�Χ�ж�
    if (dimension < 0 || dimension > 8) {
        return -1;
    }

    //����������ж�
    // ��ʵС��200HZ���������������������� ���ص���A��La 6����20�ࡣ����
    if (dimension == 0) {
        if (voiceListIndex + voiceFlag < 9) {
            //С�������
            return -1;
        }
    }

    //����������ж�
    if (dimension == 8) {
        if (voiceListIndex + voiceFlag > 0) {
            //���������
            return -1;
        }
    }

    //�����������
    voiceListIndex += voiceFlag;
    if (voiceListIndex == -1) {
        voiceListIndex = 11;
    } else if (voiceListIndex == 12) {
        voiceListIndex = 0;
    }

    // ����ʵ�ʶ�ӦƵ��
    long voiceNumber = VOICE_LIST[voiceListIndex];

    //�͸߰˶ȴ��� ÿ��һ�ΰ˶ȣ�Ƶ�ʳ�2
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
 * desc: ��ȡ��������
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