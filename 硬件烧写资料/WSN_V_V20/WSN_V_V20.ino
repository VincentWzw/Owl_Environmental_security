
/*
_ooOoo_
o8888888o
88" . "88
(| -_- |)
O\  =  /O
____/`---'\____
.'  \\|     |//  `.
/  \\|||  :  |||//  \
/  _||||| -:- |||||-  \
|   | \\\  -  /// |   |
| \_|  ''\---/''  |   |
\  .-\__  `-`  ___/-. /
__`. .'  /--.--\  `. . __
."" '<  `.___\_<|>_/___.'  >'"".
| | :  `- \`.;`\ _ /`;.`/ - ` : | |
\  \ `-.   \_ __\ /__ _/   .-` /  /
======`-.____`-.___\_____/___.-`____.-'======
`=---='
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
���汣��       ����BUG
*/
#include <util/delay.h>
int add1 = 0x60;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
int add2 = 0x37;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
int buttonPin = A2;
int alarmLed = A3;
int runLed = 7;
bool led = 0;//�����л�LED����
int buttonTemp = 0;
unsigned char hexdata[9] = {};
unsigned char hexsent[9] = {};

void setup()
{

	hexsent[0] = 0xB5;
	hexsent[1] = add1;
	hexsent[2] = add2;
	hexsent[4] = 0xFF;
	hexsent[5] = 0xFF;
	hexsent[6] = 0xFF;
	hexsent[7] = 0xFF;
	hexsent[8] = 0x5B;
	pinMode(buttonPin, INPUT);
	pinMode(alarmLed, OUTPUT);
	pinMode(runLed, OUTPUT);
	Serial.begin(9600);

}

void loop()
{
	buttonTemp = analogRead(buttonPin);
	/*------------------------------------------------------------------------------*/
	led = !led;//LED��˸
	if (led == 0)
	{
		digitalWrite(runLed, LOW);
	}
	else
	{
		digitalWrite(runLed, HIGH);
	}
	/*------------------------------------------------------------------------------*/
	if (buttonTemp >  80)
	{
		digitalWrite(alarmLed, LOW);
		hexsent[3] = 0x01;
		for (int k = 0; k<9; k++) Serial.write(hexsent[k]);
	}
	else
	{
		hexsent[3] = 0xFF;
		digitalWrite(alarmLed, HIGH);
	}


	while (Serial.available() > 0)
	{
		for (int i = 0; i<9; i++) hexdata[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);

	if (hexdata[0] == 0xA5 && hexdata[8] == 0x5A)//������ݰ�ͷ�����ݰ�β
	{
		if (hexdata[1] == add1&&hexdata[2] == add2 || hexdata[1] == 0x00 && hexdata[2] == 0x00)//��鱨�ĵڶ�λ�Ƿ�Ϊ�㲥��ַ������ܹ��ܵ�ַ�����޸�
		{
			if (hexdata[3] == 0xFF && hexdata[4] == 0xFF && hexdata[5] == 0xFF && hexdata[6] == 0xFF && hexdata[7] == 0xFF)
			{
				for (int k = 0; k<9; k++) Serial.write(hexsent[k]);
				hexdata[0] = 0xFF;
			}
		}
	}


}