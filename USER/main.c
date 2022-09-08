/* **********************��Ŀ˵��**********************/
/*ʵ������Ĺ���ͻָ�
    �õ��ĺ�������������(��)��������ͬ�����񡢰�������
    �õ��İ�����KEY0:�����ж�ģʽ
               KEY1������ģʽ���ָ�����1�����У�
               KEY2������ģʽ���ָ�����2�����У�
               KEY_UP������ģʽ����������1�����У�
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



//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   	 
						GREEN, CYAN,BROWN, 			
						BRRED, GRAY };

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO		2
//�����ջ��С	
#define KEY_STK_SIZE 		128  
//������
TaskHandle_t KEYTask_Handler;
//������
void key_task(void *pvParameters);

//�������ȼ�
#define TASK1_TASK_PRIO		3
//�����ջ��С	
#define TASK1_STK_SIZE 		128  
//������
TaskHandle_t Task1_Handler;
//������
void task1_task(void *pvParameters);

//�������ȼ�
#define TASK2_TASK_PRIO		4
//�����ջ��С	
#define TASK2_STK_SIZE 		128  
//������
TaskHandle_t Task2_Handler;
//������
void task2_task(void *pvParameters);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				//��ʱ������ʼ��	 
	uart_init(9600);					//��ʼ������
	LED_Init();		  					//��ʼ��LED
	KEY_Init();							//��ʼ������
	EXTIX_Init();						//��ʼ���ⲿ�ж�
	LCD_Init();							//��ʼ��LCD
	
 

		POINT_COLOR=RED;	  
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����KEY����
    xTaskCreate((TaskFunction_t )key_task,     	
                (const char*    )"key_task",   	
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEY_TASK_PRIO,	
                (TaskHandle_t*  )&KEYTask_Handler);   
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,     
                (const char*    )"task1_task",   
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1_Handler);    
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2_Handler);                

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//key������
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
					vTaskSuspend(Task1_Handler);//��������
					printf("��������1������!\r\n");
					LCD_ShowString(30,40,200,24,24,"Stop task 1");
				}
				else if(statflag==0)
				{
					vTaskResume(Task1_Handler);	//�ָ�����1
					printf("�ָ�����1������!\r\n");
					LCD_ShowString(30,40,200,24,24,"Run  task 1");
				}		
				break;
			case KEY1_PRES:
				vTaskSuspend(Task2_Handler);//��������2
				printf("��������2������!\r\n");
			LCD_ShowString(30,40,200,24,24,"Run  task 2");
				break;
		}
		vTaskDelay(10);			//��ʱ10ms 
	}
}  

//TASK1������
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
        printf("����1�����У�%d��\r\n",task1_num);
        LCD_Fill(6,131,114,313,lcd_discolor[task1_num%14]);//�������
        LCD_ShowxNum(86,111,task1_num,3,16,0x80);//ʵ������ִ�д���
        vTaskDelay(1000);
    }
}

//TASK2������
void task2_task(void *pvParameters)
{
    u8 task2_num=0;
    POINT_COLOR=BLACK;

	LCD_DrawRectangle(125,110,234,314); //��һ������	
	LCD_DrawLine(125,130,234,130);		//����
    POINT_COLOR=BLUE;
	LCD_ShowString(126,111,110,16,16,"Task2 Run:000");
    while(1)
    {
        task2_num++;
        LED1=!LED1;
		printf("����2�Ѿ�ִ�У�%d��\r\n",task2_num);
		LCD_ShowxNum(206,111,task2_num,3,16,0x80);  //��ʾ����ִ�д���
		LCD_Fill(126,131,233,313,lcd_discolor[13-task2_num%14]); //�������
        vTaskDelay(1000);
    }
}

