#include "main.h"

#define P5_ON 1
#define LED_P5_FULL P5_ON

#define setbit(sfr,bit)		(sfr|=(0x01<<bit))
#define clrbit(sfr,bit)		(sfr&=~(1<<bit))

char check_bit(unsigned char byte,unsigned char bit){
	if((byte&(1<<bit))==0x00){
		return 0;
	}
	else{
		return 1;
	}
}

const unsigned short mangtg[]={40,80,160,320,640,1280,2560,5120};
uint16_t manght[32][128];
unsigned char mangxuatled[16][5][128];

#if (LED_P5_FULL == P5_ON)
unsigned char mangxuatled[16][5][128];
void giaimap5fullmau(){
	unsigned char hang,cot,pixel,pwm;
	for(hang=0;hang<16;hang++){
		 for(pwm=0,pixel=0x01;pwm<5;pixel<<=1,pwm++){
			 for(cot=0;cot<128;cot++){
					   mangxuatled[hang][pwm][cot]=0;
						 if((manght[hang][cot] & (pixel<<11))) {mangxuatled[hang][pwm][cot] |= 0x01;}
						 if((manght[hang+16][cot] & (pixel<<11))) {mangxuatled[hang][pwm][cot] |= 0x02;}
						 if((manght[hang][cot] & (pixel<<5))) {mangxuatled[hang][pwm][cot] |= 0x04;}
						 if((manght[hang+16][cot] & (pixel<<5))) {mangxuatled[hang][pwm][cot] |= 0x08;}
						 if((manght[hang][cot] & pixel)) {mangxuatled[hang][pwm][cot] |= 0x10;}
						 if((manght[hang+16][cot] & pixel)) {mangxuatled[hang][pwm][cot] |= 0x20;}
			 }
		 }
	}
}
#endif

union tinhieudk
{
	unsigned char bytedk;
  struct 
	{
		int  hang:4;
		int  tgian :3;
		int  chot:1;			
	}thanhphan;
} bytedieukhien;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	    if(htim->Instance ==TIM1)
			{
				 for(int ___i=0;___i<128;___i++)
			   {
					  GPIOD->ODR = mangxuatled[bytedieukhien.thanhphan.hang][bytedieukhien.thanhphan.tgian%5][___i];
					 
					  GPIOD->ODR |= 0x0040;
					 
				 }
				   GPIOA -> ODR |= (1<<4);
					 TIM1->ARR = mangtg[bytedieukhien.thanhphan.tgian];
					 GPIOA -> ODR = bytedieukhien.thanhphan.hang;
					 bytedieukhien.bytedk++;
			}
}
void MATRIX_pixel_color(unsigned char x, unsigned char y,unsigned char r,unsigned char g,unsigned char b){
	
	if(x>=32 || y>127) return;
	if(y<0 || x<0) return;
	manght[x][y]=(((r & 0xF8) | (g >> 5))<<8)|(((g & 0x1C) << 3) | (b  >> 3));
}

void MATRIX_putchar8x8(unsigned char x,unsigned char y,unsigned char chu,unsigned char r,unsigned char g,unsigned char b){
	chu=chu-32;
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
				if(check_bit(myfont8x8[chu][i],j)){
					MATRIX_pixel_color(x+i,y+j,r,g,b);
				}
		}
	}
}

void MATRIX_putStr8x8(unsigned char x,unsigned char y,unsigned char a[],unsigned char r,unsigned char g,unsigned char b){
	unsigned char i=0;
	while(a[i]!=0x00){
		 MATRIX_putchar8x8(x,y,a[i],r,g,b);
		 i++;
		 if(a[i] == ' ') y=y+3;
		 else y=y+7;
	}
}
void MATRIX_putchar16x16(unsigned char x,unsigned char y,unsigned char chu,unsigned char r,unsigned char g,unsigned char b){
	 for(int i=0;i<32;i=i+2)
		 for(int j=0;j<16;j++)
		 {
			 if(check_bit(font11[chu][i+j/8],(15-j)%8)){
					if(x+i/2 >128 || x+i/2<0 ) return;
					if(y+j >255 || y+j<0)  return;
				   MATRIX_pixel_color(x+i/2,y+j,r,g,b);
			}		 
		}
}
void MATRIX_putStr16x16(signed int x,signed int y,unsigned char a[],unsigned char r,unsigned char g,unsigned char b){
	unsigned char i=0;
	while(a[i]!=0x00){
		 MATRIX_putchar16x16(x,y,a[i],r,g,b);
		 i++;
		 if(a[i] ==' ') y=y+3;
		 else y=y+13;
	}
}
void MATRIX_putchar16x32(unsigned char x,unsigned char y,unsigned char chu,unsigned char r,unsigned char g,unsigned char b){
	for(int i=0;i<64;i=i+2){
		for(int j=0;j<16;j++){
				if(check_bit(myfont16x32[chu][i+j/8],(15-j)%8)){
					MATRIX_pixel_color(x+i/2,y+j,r,g,b);
				}
		}
	}
}
void MATRIX_putStr16x32(unsigned char x,unsigned char y,unsigned char a[],unsigned char r,unsigned char g,unsigned char b){
	unsigned char i=0;
	while(a[i]!=0x00){
		 MATRIX_putchar16x32(x,y,a[i],r,g,b);
		 i++;
		 if(a[i] == ' ') y=y+10;
		 else y=y+15;
	}
}

void giaimaanh3(unsigned char *anh, unsigned int dai, unsigned int rong, signed int x, signed int y)
{
  unsigned char n,hang=0,cot=0;
   for(n=0;n<(dai*rong);n++)
   {   
				 MATRIX_pixel_color(y+cot,x+hang,anh[n],anh[dai*rong+n],anh[2*dai*rong+n]);
         cot++;
         if(cot==dai) {cot=0;hang++;}
   }
}

void MATRIX_pixel_color565(unsigned char x, unsigned char y,uint16_t mau){
	  manght[y][x]=mau;
}
void drawImage(int x, int y,int w,int h,uint16_t *a)
{
  int imageHeight = h;
  int imageWidth = w;
  int counter = 0;
  for (int yy = 0; yy < imageHeight; yy++)
  {
    for (int xx = 0; xx < imageWidth; xx++)
    {
			MATRIX_pixel_color565(xx+x,yy+y,a[counter]);
      counter++;
    }
  }
}

