
/*
******************************************************************
*                           头文件
*******************************************************************
*/
#include <rtthread.h>
#include "board.h"

#include "RTT_LED.h"
#include "key.h"


/*
******************************************************************
*                            定义的变量
*******************************************************************
*/


rt_thread_t led2_thread = RT_NULL;
rt_thread_t key_thread = RT_NULL;

rt_thread_t recive_thread = RT_NULL;
rt_thread_t send_thread = RT_NULL;

struct rt_messagequeue *test_mq = RT_NULL; 
static rt_mq_t test_mq1 = RT_NULL;        

static rt_sem_t test_sem = RT_NULL;

//ALIGN(RT_ALIGN_SIZE)
//rt_uint8_t led1_thread_stack[512];



/*
******************************************************************
*                            函数声明
*******************************************************************
*/
rt_uint32_t seedToKey (rt_uint32_t seed, rt_uint32_t MASK);

void led1_thread_entry(void* parameter);

void sendkey_thread_entry(void *parameter);

void key_thread_entry(void *parameter);

void send_thread_entry(void *parameter);
void recive_thread_entry(void *parameter); 

/*
******************************************************************
*                            主函数
*******************************************************************
*/
int main (void)
{
     
	  rt_kprintf("计数信号量测试！\n");
	  rt_kprintf("默认值为5个，按下K1申请车位，按下k2释放车位\n");
	
	  /**创建信号量***/
	 test_sem = rt_sem_create("test_sem",5,RT_IPC_FLAG_FIFO);
	if(test_sem != RT_NULL)
	{
		rt_kprintf("计数信号量创建成功\n");
	}
	
	  //rt_thread_init(&led1_thread,"led1",led1_thread_entry,RT_NULL,&led1_thread_stack[0],sizeof(led1_thread_stack),3,10);
	
	  //rt_thread_startup(&led1_thread);
	  
	  recive_thread = rt_thread_create("recive_thread",recive_thread_entry,RT_NULL,512,3,20);
	
	  if(recive_thread != RT_NULL)
			rt_thread_startup(recive_thread);
	  else 
			return -1;
		send_thread = rt_thread_create("send_thread",send_thread_entry,RT_NULL,512,2,20);
	
	  if(recive_thread != RT_NULL)
			rt_thread_startup(send_thread);
	  else 
			return -1;
	
	  led2_thread = rt_thread_create("sendkey",sendkey_thread_entry,RT_NULL,512,4,30);
	
	  if(led2_thread != RT_NULL)
			rt_thread_startup(led2_thread);
	  else 
			return -1;
		

		
		
}



/*
******************************************************************
*                            线程函数
*******************************************************************
*/
void led1_thread_entry(void *parameter)
{
	while(1)
	{
		LED1_ON;
		rt_thread_delay(200);
		rt_kprintf("LED1_ON\r\n");
		LED1_OFF;
		rt_thread_delay(200);
		rt_kprintf("LED1_OFF\r\n");
	}
}


void sendkey_thread_entry(void *parameter)
{
	rt_uint32_t seed = 0x4a;
	rt_uint32_t key = 0;
	while(1)
	{
		LED2_ON;
		seed++;
		rt_thread_delay(400);
		//rt_kprintf("LED2_ON\r\n");
		LED2_OFF;
		key = seedToKey (seed, 0x21015165);
		rt_kprintf("send = %x, key = %x\r\n",seed,key);
		rt_thread_delay(400);
		//rt_kprintf("LED2_OFF\r\n");
		
	}
}


void key_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	
	rt_uint32_t send_data1 = 101;
	rt_uint32_t send_data2 = 202;
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			
			uwRet = rt_mq_send(test_mq1,&send_data1,sizeof(send_data1));
      if(RT_EOK == uwRet)
			{
				rt_kprintf("发送队列数据1成功\n");
				
			}
      else {
				rt_kprintf("发送队列数据1失败，失败代码%s\n",uwRet);
			}			
		}
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			uwRet = rt_mq_send(test_mq1,&send_data2,sizeof(send_data1));
			
      if(RT_EOK == uwRet)
			{
				rt_kprintf("发送队列数据2成功\n");
				
			}
      else {
				rt_kprintf("发送队列数据2失败，失败代码%s\n",uwRet);
			}			
		}
		
		rt_thread_delay(20);
		
	}
}

void recive_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	while(1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
		   uwRet = rt_sem_take(test_sem,0);
			
			if(uwRet == RT_EOK)
		{
			rt_kprintf("成功申请一个车位\n");
		}
		
		else rt_kprintf("无车位可申请\n");
		}
		rt_thread_delay(20);
			
	
}
}

void send_thread_entry(void *parameter)
{
	rt_err_t uwRet = RT_EOK;
	while(1)
	{
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
		   uwRet = rt_sem_release(test_sem);
			
			if(uwRet == RT_EOK)
		{
			rt_kprintf("key2被单击：释放一个车位,value=%d\n",test_sem->value);
		}
		
		else rt_kprintf("key2被单击：但无位置可以释放\n");
	}
		rt_thread_delay(20);
		
	}
}



rt_uint32_t seedToKey (rt_uint32_t seed, rt_uint32_t MASK) 

{ 

rt_uint32_t key = 0; 
rt_uint8_t i = 0;

if (seed !=0 ) 

   { 

        for ( i=0; i<35; i++ ) 

         { 

           if (seed & 0x80000000) 

           { 

            seed = seed <<1; 

            seed = seed^MASK; 

           } 

          else 

           { 

            seed = seed << 1; 

           } 

       } 

      key = seed; 

  } 

return key; 
}
