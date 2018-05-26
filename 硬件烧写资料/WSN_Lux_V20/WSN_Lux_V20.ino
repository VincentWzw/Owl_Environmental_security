#include <SSD1306_text.h>//OLED���ļ�
#include <BH1750.h>//BH1750���ļ�
#include <Wire.h>//IIC���ļ�
#define OLED_RESET 4
BH1750 lightMeter;
SSD1306_text display(OLED_RESET);
int runLed = 7;
bool led = 0;//�����л�LED����
			 /*------------------------------------------------------------------------------*/
unsigned char add1 = 0xc0;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
unsigned char add2 = 0x18;//�豸��ַ����Ҫ�޸�Ϊ��Ҫ�ĵ�ַ
						  /*------------------------------------------------------------------------------*/
unsigned int maxLux = 0;//��ǿ�����ֵ����С0�����54612
unsigned int minLux = 0;//��ǿ����Сֵ����С0�����54612
unsigned char a = 0x00;//HEX�������ֵ�߰�λ
unsigned char b = 0x00;//HEX�������ֵ�Ͱ�λ
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
	lightMeter.begin();
	pinMode(runLed, OUTPUT);//����LED
							/*------------------------------------------------------------------------------*/
	hexSent[1] = add1;
	hexSent[2] = add2;//�豸��ַд������
					  /*------------------------------------------------------------------------------*/
	display.init();//��ʼ��OLED����ʾ�������Ϣ
	display.clear();
	display.setCursor(0, 27);
	display.setTextSize(1, 1);
	display.write("WSN-Lux V2.0");
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
	uint16_t lux = lightMeter.readLightLevel();//д�������Сֵ
	delay(50);
	minLux = lux;
	maxLux = lux;
	delay(50);
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
	/*------------------------------------------------------------------------------*/
	uint16_t lux = lightMeter.readLightLevel();//�ж������Сֵ�����洢
	if (lux > maxLux)
	{
		maxLux = lux;
	}
	if (lux < minLux)
	{
		minLux = lux;
	}
	/*------------------------------------------------------------------------------*/
	display.setCursor(5, 0);//��ʾ���ֵ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("Max:");
	if (maxLux < 10)
	{
		display.setCursor(5, 31);
		display.setTextSize(1, 1);
		display.write("                ");
	}
	else if (maxLux < 100)
	{
		display.setCursor(5, 37);
		display.setTextSize(1, 1);
		display.write("               ");
	}
	else if (maxLux < 1000)
	{
		display.setCursor(5, 43);
		display.setTextSize(1, 1);
		display.write("              ");
	}
	else if (maxLux < 10000)
	{
		display.setCursor(5, 49);
		display.setTextSize(1, 1);
		display.write("             ");
	}
	display.setCursor(5, 25);
	display.setTextSize(1, 1);
	display.print(maxLux);
	/*------------------------------------------------------------------------------*/
	display.setCursor(6, 0);//��ʾ��Сֵ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("Min:");
	if (minLux < 10)
	{
		display.setCursor(6, 31);
		display.setTextSize(1, 1);
		display.write("                ");
	}
	else if (minLux < 100)
	{
		display.setCursor(6, 37);
		display.setTextSize(1, 1);
		display.write("               ");
	}
	else if (minLux < 1000)
	{
		display.setCursor(6, 43);
		display.setTextSize(1, 1);
		display.write("              ");
	}
	else if (minLux < 10000)
	{
		display.setCursor(6, 49);
		display.setTextSize(1, 1);
		display.write("             ");
	}
	display.setCursor(6, 25);
	display.setTextSize(1, 1);
	display.print(minLux);
	/*------------------------------------------------------------------------------*/
	display.setCursor(7, 0);//��ʾ��ǰֵ��������ֵ��С��������ֹ��˸
	display.setTextSize(1, 1);
	display.write("Lux:");
	if (lux < 10)
	{
		display.setCursor(7, 31);
		display.setTextSize(1, 1);
		display.write("                ");
	}
	else if (lux < 100)
	{
		display.setCursor(7, 37);
		display.setTextSize(1, 1);
		display.write("               ");
	}
	else if (lux < 1000)
	{
		display.setCursor(7, 43);
		display.setTextSize(1, 1);
		display.write("              ");
	}
	else if (lux < 10000)
	{
		display.setCursor(7, 49);
		display.setTextSize(1, 1);
		display.write("             ");
	}
	display.setCursor(7, 25);
	display.setTextSize(1, 1);
	display.print(lux);
	/*------------------------------------------------------------------------------*/
	while (Serial.available() > 0)//����ͨѶ����
	{
		for (int i = 0; i < 9; i++) hexData[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);
	lux = lightMeter.readLightLevel();
	a = lux >> 8;
	b = lux & 0x00ff;
	if (hexData[0] == 0xA5 && hexData[8] == 0x5A) //������ݰ�ͷ�����ݰ�β
	{
		if (hexData[1] == add1 && hexData[2] == add2 || hexData[1] == 0x00 && hexData[2] == 0x00) //��鱨�ĵڶ�λ�Ƿ�Ϊ�㲥��ַ������ܹ��ܵ�ַ�����޸�
		{
			if (hexData[3] == 0xFF && hexData[4] == 0xFF && hexData[5] == 0xFF && hexData[6] == 0xFF && hexData[7] == 0xFF)
			{
				hexSent[3] = a;
				hexSent[4] = b;
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