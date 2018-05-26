#include <SSD1306_text.h>//OLED���ļ�
#include <Wire.h>//IIC���ļ�
#define OLED_RESET 5
SSD1306_text display(OLED_RESET);
bool led = 0;//�����л�LED����
			 /*------------------------------------------------------------------------------*/
unsigned char add1 = 0x40;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
unsigned char add2 = 0x4A;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
						  /*------------------------------------------------------------------------------*/
unsigned int AlarmValue= 0;//��С0�����1024
unsigned int MQ2Value = 0;//��С0�����1024
int runLed = 7;
int MQ2 = A3;
int Alarm = A2;
/*------------------------------------------------------------------------------*/
unsigned char counter = 0;//������
unsigned char hexData[9] = {};//�����ݴ�
unsigned char zgbData[5] = {};//��ȡ��zigbee���ݴ����ݴ�
unsigned char getConfig[7] = { 0xaa, 0xaa, 0x55, 0x55, 0xa5, 0xa5, 0xc3 }; //��ѯZigbee Panid��Ƶ��ָ��
unsigned char hexSent[9] = { 0xb5, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5b }; //����������
unsigned char pingOrder[9] = { 0xa5, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5a }; //PINGָ��
																					   /*------------------------------------------------------------------------------*/
void setup() {
	delay(1000);
	/*------------------------------------------------------------------------------*/
	Serial.begin(9600);
	pinMode(MQ2, INPUT);
	pinMode(Alarm, INPUT);
	pinMode(runLed, OUTPUT);//����LED
	/*------------------------------------------------------------------------------*/
	hexSent[1] = add1;
	hexSent[2] = add2;//�豸��ַд������
					  /*------------------------------------------------------------------------------*/
	display.init();//��ʼ��OLED����ʾ�������Ϣ
	display.clear();
	display.setCursor(0, 36);
	display.setTextSize(1, 1);
	display.write("WSN-S V2.0");
	display.setCursor(1, 1);
	display.setTextSize(1, 0);
	display.write("-------------------------");
	/*------------------------------------------------------------------------------*/
	display.setCursor(2, 0);//��ʾ�˵�
	display.setTextSize(1, 1);
	display.write("Zigbee Panid:");
	/*------------------------------------------------------------------------------*/
	display.setCursor(3, 0);//��ʾ�˵�
	display.setTextSize(1, 1);
	display.write("Zigbee Channel:");
	/*------------------------------------------------------------------------------*/
	display.setCursor(4, 0);//��ʾ�˵�
	display.setTextSize(1, 1);
	display.write("WSN Dev Add:");
	/*---------------------------------------*/
	display.setCursor(4, 72);//��OLED��ʾ�豸Ӳ����ַ��λ��λ
	display.setTextSize(1, 1);
	display.write(hexH2ascii(add1));
	display.setCursor(4, 78);
	display.setTextSize(1, 1);
	display.write(hexL2ascii(add1));
	display.setCursor(4, 84);
	display.setTextSize(1, 1);
	display.write(" ");
	display.setCursor(4, 90);
	display.setTextSize(1, 1);
	display.write(hexH2ascii(add2));
	display.setCursor(4, 96);
	display.setTextSize(1, 1);
	display.write(hexL2ascii(add2));
	/*------------------------------------------------------------------------------*/
bailout://��ǵ�
	for (int k = 0; k < 7; k++) Serial.write(getConfig[k]); //���Ͳ�ѯZigbee Panid��Ƶ��
	delay(1000);
	while (Serial.available() > 0)
	{
		for (int i = 0; i < 5; i++) zgbData[i] = char(Serial.read());
		delay(3);
	}//�Ƚ������
	delay(1000);

	if (zgbData[0] == 0xAA && zgbData[1] == 0xC3)//�������ͷ��ȷ��׼����ʾ
	{
		display.setCursor(2, 78);//��ʾPANID��λ
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[3]));
		display.setCursor(2, 84);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[3]));
		/*---------------------------------------*/
		display.setCursor(2, 90);//��ʾPANID��λ
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[2]));
		display.setCursor(2, 96);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[2]));
		/*---------------------------------------*/
		display.setCursor(3, 90);//��ʾƵ��
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[4]));
		display.setCursor(3, 96);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[4]));

	}
	else//�������ȷ
	{
		counter++;//
		if (counter > 2)//��������1������������3����ʾ����С�������¶�ȡ
		{
			display.setCursor(2, 78);//��ʾzigbee��ȡ��ʱ����
			display.setTextSize(1, 1);
			display.write("error");
			display.setCursor(3, 90);//��ʾzigbee��ȡ��ʱ����
			display.setTextSize(1, 1);
			display.write("error");
		}
		else
		{
			goto bailout;//���ر�ǵ�
		}
	}
	/*------------------------------------------------------------------------------*/
}


void loop() {
	/*------------------------------------------------------------------------------*/
	led = !led;
	if (led == 0)
	{
		digitalWrite(runLed, LOW);
	}
	else
	{
		digitalWrite(runLed, HIGH);
	}

	AlarmValue = analogRead(Alarm) / 4;
	MQ2Value = analogRead(MQ2) / 2;
	/*------------------------------------------------------------------------------*/
	display.setCursor(5, 0);//��ʾ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("AlarmValue:");
	if (AlarmValue < 10)
	{
		display.setCursor(5, 72);
		display.setTextSize(1, 1);
		display.write("         ");
	}
	else if (AlarmValue < 100)
	{
		display.setCursor(5, 78);
		display.setTextSize(1, 1);
		display.write("        ");
	}
	else if (AlarmValue < 1000)
	{
		display.setCursor(5, 84);
		display.setTextSize(1, 1);
		display.write("       ");
	}
	display.setCursor(5, 66);
	display.setTextSize(1, 1);
	display.print(AlarmValue);
	/*------------------------------------------------------------------------------*/
	display.setCursor(6, 0);//��ʾ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("MQ2  Value:");
	if (MQ2Value < 10)
	{
		display.setCursor(6, 72);
		display.setTextSize(1, 1);
		display.write("         ");
	}
	else if (MQ2Value < 100)
	{
		display.setCursor(6, 78);
		display.setTextSize(1, 1);
		display.write("        ");
	}
	else if (MQ2Value < 1000)
	{
		display.setCursor(6, 84);
		display.setTextSize(1, 1);
		display.write("       ");
	}

	display.setCursor(6, 66);
	display.setTextSize(1, 1);
	display.print(MQ2Value);
	/*------------------------------------------------------------------------------*/
	if (MQ2Value > AlarmValue)
	{
		hexSent[3] = 0x01;
		for (int k = 0; k<9; k++) Serial.write(hexSent[k]);
	}
	else
	{
		hexSent[3] = 0xFF;
	}
	/*------------------------------------------------------------------------------*/
	while (Serial.available() > 0)//����ͨѶ����
	{
		for (int i = 0; i < 9; i++) hexData[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);

	if (hexData[0] == 0xA5 && hexData[8] == 0x5A)//������ݰ�ͷ�����ݰ�β
	{
		if (hexData[1] == add1&&hexData[2] == add2 || hexData[1] == 0x00 && hexData[2] == 0x00)//��鱨�ĵڶ�λ�Ƿ�Ϊ�㲥��ַ������ܹ��ܵ�ַ�����޸�
		{
			if (hexData[3] == 0xFF && hexData[4] == 0xFF && hexData[5] == 0xFF && hexData[6] == 0xFF && hexData[7] == 0xFF)
			{
				for (int k = 0; k<9; k++) Serial.write(hexSent[k]);
				hexData[0] = 0xFF;
			}
		}
	}
}

char hexH2ascii(char x)//16����ȡ��λת�ַ�
{
	x = x >> 4;
	x = x & 0x0f;
	if (x > 9)
	{
		x = x + 87;
	}
	else
	{
		x = x + 48;
	}
	return x;
}
char hexL2ascii(char x)//16����ȡ��λת�ַ�
{
	x = x & 0x0f;
	if (x > 9)
	{
		x = x + 87;
	}
	else
	{
		x = x + 48;
	}
	return x;
}