
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
int add1 = 0x50;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
int add2 = 0x42;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
const int ledOut = A2;
const int buttonPin = A3;
int buttonState = 0;
unsigned char hexdata[9] = {};
unsigned char hexsent[9] = {};
int runLed = 7;
bool led = 0;//�����л�LED����

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
	pinMode(ledOut, OUTPUT);
	pinMode(buttonPin, INPUT);
	pinMode(runLed, OUTPUT);//����LED
	Serial.begin(9600);

}

void loop()
{
	led = !led;
	if (led == 0)
	{
		digitalWrite(runLed, LOW);
	}
	else
	{
		digitalWrite(runLed, HIGH);
	}
	buttonState = digitalRead(buttonPin);
	digitalWrite(ledOut, buttonState);
	if (buttonState == HIGH)
	{
		hexsent[3] = 0x01;
		for (int k = 0; k<9; k++) Serial.write(hexsent[k]);
	}
	else
	{
		hexsent[3] = 0xFF;
	}


	while (Serial.available() > 0)
	{
		for (int i = 0; i<9; i++) hexdata[i] = char(Serial.read());
		delay(3);
	}
	delay(2000);

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