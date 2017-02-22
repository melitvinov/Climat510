#include "syntax.h"
#include "405_ConfigEnRuSr.h"

const uint16_t InitSystems[cSTepl][cConfSSystem]={
//����1
    {5, //���������
        2, //����������
        1, //�����
#ifdef AHU
        2,
#else
        0, //UltraClima
#endif
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����2
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����3
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����4
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����5
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����6
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����7
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

//����8
    {5, //���������
        2, //����������
        1, //�����
        0, //UltraClima
        2, //��2
        6, //��������
        5, //����
        1, //�����������
        0, //������ 1
        0},//������ 2

};

const const uint16_t MechC[cSTepl][SUM_NAME_CONF]={
    {   /* ������� 1*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    },

    {   /* ������� 2*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

    {   /* ������� 3*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

    {   /* ������� 4*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {   /* ������� 5*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {   /* ������� 6*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {   /* ������� 7*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {   /* ������� 8*/
        0,  //������ 1
        0,  //������ 2
        0,  //������ 3
        0,  //������ 4
        0,  //������ 5

        0,  //������� �����
        0,  //������� ��
        0,  //�����
        0,  //����� �������������� �����
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������
        0,  //����� ������������ ������

        0,  //��2
        0,  //��������� ��������
        0,  //������
        0,  //����� 1
        0,  //����� 2

        0,  //����� 3
        0,  //����� 4
        0,  //����� 5
        0,  //����������
        0,  //�����������

        0,  //����� ����
        0,  //������ �������� ����
        0,  //��������
        0,  //���������� ����
        0,

        0,
        0,
        0,
        0,
        0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
