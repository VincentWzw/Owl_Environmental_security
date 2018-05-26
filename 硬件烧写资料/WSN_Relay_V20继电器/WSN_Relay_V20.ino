int relay1 = 2;
int relay2 = 3;
int relay3 = 4;
int relay4 = 5;
int add1 = 0x20;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
int add2 = 0x57;//��Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
int runLed = 7;
bool led = 0;//�����л�LED����
unsigned char hexdata[9] = {};
unsigned char hex;
unsigned char hexsent[9] = {};

void setup()
{
	pinMode(relay1, OUTPUT);
	pinMode(relay2, OUTPUT);
	pinMode(relay3, OUTPUT);
	pinMode(relay4, OUTPUT);
	pinMode(runLed, OUTPUT);//����LED
	hexsent[0] = 0xB5;
	hexsent[1] = add1;
	hexsent[2] = add2;
	hexsent[3] = 0x00;
	hexsent[4] = 0x00;
	hexsent[5] = 0x00;
	hexsent[6] = 0x00;
	hexsent[7] = 0xFF;
	hexsent[8] = 0x5B;
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
	while (Serial.available() > 0)
	{
		for (int i = 0; i < 9; i++) hexdata[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);
	if (hexdata[0] == 0xA5 && hexdata[8] == 0x5A) //������ݰ�ͷ�����ݰ�β
	{
		if (hexdata[1] == add1 && hexdata[2] == add2 || hexdata[1] == 0x00 && hexdata[2] == 0x00) //��鱨�ĵڶ�λ�Ƿ�Ϊ�㲥��ַ������ܹ��ܵ�ַ�����޸�
		{
			if (hexdata[3] == 0xFF && hexdata[4] == 0xFF && hexdata[5] == 0xFF && hexdata[6] == 0xFF && hexdata[7] == 0xFF)
			{
				for (int k = 0; k < 9; k++) Serial.write(hexsent[k]);
				hexdata[0] = 0xFF;
			}
			if (hexdata[3] <= 0x01 && hexdata[4] <= 0x01 && hexdata[5] <= 0x01 && hexdata[6] <= 0x01 && hexdata[7] <= 0xFF) //�ж�4.5.6.7�����Ƿ�С�ڵ���0x01��8�Ƿ�С�ڵ���0x01
			{
				digitalWrite(relay1, hexdata[3]);
				digitalWrite(relay2, hexdata[4]);
				digitalWrite(relay3, hexdata[5]);
				digitalWrite(relay4, hexdata[6]);
				hexsent[3] = hexdata[3]; //�������飬׼�����������ʶ��������������ġ����޸�����Ϊ�豸��ַ��
				hexsent[4] = hexdata[4];
				hexsent[5] = hexdata[5];
				hexsent[6] = hexdata[6];
				hexdata[0] = 0xFF;
				for (int k = 0; k < 9; k++) Serial.write(hexsent[k]);
			}
		}
	}
}
