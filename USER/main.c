#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "string.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "I2C.h"
#include "mpu6050.h"
#include "24l01.h"

//ALIENTEK战舰STM32开发板实验49
//触控USB鼠标 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司  		 	

 
//计算x1,x2的绝对值
u32 usb_abs(u32 x1,u32 x2)
{
	if(x1>x2)return x1-x2;
	else return x2-x1;
}
//设置USB 连接/断线
//enable:0,断开
//       1,允许连接	   
void usb_port_set(u8 enable)
{  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));//退出断电模式
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // 断电模式
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;    		  
	}
}  	

 int main(void)
 {	
	//	short temp1[3];//加速度计
	//	short temp2[3];
	//	short temp3;  //温度
	 u8 tmp_buf[5];	 //发射寄存器
		u8 key;
		short X,Y,X1,Y1;		 //发送到电脑端的坐标值
		
		u8 keysta;		//[0]:0,左键松开;1,左键按下;
						//[1]:0,右键松开;1,右键按下
						//[2]:0,中键松开;1,中键按下   		  
		delay_init();	    	 //延时函数初始化	  
		NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
		uart_init(9600);	 	//串口初始化为9600
		printf("串口初始化完毕\r\n");
// 		LED_Init();			     //LED端口初始化
// 		KEY_Init();	 			 //按键初始化
// 		printf("按键初始化完毕\r\n");
		I2C_Init();
		printf("IIC初始化完毕\r\n");
		delay_ms(100);
		MPU6050_Init();
		printf("MPU6050初始化完毕\r\n");
		delay_ms(100);
		NRF24L01_Init(); 
		printf("24l01初始化完毕\r\n");
		while(NRF24L01_Check())	//检查NRF24L01是否在位.	
		{
				printf("未检测到24l01\r\n");
				delay_ms(200);
		}		
		printf("检测到24l01---OK\r\n");
		NRF24L01_RX_Mode();
		printf("设置24l01为接收模式成功！\r\n");
		
		usb_port_set(0); 	//USB先断开
		delay_ms(300);
			usb_port_set(1);	//USB再次连接
		//USB配置
		USB_Interrupts_Config();    
		Set_USBClock();   
		USB_Init();	 
		printf("USB都初始化完毕了，骚年，开始接收吧！！！\r\n");		
 	while(1)
	{	 	
				if(NRF24L01_RxPacket(tmp_buf)==0)
		{
			X=(s16)((tmp_buf[1]<<8)|tmp_buf[2]);
			Y=(s16)((tmp_buf[3]<<8)|tmp_buf[4]);	
			printf(" %8d%8d\r\n",X,Y);	
			Joystick_Send(tmp_buf[0],X,Y,0);			
		}


 //		X1=(s16)((tmp_buf[1]<<8)|tmp_buf[2]);
 //		Y1=(s16)((tmp_buf[3]<<8)|tmp_buf[4]);
//printf("%8d%8d%4X\r\n",tmp_buf[1],tmp_buf[2],NRF24L01_TxPacket(tmp_buf));  //NRF24l01发射函数
//	Joystick_Send(keysta,X/15,Y/15,0);//鼠标函数，按键，X轴坐标，Y轴坐标



	}
}
	
