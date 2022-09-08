/* **********************项目说明**********************/
/*实现任务的挂起和恢复
    用到的函数：创建任务(√)、两个不同的任务、按键函数
    用到的按键：KEY0:进入中断模式
               KEY1：输入模式（恢复任务1的运行）
               KEY2：输入模式（恢复任务2的运行）
               KEY_UP：输入模式（挂起任务1的运行）
*/


#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"



//LCD刷屏时使用的颜色
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   	 
						GREEN, CYAN,BROWN, 			
						BRRED, GRAY };

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define KEY_TASK_PRIO		2
//任务堆栈大小	
#define KEY_STK_SIZE 		128  
//任务句柄
TaskHandle_t KEYTask_Handler;
//任务函数
void key_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		3
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task1_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		4
//任务堆栈大小	
#define TASK2_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task2_Handler;
//任务函数
void task2_task(void *pvParameters);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				//延时函数初始化	 
	uart_init(9600);					//初始化串口
	LED_Init();		  					//初始化LED
	KEY_Init();							//初始化按键
	EXTIX_Init();						//初始化外部中断
	LCD_Init();							//初始化LCD
	
 

		POINT_COLOR=RED;	  
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建KEY任务
    xTaskCreate((TaskFunction_t )key_task,     	
                (const char*    )"key_task",   	
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEY_TASK_PRIO,	
                (TaskHandle_t*  )&KEYTask_Handler);   
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,     
                (const char*    )"task1_task",   
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1_Handler);    
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2_Handler);                

    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//key任务函数
void key_task(void *pvParameters)
{
	u8 key,statflag=0;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case WKUP_PRES:
				statflag=!statflag;
				if(statflag==1)
				{
					vTaskSuspend(Task1_Handler);//挂起任务
					printf("挂起任务1的运行!\r\n");
					LCD_ShowString(30,40,200,24,24,"Stop task 1");
				}
				else if(statflag==0)
				{
					vTaskResume(Task1_Handler);	//恢复任务1
					printf("恢复任务1的运行!\r\n");
					LCD_ShowString(30,40,200,24,24,"Run  task 1");
				}		
				break;
			case KEY1_PRES:
				vTaskSuspend(Task2_Handler);//挂起任务2
				printf("挂起任务2的运行!\r\n");
			LCD_ShowString(30,40,200,24,24,"Run  task 2");
				break;
		}
		vTaskDelay(10);			//延时10ms 
	}
}  

//TASK1任务函数
void task1_task(void *pvParameters)
{
    u8 task1_num=0;
    POINT_COLOR=BLACK;

    LCD_DrawRectangle(5,110,115,314);
    LCD_DrawLine(5,130,115,130);
    POINT_COLOR=BLUE;
    LCD_ShowString(6,111,110,16,16,"Task1 run:000");
    while(1)
    {
        task1_num++;
        LED0=!LED0;
        printf("任务1已运行：%d次\r\n",task1_num);
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]);//填充区域
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);//实现任务执行次数
        vTaskDelay(1000);
    }
}

//TASK2任务函数
void task2_task(void *pvParameters)
{
    u8 task2_num=0;
    POINT_COLOR=BLACK;

	LCD_DrawRectangle(125,110,234,314); //画一个矩形	
	LCD_DrawLine(125,130,234,130);		//画线
    POINT_COLOR=BLUE;
	LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
    while(1)
    {
        task2_num++;
        LED1=!LED1;
		printf("任务2已经执行：%d次\r\n",task2_num);
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //显示任务执行次数
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //填充区域
        vTaskDelay(1000);
    }
}

