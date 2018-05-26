#include <SSD1306_text.h>//OLED库文件
#include <BH1750.h>//BH1750库文件
#include <Wire.h>//IIC库文件
#define OLED_RESET 4
BH1750 lightMeter;
SSD1306_text display(OLED_RESET);
int runLed = 7;
bool led = 0;//用来切换LED亮灭
			 /*------------------------------------------------------------------------------*/
unsigned char add1 = 0xc0;//设备地址，需要修改为需要的地址
unsigned char add2 = 0x18;//设备地址，需要修改为需要的地址
						  /*------------------------------------------------------------------------------*/
unsigned int maxLux = 0;//光强度最大值，最小0，最大54612
unsigned int minLux = 0;//光强度最小值，最小0，最大54612
unsigned char a = 0x00;//HEX输出亮度值高八位
unsigned char b = 0x00;//HEX输出亮度值低八位
					   /*------------------------------------------------------------------------------*/
unsigned char counter = 0;//计数器
unsigned char hexData[9] = {};//串口暂存
unsigned char zgbData[5] = {};//读取的zigbee数据串口暂存
unsigned char getConfig[7] = { 0xaa, 0xaa, 0x55, 0x55, 0xa5, 0xa5, 0xc3 }; //查询Zigbee Panid与频点指令
unsigned char hexSent[9] = { 0xb5, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5b }; //待返回数据
unsigned char pingOrder[9] = { 0xa5, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5a }; //PING指令
																					   /*------------------------------------------------------------------------------*/
void setup() {
	delay(1000);
	/*------------------------------------------------------------------------------*/
	Serial.begin(9600);
	lightMeter.begin();
	pinMode(runLed, OUTPUT);//运行LED
							/*------------------------------------------------------------------------------*/
	hexSent[1] = add1;
	hexSent[2] = add2;//设备地址写入数组
					  /*------------------------------------------------------------------------------*/
	display.init();//初始化OLED，显示标题等信息
	display.clear();
	display.setCursor(0, 27);
	display.setTextSize(1, 1);
	display.write("WSN-Lux V2.0");
	display.setCursor(1, 1);
	display.setTextSize(1, 0);
	display.write("-------------------------");
	/*------------------------------------------------------------------------------*/
	display.setCursor(2, 0);//显示菜单
	display.setTextSize(1, 1);
	display.write("Zigbee Panid:");
	/*------------------------------------------------------------------------------*/
	display.setCursor(3, 0);//显示菜单
	display.setTextSize(1, 1);
	display.write("Zigbee Channel:");
	/*------------------------------------------------------------------------------*/
	display.setCursor(4, 0);//显示菜单
	display.setTextSize(1, 1);
	display.write("WSN Dev Add:");
	/*---------------------------------------*/
	display.setCursor(4, 72);//在OLED显示设备硬件地址高位低位
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
bailout://标记点
	for (int k = 0; k < 7; k++) Serial.write(getConfig[k]); //发送查询Zigbee Panid与频点
	delay(1000);
	while (Serial.available() > 0)
	{
		for (int i = 0; i < 5; i++) zgbData[i] = char(Serial.read());
		delay(3);
	}//等接收完成
	delay(1000);

	if (zgbData[0] == 0xAA && zgbData[1] == 0xC3)//如果数据头正确就准备显示
	{
		display.setCursor(2, 78);//显示PANID高位
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[3]));
		display.setCursor(2, 84);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[3]));
		/*---------------------------------------*/
		display.setCursor(2, 90);//显示PANID低位
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[2]));
		display.setCursor(2, 96);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[2]));
		/*---------------------------------------*/
		display.setCursor(3, 90);//显示频点
		display.setTextSize(1, 1);
		display.write(hexH2ascii(zgbData[4]));
		display.setCursor(3, 96);
		display.setTextSize(1, 1);
		display.write(hexL2ascii(zgbData[4]));

	}
	else//如果不正确
	{
		counter++;//
		if (counter > 2)//计数器加1，计数器大于3就显示错误。小于三重新读取
		{
			display.setCursor(2, 78);//显示zigbee读取超时错误
			display.setTextSize(1, 1);
			display.write("error");
			display.setCursor(3, 90);//显示zigbee读取超时错误
			display.setTextSize(1, 1);
			display.write("error");
		}
		else
		{
			goto bailout;//返回标记点
		}
	}
	/*------------------------------------------------------------------------------*/
	uint16_t lux = lightMeter.readLightLevel();//写入最大最小值
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
	uint16_t lux = lightMeter.readLightLevel();//判断最大最小值，并存储
	if (lux > maxLux)
	{
		maxLux = lux;
	}
	if (lux < minLux)
	{
		minLux = lux;
	}
	/*------------------------------------------------------------------------------*/
	display.setCursor(5, 0);//显示最大值，根据数值大小清屏，防止闪烁
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
	display.setCursor(6, 0);//显示最小值，根据数值大小清屏，防止闪烁
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
	display.setCursor(7, 0);//显示当前值，根据数值大小清屏，防止闪烁
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
	while (Serial.available() > 0)//串口通讯功能
	{
		for (int i = 0; i < 9; i++) hexData[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);
	lux = lightMeter.readLightLevel();
	a = lux >> 8;
	b = lux & 0x00ff;
	if (hexData[0] == 0xA5 && hexData[8] == 0x5A) //检查数据包头与数据包尾
	{
		if (hexData[1] == add1 && hexData[2] == add2 || hexData[1] == 0x00 && hexData[2] == 0x00) //检查报文第二位是否为广播地址或数码管功能地址，需修改
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
char hexH2ascii(char x)//16进制取高位转字符
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
char hexL2ascii(char x)//16进制取低位转字符
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