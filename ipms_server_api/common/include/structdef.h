/************************************************************************************/
//ģ�����ƣ�GSLB����,���ýṹ�嶨��
//����˵����1)�ṹ��
//���ߣ������� tianya0513@hotmail.com
//���ڣ�2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __STRUCTDEF_H__
#define __STRUCTDEF_H__


typedef struct _DNION_TRANSFER_HEADER
{
	int			nMsgLen;					//��Ϣ�ܳ���
	int			nMsgCommand;				//��Ϣ�����
	int			nErrorCode;					//��Ϣ������
	_DNION_TRANSFER_HEADER()
	{
		nMsgLen = 0;
		nMsgCommand = 0;
		nErrorCode = 0;
	};
}DNION_TRANSFER_HEADER;
//===============================================================//
//4 BYTES || ��Ϣ�ܳ���
//4 BYTES || ��Ϣ����  ADD DELETE | ADD RESPONSE DELETE RESPONSE 
//4 BYTES || ��Ϣ������
//===============================================================
//XML -----------------------------------------------------------
//===============================================================

//===============================================================
//DATA ----------------------------------------------------------
//===============================================================//





#endif //__STRUCTDEF_H__

