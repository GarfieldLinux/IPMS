/************************************************************************************/
//ģ�����ƣ�GSLB���� ��������ͷ�ļ� 
//����˵����1)��������
//���ߣ������� tianya0513@hotmail.com
//���ڣ�2008-9-1
//Copyright (C), 2008-2010 Dnion Tech. Co., Ltd.
/************************************************************************************/
#ifndef __CONSTDEF_H__
#define __CONSTDEF_H__

#ifndef _MAX_PATH
	#define	_MAX_PATH 255
#endif

//Ĭ�ϻ��峤��,���ڶ�ȡ����
#define DEFAULT_BUFFER_SIZE	8192
//�����ļ���
#define DEFAULT_CONFIG_NAME	EXENAME".ini"
//IP��ַʹ�õĳ���
#define DEFAULT_HOST_SIZE	32

//����ļ�������
#define	MAX_FILENO_SIZE	10000

//���������Լ�״̬�붨��
/*******************************************************************************************/
#define DT_SUB		0x00000000			//������ѯ
#define	DT_SUB_REP	0x80000000			//������ѯ��Ӧ
#define	DT_ADD		0x00000001			//�����ļ�
#define DT_DEL		0x00000002			//ɾ���ļ�
#define DT_ADEL_REP	0x80000001			//ɾ���ļ���Ӧ �� �����ļ���Ӧ��ͬ


//ǰ10��Ԥ��
#define DT_FILE		0x0000000A			//�����ļ�-----------δʹ��
#define DT_FILE_REP	0x8000000A			//�����ļ���Ӧ


/*******************************************************************************************/
#define E_SUCCESS			0x00000000			//�ɹ�
#define E_BUSY_NOW			0x00000001			//�ڵ�æ
#define	E_FAILE				0x00000002			//ɾ��ʧ��
#define E_FILE_NOTEXIST		0x00000003			//ɾ��ʱ�ļ�������
#define E_FILE_EXIST_DEL	0x00000004			//����ʱ�����Ѿ����ڵ��ļ�,ɾ��ʧ��
#define E_FILE_EXIST		0x00000005			//����ʱ�����ļ��Ѿ�����,����MD5��ͬ
#define E_MD5_CHECK_FAIL	0x00000006			//������ɺ���֤MD5ʧ��
#define E_RENAME_FAIL		0x00000007			//������ɺ�������ʧ��
#define E_CONNECT_FAIL		0x00000008			//UDT���Ӵ���
#define E_SUB_SYS			0x00000009			//�����ַ���,���ٴβ�ѯ�����ַ���״̬
#define E_SEND_ERROR		0x0000000A			//��������ʧ��
#define E_RECV_ERROR		0x0000000B			//��������ʧ��
#define E_SUB_NOEXIST		0x0000000C			//�����ַ���,�������񲻴���,��������
#define	E_FEINT_STATE		0x0000000D			//�����ַ������ؼ������״̬

#define E_MD5_FAILED	0x00000004			//MD5��֤ʧ��
#define	E_WRITE_ERROR	0x00000005			//д�ļ�ʧ��





/*******************************************************************************************/




#endif //__CONSTDEF_H__
