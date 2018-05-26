#include <SSD1306_text.h>//OLED库文件
#include <Wire.h>//IIC库文件
#include <dht.h> 
#define DHT11_PIN A3//put the sensor in the  pin A3
#define OLED_RESET 5
dht DHT;
SSD1306_text display(OLED_RESET);
bool led = 0;//用来切换LED亮灭
/*------------------------------------------------------------------------------*/
unsigned char add1 = 0x30;//设备地址，需要修改为需要的地址
unsigned char add2 = 0x68;//设备地址，需要修改为需要的地址
/*------------------------------------------------------------------------------*/
int temp = 0;//最小0，最大99
int humi = 0;//最小0，最大99
int runLed = 7;

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

	pinMode(runLed, OUTPUT);//运行LED
	/*------------------------------------------------------------------------------*/
	hexSent[1] = add1;
	hexSent[2] = add2;//设备地址写入数组
	/*------------------------------------------------------------------------------*/
	display.init();//初始化OLED，显示标题等信息
	display.clear();
	display.setCursor(0, 36);
	display.setTextSize(1, 1);
	display.write("WSN-S V2.0");
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


	int chk = DHT.read11(DHT11_PIN);
	switch (chk)
	{
	case 0:
	{
		temp = DHT.temperature;
		humi = DHT.humidity;
		hexSent[3] = 0x00;
		hexSent[4] = temp;
		hexSent[5] = 0x00;
		hexSent[6] = humi;
		/*------------------------------------------------------------------------------*/
		display.setCursor(5, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RT:");
		if (temp < 10)
		{
			display.setCursor(5, 24);
			display.setTextSize(1, 1);
			display.write("c                ");
		}
		else if (temp < 100)
		{
			display.setCursor(5, 30);
			display.setTextSize(1, 1);
			display.write("c               ");
		}
		else if (temp < 1000)
		{
			display.setCursor(5, 36);
			display.setTextSize(1, 1);
			display.write("c              ");
		}
		display.setCursor(5, 18);
		display.setTextSize(1, 1);
		display.print(temp);
		/*------------------------------------------------------------------------------*/
		display.setCursor(6, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RH:");
		if (humi < 10)
		{
			display.setCursor(6, 24);
			display.setTextSize(1, 1);
			display.write("%                ");
		}
		else if (humi < 100)
		{
			display.setCursor(6, 30);
			display.setTextSize(1, 1);
			display.write("%               ");
		}
		else if (humi < 1000)
		{
			display.setCursor(6, 36);
			display.setTextSize(1, 1);
			display.write("%              ");
		}
		display.setCursor(6, 18);
		display.setTextSize(1, 1);
		display.print(humi);
		/*------------------------------------------------------------------------------*/
	}break;
	case -1:
	{
		temp = DHT.temperature;
		humi = DHT.humidity;
		hexSent[3] = 0xDD;
		hexSent[4] = temp;
		hexSent[5] = 0xDD;
		hexSent[6] = humi;
		/*------------------------------------------------------------------------------*/
		display.setCursor(5, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RT:");
		display.setCursor(5, 18);
		display.setTextSize(1, 1);
		display.write("Checksum error    ");
		/*------------------------------------------------------------------------------*/
		display.setCursor(6, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RH:");
		display.setCursor(6, 18);
		display.setTextSize(1, 1);
		display.write("Checksum error    ");
		/*------------------------------------------------------------------------------*/
	}break;
	case -2:
	{
		temp = DHT.temperature;
		humi = DHT.humidity;
		hexSent[3] = 0xEE;
		hexSent[4] = temp;
		hexSent[5] = 0xEE;
		hexSent[6] = humi;
		/*------------------------------------------------------------------------------*/
		display.setCursor(5, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RT:");
		display.setCursor(5, 18);
		display.setTextSize(1, 1);
		display.write("Not connected     ");
		/*------------------------------------------------------------------------------*/
		display.setCursor(6, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RH:");
		display.setCursor(6, 18);
		display.setTextSize(1, 1);
		display.write("Not connected     ");
		/*------------------------------------------------------------------------------*/
	}break;
	default:
	{
		temp = DHT.temperature;
		humi = DHT.humidity;
		hexSent[3] = 0xFF;
		hexSent[4] = temp;
		hexSent[5] = 0xFF;
		hexSent[6] = humi;
		/*------------------------------------------------------------------------------*/
		display.setCursor(5, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RT:");
		display.setCursor(5, 18);
		display.setTextSize(1, 1);
		display.write("Unknown error");
		/*------------------------------------------------------------------------------*/
		display.setCursor(6, 0);//显示，根据数值大小清屏，防止闪烁
		display.setTextSize(1, 1);
		display.write("RH:");
		display.setCursor(6, 18);
		display.setTextSize(1, 1);
		display.write("Unknown error");
		/*------------------------------------------------------------------------------*/
	}break;
	}

	

	/*------------------------------------------------------------------------------*/
	while (Serial.available() > 0)//串口通讯功能
	{
		for (int i = 0; i < 9; i++) hexData[i] = char(Serial.read());
		delay(3);
	}
	delay(1000);

	if (hexData[0] == 0xA5 && hexData[8] == 0x5A)//检查数据包头与数据包尾
	{
		if (hexData[1] == add1&&hexData[2] == add2 || hexData[1] == 0x00 && hexData[2] == 0x00)//检查报文第二位是否为广播地址或数码管功能地址，需修改
		{
			if (hexData[3] == 0xFF && hexData[4] == 0xFF && hexData[5] == 0xFF && hexData[6] == 0xFF && hexData[7] == 0xFF)
			{
				for (int k = 0; k<9; k++) Serial.write(hexSent[k]);
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