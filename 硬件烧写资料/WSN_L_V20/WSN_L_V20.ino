#include <SSD1306_text.h>//OLED���ļ�
#include <Wire.h>//IIC���ļ�
#define OLED_RESET 5
SSD1306_text display(OLED_RESET);
bool led = 0;//�����л�LED����
			 /*------------------------------------------------------------------------------*/
unsigned char add1 = 0x70;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
unsigned char add2 = 0x1F;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
						  /*------------------------------------------------------------------------------*/
unsigned int inNum = 0;//��С0�����65535
unsigned int outNum = 0;//��С0�����65535
int runLed = 7;
int Lpin = 4;
int beep = A2;
unsigned char state = 1;
unsigned char flag = 0;
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
	pinMode(Lpin, INPUT);
	pinMode(beep, OUTPUT);
	pinMode(runLed, OUTPUT);//����LED
	digitalWrite(beep, HIGH);
	delay(150);
	digitalWrite(beep, LOW);
	attachInterrupt(0, stateChange0, FALLING);
	attachInterrupt(1, stateChange1, FALLING);
	/*------------------------------------------------------------------------------*/
	hexSent[1] = add1;
	hexSent[2] = add2;//�豸��ַд������
					  /*------------------------------------------------------------------------------*/
	display.init();//��ʼ��OLED����ʾ�������Ϣ
	display.clear();
	display.setCursor(0, 36);
	display.setTextSize(1, 1);
	display.write("WSN-L V2.0");
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

void stateChange0()
{
	state = digitalRead(Lpin);
	if (state == 0)
	{
		flag = 1;
	}
}

void stateChange1()
{
	state = digitalRead(Lpin);
	if (state == 0)
	{
		flag = 2;
	}
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
	if (inNum > 60000)
	{
		inNum = 0;
	}
	if (outNum > 60000)
	{
		outNum = 0;
	}
	if (flag == 1)
	{
		digitalWrite(beep, HIGH);
		hexSent[3] = 0x01;
		inNum++;
		for (int k = 0; k < 9; k++) Serial.write(hexSent[k]);
		delay(100);
		flag = 0;
		hexSent[3] = 0xFF;
		digitalWrite(beep, LOW);
	}
	if (flag == 2)
	{
		digitalWrite(beep, HIGH);
		hexSent[3] = 0x02;
		outNum++;
		for (int k = 0; k < 9; k++) Serial.write(hexSent[k]);
		delay(100);
		flag = 0;
		hexSent[3] = 0xFF;
		digitalWrite(beep, LOW);
	}
	/*------------------------------------------------------------------------------*/
	display.setCursor(5, 0);//��ʾ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write(" IN:");
	if (inNum < 10)
	{
		display.setCursor(5, 31);
		display.setTextSize(1, 1);
		display.write("                ");
	}
	else if (inNum < 100)
	{
		display.setCursor(5, 37);
		display.setTextSize(1, 1);
		display.write("               ");
	}
	else if (inNum < 1000)
	{
		display.setCursor(5, 43);
		display.setTextSize(1, 1);
		display.write("              ");
	}
	else if (inNum < 10000)
	{
		display.setCursor(5, 49);
		display.setTextSize(1, 1);
		display.write("             ");
	}
	display.setCursor(5, 25);
	display.setTextSize(1, 1);
	display.print(inNum);
	/*------------------------------------------------------------------------------*/
	display.setCursor(6, 0);//��ʾ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("OUT:");
	if (outNum < 10)
	{
		display.setCursor(6, 31);
		display.setTextSize(1, 1);
		display.write("                ");
	}
	else if (outNum < 100)
	{
		display.setCursor(6, 37);
		display.setTextSize(1, 1);
		display.write("               ");
	}
	else if (outNum < 1000)
	{
		display.setCursor(6, 43);
		display.setTextSize(1, 1);
		display.write("              ");
	}
	else if (outNum < 10000)
	{
		display.setCursor(6, 49);
		display.setTextSize(1, 1);
		display.write("             ");
	}
	display.setCursor(6, 25);
	display.setTextSize(1, 1);
	display.print(outNum);
	/*------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------*/
	while (Serial.available() > 0)//����ͨѶ����
	{
		for (int i = 0; i < 9; i++) hexData[i] = char(Serial.read());
		delay(3);
	}
	delay(500);

	if (hexData[0] == 0xA5 && hexData[8] == 0x5A) //������ݰ�ͷ�����ݰ�β
	{
		if (hexData[1] == add1 && hexData[2] == add2 || hexData[1] == 0x00 && hexData[2] == 0x00) //��鱨�ĵڶ�λ�Ƿ�Ϊ�㲥��ַ������ܹ��ܵ�ַ�����޸�
		{
			if (hexData[3] == 0xFF && hexData[4] == 0xFF && hexData[5] == 0xFF && hexData[6] == 0xFF && hexData[7] == 0xFF)
			{
				for (int k = 0; k < 9; k++) Serial.write(hexSent[k]);
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